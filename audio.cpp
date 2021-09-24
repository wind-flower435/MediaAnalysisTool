#include "audio.h"
#include "ffmpegDll.h"
#include "sdl2Dll.h"

uint8_t *audio::audio_chunk = nullptr;
uint8_t *audio::audio_pos = nullptr;
uint32_t audio::audio_len = 0;
HMODULE audio::sdl = 0;

audio::audio()
{
    m_help = new helper();
}

void audio::loadSDL(QString str)
{
    SetDllDirectory((LPCWSTR)str.toStdWString().c_str());

    for (int i = 0; i < m_help->rec.size(); i++)
    {
        if (m_help->rec[i].left(m_help->rec[i].indexOf(".")) == "SDL2")
        {
            sdl = LoadLibrary((LPCWSTR)m_help->rec[i].toStdWString().c_str());
            break;
        }
    }
}

void audio::fill_audio(void *udata, uint8_t *stream, int len)
{
    P_SDL_MixAudio c_SDL_MixAudio = (P_SDL_MixAudio)GetProcAddress(audio::sdl, "SDL_MixAudio");
    P_SDL_memset c_SDL_memset = (P_SDL_memset)GetProcAddress(audio::sdl, "SDL_memset");

    c_SDL_memset(stream, 0, len);

    if (audio_len == 0)
        return;

    len = (len > audio_len ? audio_len : len);
    c_SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
    audio_pos += len;
    audio_len -= len;
}

void audio::_init()
{
    pFormatCtx = m_help->avformat_alloc_context();
    aCodecCtx = m_help->avcodec_alloc_context3(nullptr);
    aFrame = m_help->av_frame_alloc();
    out_buffer_audio = (uint8_t *)m_help->av_malloc(2 * 44100);
    swrCtx = m_help->swr_alloc();
}

void audio::getFileName(std::string str)
{
    strcpy(filePath, str.c_str());
}

void audio::m_helpInit()
{
    m_help->init();
}

bool audio::begin()
{
    if (m_help->dllMap.empty())
        return false;
    _init();

    //sdl库加载
    P_SDL_Init c_SDL_Init = (P_SDL_Init)GetProcAddress(sdl, "SDL_Init");
    P_SDL_OpenAudio c_SDL_OpenAudio = (P_SDL_OpenAudio)GetProcAddress(sdl, "SDL_OpenAudio");

    if (m_VersionFfDll == "3" || m_VersionFfDll == "n3") //Ff3
        m_help->av_register_all();

    if (m_help->avformat_open_input( &pFormatCtx, filePath, nullptr, nullptr) != 0)
    {
        emit noSupported();
        return false;
    }
    if (m_help->avformat_find_stream_info(pFormatCtx, nullptr) < 0)
        return false;

    //获取音频解码器
    bool flag = false;
    if (m_VersionFfDll == "3" || m_VersionFfDll == "n3")
        flag = _getAudioDecoderFf3();
    else
        flag = _getAudioDecoderFf4();
    if (!flag)
        return false;

    //打开音频解码器
    if (audioIndex >= 0 && m_help->avcodec_open2(aCodecCtx, aCodec, nullptr) < 0)
    {
        if (aCodecCtx->codec_id == AV_CODEC_ID_NONE)
            emit Custom_Signal("The audio codec is Unknown, no supported.");
        else
            emit Custom_Signal("The audio codec is " + QString(aCodecCtx->codec_descriptor->name) + ", no supported.");
        return false;
    }

    //音频参数初始化
    in_sample_fmt = aCodecCtx->sample_fmt;
    out_sample_fmt = AV_SAMPLE_FMT_S16;
    in_sample_rate = aCodecCtx->sample_rate;
    out_sample_rate = 44100;
    in_ch_layout = aCodecCtx->channel_layout;
    out_ch_layout = AV_CH_LAYOUT_MONO;
    out_nb_samples = aCodecCtx->frame_size;
    out_channel_nb = m_help->av_get_channel_layout_nb_channels(out_ch_layout);
    if (c_SDL_Init(SDL_INIT_AUDIO))
        return false;
    wanted_spec.freq = out_sample_rate;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.channels = uint8_t(out_channel_nb);
    wanted_spec.silence = 0;
    wanted_spec.samples = out_nb_samples;
    wanted_spec.callback = audio::fill_audio;
    wanted_spec.userdata = aCodecCtx;

    if (c_SDL_OpenAudio(&wanted_spec, nullptr) < 0)
        return false;

    m_help->swr_alloc_set_opts(
             swrCtx,
             out_ch_layout,
             out_sample_fmt,
             out_sample_rate,
             in_ch_layout,
             in_sample_fmt,
             in_sample_rate,
             0,
             nullptr);
    m_help->swr_init(swrCtx);

    return true;
}

