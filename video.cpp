#include "video.h"
#include "ffmpegDll.h"

#include <chrono>
#include <QMetaType>
#include <QDebug>

video::video()
{
    qRegisterMetaType<QImage>("QImage&");
    m_help = new helper();
}

void video::_init()
{
    pFormatCtx = m_help->avformat_alloc_context();
    pCodecCtx = m_help->avcodec_alloc_context3(nullptr);
    pFrame = m_help->av_frame_alloc();
}

void video::getFileName(std::string str)
{
    strcpy(filePath, str.c_str());
}

double video::getRotateAngle()
{
    AVDictionaryEntry *tag = nullptr;
    double m_rotate = 0;
    while ((tag = m_help->av_dict_get(pFormatCtx->streams[videoIndex]->metadata, "rotate", tag, AV_DICT_IGNORE_SUFFIX)) != 0)
        m_rotate = atoi(tag->value) % 360;
    return m_rotate;
}

QString video::fetchVersionFfDll()
{
    QString m_version = m_help->av_version_info();
    m_version = m_version.left(m_version.indexOf("."));
    return m_version;
}

void video::m_helpInit()
{
    m_help->init();
}

bool video::begin()
{
    if (m_help->dllMap.empty())
        return false;
    _init();

    if (m_VersionFfDll == "3" || m_VersionFfDll == "n3") //Ff3
        m_help->av_register_all();

    if (m_help->avformat_open_input(&pFormatCtx, filePath, nullptr, nullptr) != 0)
    {
        emit noSupported();
        return false;
    }
    if (m_help->avformat_find_stream_info(pFormatCtx, nullptr) < 0)
        return false;

    //获取视频解码器
    bool flag = false;
    if (m_VersionFfDll == "3" || m_VersionFfDll == "n3")
        flag = _getVideoDecoderFf3();
    else
        flag = _getVideoDecoderFf4();
    if (!flag)
        return false;

    //打开视频解码器
    if (videoIndex >= 0 && m_help->avcodec_open2(pCodecCtx, pCodec, nullptr) < 0)
    {
        if (pCodecCtx->codec_id == AV_CODEC_ID_NONE)
            emit Custom_Signal("The video codec is Unknown, no supported.");
        else
            emit Custom_Signal("The video codec is " + QString(pCodecCtx->codec_descriptor->name) + ", no supported.");
        return false;
    }
    return true;
}

bool video::_getVideoDecoderFf4()
{
    for (int i = 0; i < (int)pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoIndex = i;
            break;
        }
    }

    if (videoIndex == -1)
        return false;

    AVCodecParameters *avCodecParameters = pFormatCtx->streams[videoIndex]->codecpar;
    int ret(-1);
    ret = m_help->avcodec_parameters_to_context(pCodecCtx, avCodecParameters);
    if (ret < 0)
    {
        emit noSuitVideoCodec();
        return false;
    }
    pCodec = const_cast<AVCodec *>(m_help->avcodec_find_decoder(pCodecCtx->codec_id));

    return true;
}

bool video::_getVideoDecoderFf3() //获取视频解码器
{
    for (int i = 0; i < (int)pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoIndex = i;
            break;
        }
    }
    if (videoIndex == -1)
        return false;

    AVCodecContext *avCodecParameters = pFormatCtx->streams[videoIndex]->codec;
    int ret(-1);
    ret = m_help->avcodec_copy_context(pCodecCtx, avCodecParameters);
    if (ret < 0)
    {
        emit noSuitVideoCodec();
        return false;
    }
    pCodec = const_cast<AVCodec *>(m_help->avcodec_find_decoder(pCodecCtx->codec_id));

    return true;
}

bool video::ProPktsEmptyFf3(AVFrame *pFrameRGB)
{
    std::unique_lock<std::mutex> lk(vpacketMutex);
    if (vpacketCv.wait_for(lk, std::chrono::milliseconds(100)) == std::cv_status::timeout)
    {
        lk.unlock();
        freeVideo();
        m_help->av_frame_free(&pFrameRGB);
        return true;
    }
    return false;
}

