#include "manageAudioDevice.h"
#include "sdl2Dll.h"

#include <functiondiscoverykeys_devpkey.h>
#include <setupapi.h>
#include <devguid.h>

#pragma comment(lib, "Setupapi.lib")

manageAudioDevice::manageAudioDevice()
{
    audioStatus = ENABLE;
}

void manageAudioDevice::run()
{
    std::thread(&manageAudioDevice::pollAudioName, this).detach();
}

QString manageAudioDevice::getDefaultAudioName()
{
    IMMDeviceEnumerator *ptrEnumerator = nullptr;
    IMMDevice *pDevice = nullptr;
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                  __uuidof(IMMDeviceEnumerator), reinterpret_cast<void **>(&ptrEnumerator));
    hr = ptrEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    IPropertyStore *pPropertyStore = nullptr;
    PROPVARIANT pv;
    PropVariantInit(&pv);
    if (!pDevice)
    {
        deviceName = QString::fromStdWString(L"音频设备关闭");
        return deviceName;
    }
    hr = pDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
    hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &pv);
    deviceName = QString::fromStdWString(pv.pwszVal);
    deviceName = deviceName.left(deviceName.indexOf(" "));
    deviceName += ":open";
    return deviceName;
}

void manageAudioDevice::pollAudioName()
{
    QString str;
    IMMDeviceEnumerator *ptrEnumerator = nullptr;
    IMMDevice *pDevice = nullptr;
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                  __uuidof(IMMDeviceEnumerator), reinterpret_cast<void **>(&ptrEnumerator));

    IPropertyStore *pPropertyStore = nullptr;
    PROPVARIANT pv;

    while (1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(600));

        std::unique_lock<std::mutex> lk(devicemut);
        deviceCv.wait(lk, [this]
                      { return audioStatus == ENABLE; });

        hr = ptrEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
        PropVariantInit(&pv);
        if (!pDevice)
            continue;
        hr = pDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
        hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &pv);
        lk.unlock();

        str = QString::fromStdWString(pv.pwszVal);
        str = str.left(str.indexOf(" "));
        if (str != deviceName.left(deviceName.indexOf(":")))
        {
            QString temp = deviceName.mid(deviceName.indexOf(":"), deviceName.size() - deviceName.indexOf(":"));
            deviceName = str;
            deviceName += temp;
            emit changeAudioName();
        }
    }
}

void manageAudioDevice::changeAudioDevice(bool isOnPlayingFlag, HMODULE sdl, SDL_AudioSpec &wanted_spec)
{
    P_SDL_CloseAudio c_SDL_CloseAudio = (P_SDL_CloseAudio)GetProcAddress(sdl, "SDL_CloseAudio");
    P_SDL_OpenAudio c_SDL_OpenAudio = (P_SDL_OpenAudio)GetProcAddress(sdl, "SDL_OpenAudio");
    P_SDL_PauseAudio c_SDL_PauseAudio = (P_SDL_PauseAudio)GetProcAddress(sdl, "SDL_PauseAudio");

    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DeviceInfoData;
    hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_MEDIA, NULL, NULL, DIGCF_PRESENT);
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    audioStatus = audioStatus == ENABLE ? DISABLE : ENABLE;

    for (int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
    {
        DWORD DataT;
        LPTSTR buffer = NULL;
        DWORD buffersize = 0;

        while (!SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_DEVICEDESC, &DataT, (PBYTE)buffer, buffersize, &buffersize))
        {
            if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
                break;

            if (buffer)
                LocalFree(buffer);
            buffer = (LPTSTR)(char *)LocalAlloc(LPTR, buffersize);
        }
        if (!buffer)
            continue;

        std::wstring str;
        str = buffer;
        if (str == L"Realtek Audio")
        {
            LocalFree(buffer);
            break;
        }
        LocalFree(buffer);
    }

    SP_PROPCHANGE_PARAMS propChange = {sizeof(SP_CLASSINSTALL_HEADER)};
    propChange.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
    propChange.Scope = DICS_FLAG_GLOBAL;

    if (isOnPlayingFlag && audioStatus == DISABLE) //正在播放且音频设备需要关闭
    {
        c_SDL_CloseAudio();
    }

    propChange.StateChange = audioStatus == ENABLE ? DICS_ENABLE : DICS_DISABLE;
    propChange.HwProfile = 0;
    bool rlt = false;

    if (DeviceInfoData.DevInst == 0)
    {
        return;
    }

    devicemut.lock();
    rlt = SetupDiSetClassInstallParams(hDevInfo, &DeviceInfoData, (SP_CLASSINSTALL_HEADER *)&propChange, sizeof(propChange));

    if (!rlt)
    {
        return;
    }

    rlt = SetupDiChangeState(hDevInfo, &DeviceInfoData);
    rlt = SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo, &DeviceInfoData);
    SetupDiDestroyDeviceInfoList(hDevInfo);
    devicemut.unlock();

    if (isOnPlayingFlag && audioStatus == ENABLE) //正在播放且需要开启所有音频设备
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (c_SDL_OpenAudio(&wanted_spec, nullptr) < 0)
            return;
        c_SDL_PauseAudio(0);
    }
    deviceCv.notify_one();
}
