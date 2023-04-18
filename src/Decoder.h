#ifndef FFMPy_Decoder
#define FFMPy_Decoder

#include "FFMPy.h"
#include "Utils.h"

class Decoder {
    public:
        Decoder();
        Decoder(std::string);
        ~Decoder();

        std::string set_input_file(std::string filepath);
        std::string get_input_file();
        PyObject* get_input_info();
        PyObject* extract_frame(int64_t frame);
        PyObject* extract_image(AVFrame *frame);
        bool debug(bool state);
        
        int open_stream();
        int close_stream();

    private:
        bool _debug;
        bool _is_open;
        bool _is_streaming;
        std::string filepath;

        AVFormatContext* pFormatContext;
        AVFrame *pFrame;
        AVPacket *pPacket;
        AVCodecContext *pCodecContext;
        AVStream* pVideoStream;
        int video_stream_index;
        SwsContext *swsCtx;
        AVStream* pAudioStream;
        int audio_stream_index;
        SwrContext *swrCtx;

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
        int audio_bit_per_sample;              // Audio sample rate.

        void _reset();
        int open();
        int close();
        void _get_input_info();
};

#endif