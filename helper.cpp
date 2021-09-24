#include "helper.h"

void helper::loadFfmpegDll(QString str)
{
    SetDllDirectory((LPCWSTR)str.toStdWString().c_str());

    for (int i = 0; i < rec.size(); i++)
    {
        if (rec[i].left(rec[i].indexOf(".")) == "SDL2")
            continue;
        rec[i] = rec[i].left(rec[i].indexOf("."));
        if (dllMap.find(rec[i].left(rec[i].indexOf("-"))) == dllMap.end())
            dllMap[rec[i].left(rec[i].indexOf("-"))] = LoadLibrary((LPCWSTR)rec[i].toStdWString().c_str());
    }
}

void helper::init()
{
#define TRAN(s, dll) c_##s = (P_##s)GetProcAddress(dllMap.at(#dll), #s)

    TRAN(avformat_alloc_context, avformat);
    TRAN(avcodec_alloc_context3, avcodec);
    TRAN(av_malloc, avutil);
    TRAN(av_frame_alloc, avutil);
    TRAN(swr_alloc, swresample);
    TRAN(avformat_open_input, avformat);
    TRAN(avformat_find_stream_info, avformat);
    TRAN(avcodec_parameters_to_context, avcodec);
    TRAN(avcodec_find_decoder, avcodec);
    TRAN(avcodec_open2, avcodec);
    TRAN(av_frame_free, avutil);
    TRAN(avcodec_close, avcodec);
    TRAN(av_read_frame, avformat);
    TRAN(avcodec_send_packet, avcodec);
    TRAN(avcodec_receive_frame, avcodec);
    TRAN(av_packet_unref, avcodec);
    TRAN(avcodec_flush_buffers, avcodec);
    TRAN(avcodec_get_name, avcodec);
    TRAN(avcodec_profile_name, avcodec);

    //ffmpeg 3.0.x api
    TRAN(av_register_all, avformat);
    TRAN(avcodec_copy_context, avcodec);
    TRAN(avcodec_decode_audio4, avcodec);
    TRAN(av_free_packet, avcodec);
    TRAN(avcodec_decode_video2, avcodec);

    //audio api
    TRAN(swr_convert, swresample);
    TRAN(av_samples_get_buffer_size, avutil);
    TRAN(av_get_channel_layout_nb_channels, avutil);
    TRAN(swr_alloc_set_opts, swresample);
    TRAN(swr_init, swresample);

    //video api
    TRAN(av_dict_get, avutil);
    TRAN(av_version_info, avutil);
    TRAN(sws_getContext, swscale);
    TRAN(av_image_get_buffer_size, avutil);
    TRAN(av_image_fill_arrays, avutil);
    TRAN(sws_scale, swscale);
    TRAN(av_gettime, avutil);
    TRAN(sws_freeContext, swscale);

#undef TRAN

}

void helper::initTransferCon()
{
#define conInsert(x, y) transferCon.insert({#x, y})

    conInsert(avformat_alloc_context, c_avformat_alloc_context);
    conInsert(avcodec_alloc_context3, c_avcodec_alloc_context3);
    conInsert(av_frame_alloc, c_av_frame_alloc);
    conInsert(av_malloc, c_av_malloc);
    conInsert(swr_alloc, c_swr_alloc);
    conInsert(avformat_open_input, c_avformat_open_input);
    conInsert(avformat_find_stream_info, c_avformat_find_stream_info);
    conInsert(avcodec_parameters_to_context, c_avcodec_parameters_to_context);
    conInsert(avcodec_find_decoder, c_avcodec_find_decoder);
    conInsert(avcodec_open2, c_avcodec_open2);
    conInsert(av_frame_free, c_av_frame_free);
    conInsert(avcodec_close, c_avcodec_close);
    conInsert(av_read_frame, c_av_read_frame);
    conInsert(avcodec_send_packet, c_avcodec_send_packet);
    conInsert(avcodec_receive_frame, c_avcodec_receive_frame);
    conInsert(av_packet_unref, c_av_packet_unref);
    conInsert(avcodec_flush_buffers, c_avcodec_flush_buffers);
    conInsert(avcodec_get_name, c_avcodec_get_name);
    conInsert(avcodec_profile_name, c_avcodec_profile_name);

    conInsert(av_register_all, c_av_register_all);
    conInsert(avcodec_copy_context, c_avcodec_copy_context);
    conInsert(avcodec_decode_audio4, c_avcodec_decode_audio4);
    conInsert(av_free_packet, c_av_free_packet);
    conInsert(avcodec_decode_video2, c_avcodec_decode_video2);

    conInsert(swr_convert, c_swr_convert);
    conInsert(av_samples_get_buffer_size, c_av_samples_get_buffer_size);
    conInsert(av_get_channel_layout_nb_channels, c_av_get_channel_layout_nb_channels);
    conInsert(swr_alloc_set_opts, c_swr_alloc_set_opts);
    conInsert(swr_init, c_swr_init);

    conInsert(av_dict_get, c_av_dict_get);
    conInsert(av_version_info, c_av_version_info);
    conInsert(sws_getContext, c_sws_getContext);
    conInsert(av_image_get_buffer_size, c_av_image_get_buffer_size);
    conInsert(av_image_fill_arrays, c_av_image_fill_arrays);
    conInsert(sws_scale, c_sws_scale);
    conInsert(av_gettime, c_av_gettime);
    conInsert(sws_freeContext, c_sws_freeContext);

#undef conInsert
}
