#ifndef HELPER_H
#define HELPER_H

#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/time.h>
#include <QStringList>
#include <map>
#include <QString>
#include <Windows.h>

#include "ffmpegDll.h"

class helper
{
public:
     AVFormatContext *avformat_alloc_context(void)
     {
         return c_avformat_alloc_context();
     }

     AVCodecContext *avcodec_alloc_context3(const AVCodec * codec)
     {
         return c_avcodec_alloc_context3(codec);
     }

     AVFrame *av_frame_alloc(void)
     {
         return c_av_frame_alloc();
     }

     void *av_malloc(size_t size) av_malloc_attrib av_alloc_size(1)
     {
         return c_av_malloc(size);
     }

     struct SwrContext *swr_alloc(void)
     {
         return c_swr_alloc();
     }

     int avformat_open_input(AVFormatContext **ps, const char *url, AVInputFormat *fmt, AVDictionary** options)
     {
        return c_avformat_open_input(ps, url, fmt, options);
     }

     int avformat_find_stream_info(AVFormatContext *ic, AVDictionary **options)
     {
         return c_avformat_find_stream_info(ic, options);
     }

     int avcodec_parameters_to_context(AVCodecContext * codec, const AVCodecParameters * par)
     {
         return c_avcodec_parameters_to_context(codec, par);
     }

     const AVCodec *avcodec_find_decoder(enum AVCodecID id)
     {
         return c_avcodec_find_decoder(id);
     }

     int avcodec_open2(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options)
     {
         return c_avcodec_open2(avctx, codec, options);
     }

     int av_read_frame(AVFormatContext *s, AVPacket *pkt)
     {
         return c_av_read_frame(s, pkt);
     }

     int avcodec_send_packet(AVCodecContext *avctx, const AVPacket *avpkt)
     {
         return c_avcodec_send_packet(avctx, avpkt);
     }

     int avcodec_receive_frame(AVCodecContext *avctx, AVFrame *frame)
     {
         return c_avcodec_receive_frame(avctx, frame);
     }

     void av_packet_unref(AVPacket *pkt)
     {
         return c_av_packet_unref(pkt);
     }

     void avcodec_flush_buffers(AVCodecContext *&avctx)
     {
         return c_avcodec_flush_buffers(avctx);
     }

     void av_frame_free(AVFrame **frame)
     {
         return c_av_frame_free(frame);
     }

     int avcodec_close(AVCodecContext *avctx)
     {
         return c_avcodec_close(avctx);
     }

     const char *avcodec_get_name(enum AVCodecID id)
     {
         return c_avcodec_get_name(id);
     }

     const char *avcodec_profile_name(enum AVCodecID codec_id, int& profile)
     {
         return c_avcodec_profile_name(codec_id, profile);
     }

     int swr_convert(struct SwrContext *s, uint8_t **out, int out_count, const uint8_t **in , int& in_count)
     {
         return c_swr_convert(s, out, out_count, in, in_count);
     }

     int av_samples_get_buffer_size(int *linesize, int nb_channels, int nb_samples, enum AVSampleFormat sample_fmt, int align)
     {
         return c_av_samples_get_buffer_size(linesize, nb_channels, nb_samples, sample_fmt, align);
     }

     int av_get_channel_layout_nb_channels(uint64_t channel_layout)
     {
         return c_av_get_channel_layout_nb_channels(channel_layout);
     }

     struct SwrContext *swr_alloc_set_opts(struct SwrContext * s, int64_t out_ch_layout, enum AVSampleFormat out_sample_fmt, int out_sample_rate,
                                           int64_t  in_ch_layout, enum AVSampleFormat  in_sample_fmt, int  in_sample_rate,
                                           int log_offset, void * log_ctx)
     {
         return c_swr_alloc_set_opts(s,
                                   out_ch_layout,
                                   out_sample_fmt,
                                   out_sample_rate,
                                   in_ch_layout,
                                   in_sample_fmt,
                                   in_sample_rate,
                                   log_offset,
                                   log_ctx);
     }

     int swr_init(struct SwrContext * s)
     {
         return c_swr_init(s);
     }

     AVDictionaryEntry *av_dict_get(const AVDictionary *m, const char *key,const AVDictionaryEntry *prev, int flags)
     {
         return c_av_dict_get(m, key, prev, flags);
     }

     const char *av_version_info(void)
     {
         return c_av_version_info();
     }

     struct SwsContext *sws_getContext(int srcW, int srcH, enum AVPixelFormat srcFormat, int dstW, int dstH, enum AVPixelFormat dstFormat,
                                       int flags, SwsFilter * srcFilter,SwsFilter * dstFilter, const double * param)
     {
        return c_sws_getContext(srcW, srcH, srcFormat, dstW, dstH, dstFormat, flags, srcFilter, dstFilter, param);
     }

