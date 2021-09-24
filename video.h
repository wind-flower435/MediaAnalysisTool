#ifndef VIDEO_H
#define VIDEO_H

#include "mixHeader.h"
#include "helper.h"

class video : public QObject
{
    Q_OBJECT
signals:
    void s(QImage &); //视频帧图像信号
    void noSupported();
    void noSuitVideoCodec();
    void fileError();
    void Custom_Signal(QString);

private:
    std::list<AVPacket> videoPackets;  //视频包列表
    std::condition_variable vpacketCv; //视频包列表条件变量
    std::mutex vpacketMutex;           //视频包列表互斥锁

    char filePath[1024];
    int64_t asyncStartTime = -1;

private:
    void demuxingVideoFf3();
    void demuxingVideoFf4();
    bool _getVideoDecoderFf3();
    bool _getVideoDecoderFf4();
    void imgPlayer(AVFrame *pFrameRGB); //显示一帧图片
    void _init();                       //初始化
    void getPacketToList();
    void freeVideo();
    void videoASyncClock();
    bool ProPktsEmptyFf4(AVFrame *);
    bool ProPktsEmptyFf3(AVFrame *);

public:
    AVFormatContext *pFormatCtx = nullptr;
    AVCodecContext *pCodecCtx = nullptr;
    AVCodec *pCodec = nullptr;
    AVFrame *pFrame = nullptr;
    struct SwsContext *img_convert_ctx = nullptr;

    int videoIndex = -1;
    double m_realVideoDuration = 0;
    bool isforceFinishFlag = false;
    bool isOnPlayFlag = false;
    QString m_VersionFfDll;
    helper *m_help = nullptr;

public:
    bool begin();
    void play();
    void getFileName(std::string str);
    double getRotateAngle();
    void forceFinishVideo();
    void m_helpInit();
    QString fetchVersionFfDll();
    video();
};

#endif // VIDEO_H
