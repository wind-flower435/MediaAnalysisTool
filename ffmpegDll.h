#ifndef FFMPEGDLL_H
#define FFMPEGDLL_H

#if AV_GCC_VERSION_AT_LEAST(3,1)
    #define av_malloc_attrib __attribute__((__malloc__))
#else
    #define av_malloc_attrib
#endif

#if AV_GCC_VERSION_AT_LEAST(4,3)
    #define av_alloc_size(...) __attribute__((alloc_size(__VA_ARGS__)))
#else
    #define av_alloc_size(...)
#endif

typedef int (*P_av_read_frame)(AVFormatContext *s, AVPacket *pkt);

typedef int (*P_avformat_open_input)(AVFormatContext **ps, const char *url,
                                const AVInputFormat *fmt, AVDictionary **options);

typedef int (*P_avformat_find_stream_info)(AVFormatContext *ic, AVDictionary **options);

typedef int (*P_avcodec_parameters_to_context)(AVCodecContext *codec,
                                  const AVCodecParameters *par);

typedef const AVCodec *(*P_avcodec_find_decoder)(enum AVCodecID id);

typedef int (*P_avcodec_open2)(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options);

typedef AVFormatContext *(*P_avformat_alloc_context)(void);

typedef AVCodecContext *(*P_avcodec_alloc_context3)(const AVCodec *codec);

typedef AVFrame *(*P_av_frame_alloc)(void);

typedef struct SwsContext *(*P_sws_getContext)(int srcW, int srcH, enum AVPixelFormat srcFormat,
                                          int dstW, int dstH, enum AVPixelFormat dstFormat,
                                          int flags, SwsFilter *srcFilter,
                                          SwsFilter *dstFilter, const double *param);

typedef int (*P_avcodec_send_packet)(AVCodecContext *avctx, const AVPacket *avpkt);

typedef int (*P_avcodec_receive_frame)(AVCodecContext *avctx, AVFrame *frame);

typedef int (*P_sws_scale)(struct SwsContext *c, const uint8_t *const srcSlice[],
                      const int srcStride[], int srcSliceY, int srcSliceH,
                      uint8_t *const dst[], const int dstStride[]);

typedef void (*P_av_packet_unref)(AVPacket *pkt);

typedef void *(*P_av_malloc)(size_t size) av_malloc_attrib av_alloc_size(1);

typedef int (*P_av_image_get_buffer_size)(enum AVPixelFormat pix_fmt, int width, int height, int align);

typedef int (*P_av_image_fill_arrays)(uint8_t *dst_data[4], int dst_linesize[4],
const uint8_t *src,
enum AVPixelFormat pix_fmt, int width, int height, int align);

typedef struct SwrContext *(*P_swr_alloc)(void);
typedef int (*P_swr_init)(struct SwrContext *s);
typedef struct SwrContext *(*P_swr_alloc_set_opts)(struct SwrContext *s,
                                              int64_t out_ch_layout, enum AVSampleFormat out_sample_fmt, int out_sample_rate,
                                              int64_t  in_ch_layout, enum AVSampleFormat  in_sample_fmt, int  in_sample_rate,
                                              int log_offset, void *log_ctx);

typedef int (*P_av_get_channel_layout_nb_channels)(uint64_t channel_layout);
typedef int (*P_swr_convert)(struct SwrContext *s, uint8_t **out, int out_count,
                        const uint8_t **in , int in_count);
typedef int (*P_av_samples_get_buffer_size)(int *linesize, int nb_channels, int nb_samples,
                                       enum AVSampleFormat sample_fmt, int align);
typedef int64_t (*P_av_gettime)(void);
typedef void (*P_avformat_close_input)(AVFormatContext **s);

typedef AVDictionaryEntry *(*P_av_dict_get)(const AVDictionary *m, const char *key, const AVDictionaryEntry *prev, int flags);

typedef void (*P_avcodec_flush_buffers)(AVCodecContext *avctx);

typedef int (*P_av_dict_set_int)(AVDictionary **pm, const char *key, int64_t value, int flags);
typedef int (*P_av_dict_set)(AVDictionary **pm, const char *key, const char *value, int flags);
typedef const char * (*P_avcodec_get_name)(enum AVCodecID id);
typedef const char *(*P_avcodec_profile_name)(enum AVCodecID codec_id, int profile);

//destruction
typedef void (*P_sws_freeContext)(struct SwsContext *swsContext);
typedef int (*P_avcodec_close)(AVCodecContext *avctx);
typedef void (*P_av_frame_free)(AVFrame **frame);

//version
typedef unsigned (*P_avutil_version)(void);
typedef const char *(*P_av_version_info)(void);

/*ffmpeg3.0.x*/
typedef void (*P_av_register_all)(void);
typedef int (*P_avcodec_copy_context)(AVCodecContext *dest, const AVCodecContext *src);
typedef int (*P_avcodec_decode_audio4)(AVCodecContext *avctx, AVFrame *frame,
                          int *got_frame_ptr, const AVPacket *avpkt);
typedef int (*P_avcodec_decode_video2)(AVCodecContext *avctx, AVFrame *picture,
                         int *got_picture_ptr,
                         const AVPacket *avpkt);
typedef void (*P_av_free_packet)(AVPacket *pkt);

#endif // FFMPEGDLL_H