bool audio::_getAudioDecoderFf4()
{
    for (int i = 0; i < (int)pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audioIndex = i;
            break;
        }
    }
    if (audioIndex == -1)
        return false;

    AVCodecParameters *avCodecParameters1 = pFormatCtx->streams[audioIndex]->codecpar;
    int ret(-1);
    ret = m_help->avcodec_parameters_to_context(aCodecCtx, avCodecParameters1);
    if (ret < 0)
    {
        emit noSuitAudioCodec();
        return false;
    }
    aCodec = const_cast<AVCodec *>(m_help->avcodec_find_decoder(aCodecCtx->codec_id));

    return true;
}

bool audio::_getAudioDecoderFf3()
{

    for (int i = 0; i < (int)pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audioIndex = i;
            break;
        }
    }
    if (audioIndex == -1)
        return false;

    AVCodecContext *avCodecParameters1 = pFormatCtx->streams[audioIndex]->codec;
    int ret(-1);
    ret = m_help->avcodec_copy_context(aCodecCtx, avCodecParameters1);
    if (ret < 0)
    {
        emit noSuitAudioCodec();
        return false;
    }
    aCodec = const_cast<AVCodec *>(m_help->avcodec_find_decoder(aCodecCtx->codec_id));

    return true;
}

void audio::getPacketToList()
{
    AVPacket packet;
    while (m_help->av_read_frame(pFormatCtx, &packet) >= 0)
    {
        if (isforceFinishFlag)
        {
            apacketMutex.lock();
            audioPackets.clear();
            apacketMutex.unlock();
            isforceFinishFlag = false;
            break;
        }
        while (audioPackets.size() >= 50) //防止音频队列过大
            std::this_thread::sleep_for(std::chrono::microseconds(500));
        if (packet.stream_index == audioIndex)
        {
            apacketMutex.lock();
            audioPackets.push_back(packet);
            apacketMutex.unlock();
            apacketCv.notify_one();
        }
    }
}

void audio::demuxingAudioFf3()
{
    int got_frame_ptr = 0;
    AVPacket packet;
    int ret(-1);
    while (1)
    {
        if (audioPackets.empty())
        {
            if (ProPktsEmptyFf3())
                return;
        }
        apacketMutex.lock();
        packet = audioPackets.front();
        audioPackets.pop_front();
        apacketMutex.unlock();

        ret = m_help->avcodec_decode_audio4(aCodecCtx, aFrame, &got_frame_ptr, &packet);
        if (ret < 0)
        {
            emit fileError();
            freeAudio();
            return;
        }
        if (got_frame_ptr)
        {
            audioPlayer();
        }
        m_help->av_free_packet(&packet);
    }
}

bool audio::ProPktsEmptyFf3()
{
    int ret = -1;
    std::unique_lock<std::mutex> lk(apacketMutex);
    if (apacketCv.wait_for(lk, std::chrono::milliseconds(100)) == std::cv_status::timeout)
    {
        lk.unlock();
        freeAudio();
        return true;
    }
    return false;
}

