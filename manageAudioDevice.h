#ifndef MANAGEAUDIODEVICE_H
#define MANAGEAUDIODEVICE_H

#include <QString>
#include <condition_variable>
#include <QObject>
#include <MMDeviceAPI.h>
#include <SDL.h>

class manageAudioDevice : public QObject
{
    Q_OBJECT
signals:
    void changeAudioName();

public:
    QString getDefaultAudioName();
    void pollAudioName();
    void changeAudioDevice(bool isOnPlayingFlag, HMODULE sdl, SDL_AudioSpec &wanted_spec);

public:
    QString deviceName;
    enum deviceStatus
    {
        ENABLE,
        DISABLE
    } audioStatus;
    std::mutex devicemut;
    std::condition_variable deviceCv;
    manageAudioDevice();
    void run();
};

#endif // MANAGEAUDIODEVICE_H
