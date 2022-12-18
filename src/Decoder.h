#pragma once
#include "FFMPy.h"

class Decoder
{
    public:
        Decoder();
        Decoder(std::string);
        ~Decoder();

        std::string set_filepath(std::string filepath);
        std::string get_filepath();
        PyObject* get_file_info();
        PyObject* extract_frame(int64_t frame);
        bool debug(bool state);
        
        int open_stream();
        int close_stream();

    private:
        bool _debug;
        bool _is_open;
        bool _is_streaming;
        std::string filepath;

        AVFormatContext* pFormatContext;
        AVStream* pVideoStream;
        int video_stream_index;
        AVCodecContext *pCodecContext;
        AVFrame *pFrame;
        AVPacket *pPacket;
        SwsContext *swsCtx;

        int video_width, video_height;                  // Width, height of the video.
        std::string video_codec;             // Show the name of the current codec.
        int64_t video_bit_rate;                   // File bitrate.
        double video_frame_rate;
        double video_time_base;
        int64_t video_frame_count;
        double video_duration;                   // Show the time of the video play.

        std::string audio_codec;             // Show the name of the current codec.
        int audio_channels;                 // Number of audio channels.
        int audio_sample_rate;              // Audio sample rate.
        int64_t audio_bit_rate;                   // File bitrate.
        
        void _reset();
        int open();
        int close();
        void _get_file_info();
};