     int av_image_get_buffer_size(enum AVPixelFormat pix_fmt, int width, int height, int align)
     {
         return c_av_image_get_buffer_size(pix_fmt, width, height, align);
     }

     int av_image_fill_arrays(uint8_t *dst_data[4], int dst_linesize[4], const uint8_t * src,
                              enum AVPixelFormat pix_fmt, int width, int height, int align)
     {
         return c_av_image_fill_arrays(dst_data, dst_linesize, src, pix_fmt, width, height, align);
     }

     int sws_scale(struct SwsContext * c, const uint8_t *const srcSlice[], const int srcStride[], int srcSliceY, int srcSliceH,
                   uint8_t *const dst[], const int dstStride[])
     {
          return c_sws_scale(c, srcSlice, srcStride, srcSliceY, srcSliceH, dst, dstStride);
     }

     int64_t av_gettime(void)
     {
         return c_av_gettime();
     }

     void sws_freeContext(struct SwsContext * swsContext)
     {
         return c_sws_freeContext(swsContext);
     }

     //3.0
     void av_register_all(void)
     {
         return c_av_register_all();
     }

     int avcodec_copy_context(AVCodecContext * dest, const AVCodecContext * src)
     {
         return c_avcodec_copy_context(dest, src);
     }

     int avcodec_decode_audio4(AVCodecContext * avctx, AVFrame * frame, int * got_frame_ptr, const AVPacket * avpkt)
     {
         return c_avcodec_decode_audio4(avctx, frame, got_frame_ptr, avpkt);
     }

     void av_free_packet(AVPacket *pkt)
     {
         return c_av_free_packet(pkt);
     }

     int avcodec_decode_video2(AVCodecContext * avctx, AVFrame * picture, int * got_picture_ptr, const AVPacket * avpkt)
     {
         return c_avcodec_decode_video2(avctx, picture, got_picture_ptr, avpkt);
     }

public:
    P_avformat_alloc_context c_avformat_alloc_context = nullptr;
    P_avcodec_alloc_context3 c_avcodec_alloc_context3 = nullptr;
    P_av_frame_alloc c_av_frame_alloc = nullptr;
    P_av_malloc c_av_malloc = nullptr;
    P_swr_alloc c_swr_alloc = nullptr;

    P_avformat_open_input c_avformat_open_input = nullptr;
    P_avformat_find_stream_info c_avformat_find_stream_info = nullptr;
    P_avcodec_parameters_to_context c_avcodec_parameters_to_context = nullptr;
    P_avcodec_find_decoder c_avcodec_find_decoder = nullptr;
    P_avcodec_open2 c_avcodec_open2 = nullptr;
    P_av_read_frame c_av_read_frame = nullptr;
    P_avcodec_send_packet c_avcodec_send_packet = nullptr;
    P_avcodec_receive_frame c_avcodec_receive_frame = nullptr;
    P_av_packet_unref c_av_packet_unref = nullptr;
    P_avcodec_flush_buffers c_avcodec_flush_buffers = nullptr;
    P_av_frame_free c_av_frame_free = nullptr;
    P_avcodec_close c_avcodec_close = nullptr;
    P_avcodec_get_name c_avcodec_get_name = nullptr;
    P_avcodec_profile_name c_avcodec_profile_name = nullptr;

    //audio api
    P_swr_convert c_swr_convert = nullptr;
    P_av_samples_get_buffer_size c_av_samples_get_buffer_size = nullptr;
    P_av_get_channel_layout_nb_channels c_av_get_channel_layout_nb_channels = nullptr;
    P_swr_alloc_set_opts c_swr_alloc_set_opts = nullptr;
    P_swr_init c_swr_init = nullptr;

    //video api
    P_av_dict_get c_av_dict_get = nullptr;
    P_av_version_info c_av_version_info = nullptr;
    P_sws_getContext c_sws_getContext = nullptr;
    P_av_image_get_buffer_size c_av_image_get_buffer_size = nullptr;
    P_av_image_fill_arrays c_av_image_fill_arrays = nullptr;
    P_sws_scale c_sws_scale = nullptr;
    P_av_gettime c_av_gettime = nullptr;
    P_sws_freeContext c_sws_freeContext = nullptr;

    //ffmpeg 3.0.x api
    P_av_register_all c_av_register_all = nullptr;
    P_avcodec_copy_context c_avcodec_copy_context = nullptr;
    P_avcodec_decode_audio4 c_avcodec_decode_audio4 = nullptr;
    P_av_free_packet c_av_free_packet = nullptr;
    P_avcodec_decode_video2 c_avcodec_decode_video2 = nullptr;

public:
    QStringList rec;
    std::map<QString, HMODULE> dllMap;
    std::map<QString, void *> transferCon;

public:
    void loadFfmpegDll(QString str); 
    void init();
    void initTransferCon();
};

#endif // HELPER_H
