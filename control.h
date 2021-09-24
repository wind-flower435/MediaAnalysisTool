#ifndef CONTROL_H
#define CONTROL_H

#include "video.h"
#include "audio.h"
#include "manageAudioDevice.h"

class control : public QObject
{
    Q_OBJECT

signals:
    void changeButtonText();

    //错误信息
    void noSupported();
    void noSuitAudioCodec();
    void noSuitVideoCodec();
    void videoFileError();
    void audioFileError();
    void FileError();
    void Custom_Signal(QString);

    //帧图片
    void s(QImage &);

public:
    //媒体文件信息
    QString m_format;
    QString m_videoDecode;
    QString m_audioDecode;
    double m_duration = 0;
    double m_frameRate = 0;
    int64_t m_rotate = 0;
    int64_t m_frameNums = 0;
    QString isNotPausedButtonText;
    QString m_FfDllVersion;

public:
    void getFileName(std::string str);
    void loadFfmpegDll(QString str);
    void play();
    void begin();
    void isPausedAudio();
    void getMediaInfo();
    void forceFinishPlay();
    void pollAudioName();
    void changeAudioDevice();
    video *getVideoHandle();
    audio *getAudioHandle();
    control();

private:
    bool isVideoFile = false;
    bool isAudioFile = false;
    bool isNotPaused = true; //音频静音
    video *m_videoHandle = nullptr;
    audio *m_audioHandle = nullptr;
    manageAudioDevice *m_manageDEV = nullptr;
    QImage m_img;
    char filePath[1024];

private:
    void _conn();
    void _init_isNotPausedButton();
    void _fetchVersionFfDll();
};

#endif // CONTROL_H