bool audio::ProPktsEmptyFf4()
{
    int ret = -1;
    std::unique_lock<std::mutex> lk(apacketMutex);
    if (apacketCv.wait_for(lk, std::chrono::milliseconds(100)) == std::cv_status::timeout)
    {
        lk.unlock();
        ret = m_help->avcodec_send_packet(aCodecCtx, nullptr);
        while (ret != AVERROR_EOF)
        {
            ret = m_help->avcodec_receive_frame(aCodecCtx, aFrame);
            if (ret == AVERROR_EOF)
            {
                m_help->avcodec_flush_buffers(aCodecCtx);
                break;
            }
            audioPlayer();
        }
        freeAudio();
        return true;
    }
    return false;
}

void audio::demuxingAudioFf4()
{
    int ret(-1);

    AVPacket packet;
    while (1)
    {
        if (audioPackets.empty())
        {
            if (ProPktsEmptyFf4())
                return;
        }
        apacketMutex.lock();
        packet = audioPackets.front();
        audioPackets.pop_front();
        apacketMutex.unlock();

        ret = m_help->avcodec_send_packet(aCodecCtx, &packet);
        if (ret == AVERROR(EAGAIN))
        {
            m_help->av_packet_unref(&packet);
            continue;
        }
        if (ret < 0)
        {
            emit fileError();
            freeAudio();
            return;
        }
        while (ret >= 0)
        {
            ret = m_help->avcodec_receive_frame(aCodecCtx, aFrame);
            if (ret == AVERROR(EAGAIN))
                break;
            if (ret < 0)
            {
                emit fileError();
                freeAudio();
                return;
            }
            audioPlayer();
        }
        m_help->av_packet_unref(&packet);
    }
}

void audio::audioPlayer()
{

    //sdl库加载
    P_SDL_Delay c_SDL_Delay = (P_SDL_Delay)GetProcAddress(sdl, "SDL_Delay");
    P_SDL_PauseAudio c_SDL_PauseAudio = (P_SDL_PauseAudio)GetProcAddress(sdl, "SDL_PauseAudio");

    c_SDL_PauseAudio(0);
    m_realAudioDuration += aFrame->pkt_duration * av_q2d(pFormatCtx->streams[audioIndex]->time_base);
    m_help->swr_convert(swrCtx, &out_buffer_audio, 2 * 44100, (const uint8_t **)aFrame->data, aFrame->nb_samples);
    int out_buffer_audio_size = m_help->av_samples_get_buffer_size(NULL, out_channel_nb, aFrame->nb_samples, out_sample_fmt, 1);

    if (!isNotPaused)
        memset(out_buffer_audio, 0, out_buffer_audio_size);
    while (audio_len > 0 && !isaudioDeviceCloseFlag)
        c_SDL_Delay(1);

    if (isaudioDeviceCloseFlag)
    {
        int64_t dualTime = aFrame->pkt_duration * av_q2d(pFormatCtx->streams[audioIndex]->time_base) * 1000;
        std::this_thread::sleep_for(std::chrono::milliseconds(dualTime));
    }
    audio_chunk = (uint8_t *)out_buffer_audio;
    audio_len = out_buffer_audio_size;
    audio_pos = audio_chunk;
}

void audio::play()
{
    isOnPlayFlag = true;
    std::thread(&audio::getPacketToList, this).detach();
    if (m_VersionFfDll == "3" || m_VersionFfDll == "n3")
        std::thread(&audio::demuxingAudioFf3, this).detach();
    else
        std::thread(&audio::demuxingAudioFf4, this).detach();
}

void audio::forceFinishAudio()
{
    if (isOnPlayFlag)
        isforceFinishFlag = true;
}

void audio::freeAudio()
{
    audioIndex = -1;
    m_realAudioDuration = 0;
    isNotPaused = true;
    isforceFinishFlag = false;
    isOnPlayFlag = false;

    P_SDL_CloseAudio c_SDL_CloseAudio = (P_SDL_CloseAudio)GetProcAddress(sdl, "SDL_CloseAudio");
    P_SDL_Quit c_SDL_Quit = (P_SDL_Quit)GetProcAddress(sdl, "SDL_Quit");

    c_SDL_CloseAudio();
    c_SDL_Quit();
    m_help->av_frame_free(&aFrame);
    m_help->avcodec_close(aCodecCtx);
}