void video::demuxingVideoFf3()
{
    AVFrame *pFrameRGB = m_help->av_frame_alloc();
    unsigned char *out_buffer;

    out_buffer = (unsigned char *)m_help->av_malloc((size_t)m_help->av_image_get_buffer_size(AV_PIX_FMT_RGB32,
                                                                                             pCodecCtx->width,
                                                                                             pCodecCtx->height,
                                                                                             1));
    m_help->av_image_fill_arrays(pFrameRGB->data,
                                 pFrameRGB->linesize,
                                 out_buffer,
                                 AV_PIX_FMT_RGB32,
                                 pCodecCtx->width,
                                 pCodecCtx->height,
                                 1);
    img_convert_ctx = m_help->sws_getContext(pCodecCtx->width,
                                             pCodecCtx->height,
                                             pCodecCtx->pix_fmt,
                                             pCodecCtx->width,
                                             pCodecCtx->height,
                                             AV_PIX_FMT_RGB32,
                                             SWS_BICUBIC,
                                             nullptr,
                                             nullptr,
                                             nullptr);

    AVPacket packet;
    int ret(-1);
    int got_frame_ptr = 0;
    while (1)
    {
        if (videoPackets.empty())
        {
            if (ProPktsEmptyFf3(pFrameRGB))
                return;
        }
        vpacketMutex.lock();
        packet = videoPackets.front();
        videoPackets.pop_front();
        vpacketMutex.unlock();

        ret = m_help->avcodec_decode_video2(pCodecCtx, pFrame, &got_frame_ptr, &packet);

        if (ret < 0)
        {
            emit fileError();
            freeVideo();
            m_help->av_frame_free(&pFrameRGB);
            return;
        }
        if (got_frame_ptr)
        {
            imgPlayer(pFrameRGB);
        }
        m_help->av_free_packet(&packet);
    }
}

bool video::ProPktsEmptyFf4(AVFrame *pFrameRGB)
{
    int ret(-1);
    std::unique_lock<std::mutex> lk(vpacketMutex);
    if (vpacketCv.wait_for(lk, std::chrono::milliseconds(100)) == std::cv_status::timeout)
    {
        lk.unlock();
        ret = m_help->avcodec_send_packet(pCodecCtx, nullptr);
        while (ret != AVERROR_EOF)
        {
            ret = m_help->avcodec_receive_frame(pCodecCtx, pFrame);
            if (ret == AVERROR_EOF)
            {
                m_help->avcodec_flush_buffers(pCodecCtx);
                break;
            }
            imgPlayer(pFrameRGB);
        }
        freeVideo();
        m_help->av_frame_free(&pFrameRGB);
        return true;
    }
    return false;
}

void video::demuxingVideoFf4()
{
    int ret(-1);
    AVFrame *pFrameRGB = m_help->av_frame_alloc();
    unsigned char *out_buffer;

    out_buffer = (unsigned char *)m_help->av_malloc((size_t)m_help->av_image_get_buffer_size(AV_PIX_FMT_RGB32,
                                                                                             pCodecCtx->width,
                                                                                             pCodecCtx->height,
                                                                                             1));
    m_help->av_image_fill_arrays(pFrameRGB->data,
                                 pFrameRGB->linesize,
                                 out_buffer,
                                 AV_PIX_FMT_RGB32,
                                 pCodecCtx->width,
                                 pCodecCtx->height,
                                 1);
    img_convert_ctx = m_help->sws_getContext(pCodecCtx->width,
                                             pCodecCtx->height,
                                             pCodecCtx->pix_fmt,
                                             pCodecCtx->width,
                                             pCodecCtx->height,
                                             AV_PIX_FMT_RGB32,
                                             SWS_BICUBIC,
                                             nullptr,
                                             nullptr,
                                             nullptr);

    AVPacket packet;

    while (1)
    {
        if (videoPackets.empty())
        {
            if (ProPktsEmptyFf4(pFrameRGB))
                return;
        }
        vpacketMutex.lock();
        packet = videoPackets.front();
        videoPackets.pop_front();
        vpacketMutex.unlock();

        ret = m_help->avcodec_send_packet(pCodecCtx, &packet);
        if (ret == AVERROR(EAGAIN))
        {
            m_help->av_packet_unref(&packet);
            continue;
        }
        if (ret < 0)
        {
            emit fileError();
            freeVideo();
            m_help->av_frame_free(&pFrameRGB);
            return;
        }
        while (ret >= 0)
        {
            ret = m_help->avcodec_receive_frame(pCodecCtx, pFrame);
            if (ret == AVERROR(EAGAIN))
                break;
            if (ret < 0)
            {
                emit fileError();
                freeVideo();
                m_help->av_frame_free(&pFrameRGB);
                return;
            }
            imgPlayer(pFrameRGB);
        }
        m_help->av_packet_unref(&packet);
    }
}

