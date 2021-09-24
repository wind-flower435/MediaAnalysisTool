#ifndef AUDIO_H
#define AUDIO_H

#include "mixHeader.h"
#include "helper.h"

class audio : public QObject
{
    Q_OBJECT
signals:
    void noSupported();
    void noSuitAudioCodec();
    void fileError();
    void Custom_Signal(QString);

private:
    uint8_t *out_buffer_audio = nullptr;
    SwrContext *swrCtx = nullptr;       //frame->16bit 44100 PCM 统一音频采样格式与采样率
    enum AVSampleFormat in_sample_fmt;  //输入的采样格式
    enum AVSampleFormat out_sample_fmt; //输出的采样格式 16bit PCM

    int in_sample_rate;     //输入的采样率
    int out_sample_rate;    //输出的采样率
    uint64_t in_ch_layout;  //输入的声道布局
    uint64_t out_ch_layout; //输出的声道布局
    uint16_t out_nb_samples;
    int out_channel_nb;

    std::list<AVPacket> audioPackets;  //音频包列表
    std::condition_variable apacketCv; //音频包列表条件变量
    std::mutex apacketMutex;           //音频包列表互斥锁

private:
    void _init();            //初始化相关结构体
    void getPacketToList();  //音频包入列表
    void demuxingAudioFf3(); //从列表取出包解码
    void demuxingAudioFf4();
    void audioPlayer(); //播放音频
    void freeAudio();   //释放
    bool ProPktsEmptyFf4();
    bool ProPktsEmptyFf3();
    bool _getAudioDecoderFf3();
    bool _getAudioDecoderFf4();

public:
    static uint8_t *audio_chunk;
    static uint32_t audio_len;
    static uint8_t *audio_pos;
    static HMODULE sdl;
    static void fill_audio(void *udata, uint8_t *stream, int len);

public:
    ///音频相关
    SDL_AudioSpec wanted_spec;
    AVFormatContext *pFormatCtx = nullptr;
    AVCodecContext *aCodecCtx = nullptr;
    AVCodec *aCodec = nullptr;
    AVFrame *aFrame = nullptr;
    int audioIndex = -1;
    double m_realAudioDuration = 0;      //实际播放音频时长
    bool isNotPaused = true;             //音频静音
    bool isforceFinishFlag = false;      //强制结束标志
    bool isOnPlayFlag = false;           //正在播放标志
    bool isaudioDeviceCloseFlag = false; //音频设备关闭标志
    char filePath[1024];                 //文件路径
    QString m_VersionFfDll;
    helper *m_help = nullptr;

public:
    bool begin(); //打开媒体文件获取解码器等
    void play();  //播放音频与解码同时进行
    void getFileName(std::string str);
    void forceFinishAudio();
    void loadSDL(QString str);
    void m_helpInit();
    audio();
};

#endif // AUDIO_H
