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

    private:
        bool _debug;
        std::string filepath;
        
        // int widthDst, heightDst;            // Target width, height of ExtractFrame().
        AVFormatContext* pFormatContext;    // Format context of the video.
        AVCodecContext* pCodecContext;      // Codec context of the video.
        AVPacket* pPacket;
        AVFrame* pFrame;
        const AVCodec* pCodec;
        AVCodecParameters* pCodecParameters;
        
        // uint8_t* RGBbuffer;                 // The buffer of the RGB formatted images.
        // struct SwsContext* PswsCtx;         // The context of the scale transformator.
        
        int video_width, video_height;                  // Width, height of the video.
        std::string video_codec;             // Show the name of the current codec.
        int64_t video_bit_rate;                   // File bitrate.
        double video_frame_rate;
        double video_time_base;
        int64_t video_frame_count;
        double video_duration;                   // Show the time of the video play.
        int video_stream_index;

        std::string audio_codec;             // Show the name of the current codec.
        int audio_channels;                 // Number of audio channels.
        int audio_sample_rate;              // Audio sample rate.
        int64_t audio_bit_rate;                   // File bitrate.
        
        void _reset();
        int _open();
        int _close();
        void _get_file_info();
};