void video::imgPlayer(AVFrame *pFrameRGB)
{
    m_help->sws_scale(img_convert_ctx,
                      (const unsigned char *const *)pFrame->data,
                      pFrame->linesize,
                      0,
                      pCodecCtx->height,
                      pFrameRGB->data,
                      pFrameRGB->linesize);
    QImage tempImg((uchar *)pFrameRGB->data[0], pCodecCtx->width, pCodecCtx->height, QImage::Format_RGB32);
    emit s(tempImg);
    videoASyncClock();
}

void video::videoASyncClock()
{
    m_realVideoDuration += pFrame->pkt_duration * av_q2d(pFormatCtx->streams[videoIndex]->time_base);
    int64_t dualTime = pFrame->pkt_duration * av_q2d(pFormatCtx->streams[videoIndex]->time_base) * 1000;
    dualTime *= 0.8;
    std::this_thread::sleep_for(std::chrono::milliseconds(dualTime));
    if (asyncStartTime == -1)
        asyncStartTime = m_help->av_gettime();

    int64_t realTime = m_help->av_gettime() - asyncStartTime;
    int64_t ptsTime = pFrame->pts * av_q2d(pFormatCtx->streams[videoIndex]->time_base) * 1000000;
    if (ptsTime > realTime)
        std::this_thread::sleep_for(std::chrono::microseconds(ptsTime - realTime));
}

void video::getPacketToList()
{
    AVPacket packet;
    while (m_help->av_read_frame(pFormatCtx, &packet) >= 0)
    {
        if (isforceFinishFlag)
        {
            vpacketMutex.lock();
            videoPackets.clear();
            vpacketMutex.unlock();
            isforceFinishFlag = false;
            break;
        }
        while (videoPackets.size() >= 50) //防止视频队列过大
            std::this_thread::sleep_for(std::chrono::microseconds(500));
        if (packet.stream_index == videoIndex)
        {
            vpacketMutex.lock();
            videoPackets.push_back(packet);
            vpacketMutex.unlock();
            vpacketCv.notify_one();
        }
    }
}

void video::play()
{
    isOnPlayFlag = true;
    std::thread(&video::getPacketToList, this).detach();
    if (m_VersionFfDll == "3" || m_VersionFfDll == "n3")
        std::thread(&video::demuxingVideoFf3, this).detach();
    else
        std::thread(&video::demuxingVideoFf4, this).detach();
}

void video::forceFinishVideo()
{
    if (isOnPlayFlag)
        isforceFinishFlag = true;
}

void video::freeVideo()
{
    asyncStartTime = -1;
    videoIndex = -1;
    m_realVideoDuration = 0;
    isforceFinishFlag = false;
    isOnPlayFlag = false;

    m_help->sws_freeContext(img_convert_ctx);
    m_help->av_frame_free(&pFrame);
    m_help->avcodec_close(pCodecCtx);
}
