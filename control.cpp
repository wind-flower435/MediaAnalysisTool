#include "control.h"

#include <QMetaType>
#include <MMDeviceAPI.h>
#include <functiondiscoverykeys_devpkey.h>

#pragma comment(lib, "Ole32.lib")

control::control()
{
    qRegisterMetaType<QImage>("QImage&");

    m_videoHandle = new video();
    m_audioHandle = new audio();
    m_manageDEV = new manageAudioDevice();

    _conn();
    _init_isNotPausedButton();
}

video *control::getVideoHandle()
{
    return m_videoHandle;
}

audio *control::getAudioHandle()
{
    return m_audioHandle;
}

void control::_conn()
{
    connect(m_videoHandle, &video::noSupported, this, [this]
            { emit this->noSupported(); });
    connect(m_videoHandle, &video::s, this, [this](QImage &img)
            { emit s(img); });
    connect(m_videoHandle, &video::noSuitVideoCodec, this, [this]
            { emit this->noSuitVideoCodec(); });
    connect(m_audioHandle, &audio::noSuitAudioCodec, this, [this]
            { emit this->noSuitAudioCodec(); });
    connect(m_videoHandle, &video::fileError, this, [this]
            { emit this->videoFileError(); });
    connect(m_audioHandle, &audio::fileError, this, [this]
            { emit this->audioFileError(); });
    connect(m_videoHandle, &video::Custom_Signal, this, [this](QString Qstr)
            { emit this->Custom_Signal(Qstr); });
    connect(m_audioHandle, &audio::Custom_Signal, this, [this](QString Qstr)
            { emit this->Custom_Signal(Qstr); });

    connect(m_manageDEV, &manageAudioDevice::changeAudioName, this,
            [this]
            {
                isNotPausedButtonText = m_manageDEV->deviceName;
                emit this->changeButtonText();
            });
}

void control::_init_isNotPausedButton()
{
    isNotPausedButtonText = m_manageDEV->getDefaultAudioName();
    pollAudioName();
}

void control::_fetchVersionFfDll()
{
    m_FfDllVersion = m_videoHandle->fetchVersionFfDll();
    m_videoHandle->m_VersionFfDll = this->m_FfDllVersion;
    m_audioHandle->m_VersionFfDll = this->m_FfDllVersion;
}

void control::pollAudioName()
{
    m_manageDEV->run();
}

void control::changeAudioDevice()
{
    m_audioHandle->isaudioDeviceCloseFlag = m_manageDEV->audioStatus == manageAudioDevice::ENABLE ? true : false;
    m_manageDEV->changeAudioDevice(m_videoHandle->isOnPlayFlag || m_audioHandle->isOnPlayFlag, audio::sdl, m_audioHandle->wanted_spec);
}

void control::begin()
{
    m_videoHandle->m_helpInit();
    m_audioHandle->m_helpInit();
    _fetchVersionFfDll();
    isVideoFile = m_videoHandle->begin();
    isAudioFile = m_audioHandle->begin();
}

void control::getFileName(std::string str)
{
    strcpy(filePath, str.data());
}

void control::isPausedAudio()
{
    this->isNotPaused = !this->isNotPaused;
    m_audioHandle->isNotPaused = this->isNotPaused;
    if (this->isNotPaused)
    {
        isNotPausedButtonText = isNotPausedButtonText.left(isNotPausedButtonText.indexOf(":"));
        isNotPausedButtonText += ":open";
        emit changeButtonText();
        return;
    }
    isNotPausedButtonText = isNotPausedButtonText.left(isNotPausedButtonText.indexOf(":"));
    isNotPausedButtonText += ":mute";
    emit changeButtonText();
}

void control::play()
{
    if (isVideoFile && !isAudioFile)
    {
        m_videoHandle->play();
        return;
    }
    if (!isVideoFile && isAudioFile)
    {
        m_audioHandle->play();
        return;
    }
    if (isVideoFile && isAudioFile)
    {
        m_videoHandle->play();
        m_audioHandle->play();
    }
}

void control::getMediaInfo()
{

    m_format = QString(m_videoHandle->pFormatCtx->iformat->name);
    m_duration = static_cast<double>(m_videoHandle->pFormatCtx->duration) / 1000000;

    if (m_videoHandle->videoIndex >= 0 && m_audioHandle->audioIndex < 0)
    {
        m_frameRate = static_cast<double>(m_videoHandle->pFormatCtx->streams[m_videoHandle->videoIndex]->avg_frame_rate.num) 
                                        / m_videoHandle->pFormatCtx->streams[m_videoHandle->videoIndex]->avg_frame_rate.den;
        m_frameNums = m_videoHandle->pFormatCtx->streams[m_videoHandle->videoIndex]->nb_frames > 0
                          ? m_videoHandle->pFormatCtx->streams[m_videoHandle->videoIndex]->nb_frames
                          : m_frameRate * m_duration;
        if (m_videoHandle->pCodecCtx->codec_descriptor)
            m_videoDecode = m_videoHandle->pCodecCtx->codec_descriptor->name;
        m_rotate = m_videoHandle->getRotateAngle();

        return;
    }

    if (m_audioHandle->audioIndex >= 0 && m_videoHandle->videoIndex < 0)
    {
        if (m_audioHandle->aCodecCtx->codec_descriptor)
            m_audioDecode = m_audioHandle->aCodecCtx->codec_descriptor->name;
        return;
    }

    if (m_videoHandle->pCodecCtx->codec_descriptor)
        m_videoDecode = m_videoHandle->pCodecCtx->codec_descriptor->name;
    if (m_audioHandle->aCodecCtx->codec_descriptor)
        m_audioDecode = m_audioHandle->aCodecCtx->codec_descriptor->name;

    m_rotate = m_videoHandle->getRotateAngle();

    m_frameRate = static_cast<double>(m_videoHandle->pFormatCtx->streams[m_videoHandle->videoIndex]->avg_frame_rate.num)
                                      / m_videoHandle->pFormatCtx->streams[m_videoHandle->videoIndex]->avg_frame_rate.den;
    m_frameNums = m_videoHandle->pFormatCtx->streams[m_videoHandle->videoIndex]->nb_frames > 0
                      ? m_videoHandle->pFormatCtx->streams[m_videoHandle->videoIndex]->nb_frames
                      : m_frameRate * m_duration;
}

void control::forceFinishPlay()
{
    m_videoHandle->forceFinishVideo();
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); //等待结束完成
    m_audioHandle->forceFinishAudio();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}
