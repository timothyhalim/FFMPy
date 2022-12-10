#include "Decoder.h"

Decoder::Decoder()
{
    // av_register_all();
};

Decoder::Decoder(std::string filepath)
{
    this->filepath = filepath;
};

Decoder::~Decoder()
{
};

std::string Decoder::set_filepath(std::string filepath)
{
    this->filepath = filepath;
    return this->filepath;
};

std::string Decoder::get_filepath()
{
    return this->filepath;
};

int Decoder::open_file()
{
    if (this->filepath.empty()){
        return -1;
    };

    AVFormatContext *pFormatContext = avformat_alloc_context();
    if (!pFormatContext) {
        std::cerr << "ERROR could not allocate memory for Format Context\n";
        return -1;
    }

    if (avformat_open_input(&pFormatContext, this->filepath.c_str(), NULL, NULL) != 0) {
        std::cerr << "ERROR could not open the file\n";
        return -1;
    }

    std::cout << "Duration " << pFormatContext->duration << "\n";

    if (avformat_find_stream_info(pFormatContext,  NULL) < 0) {
        std::cerr << "ERROR could not get the stream info";
        return -1;
    }

    const AVCodec *pCodec = NULL;
    AVCodecParameters *pCodecParameters = NULL;
    int video_stream_index = -1;

    for (unsigned int i = 0; i < pFormatContext->nb_streams; i++)
    {
        AVCodecParameters *pLocalCodecParameters =  NULL;
        pLocalCodecParameters = pFormatContext->streams[i]->codecpar;

        const AVCodec *pLocalCodec = NULL;

        pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

        if (pLocalCodec==NULL) {
            std::cerr << "ERROR unsupported codec!\n";
            continue;
        }
        if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
            if (video_stream_index == -1) {
                video_stream_index = i;
                pCodec = pLocalCodec;
                pCodecParameters = pLocalCodecParameters;
            }
            std::cout << "Video Codec: resolution " << pLocalCodecParameters->width << " x " << pLocalCodecParameters->height << "\n";
        } else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
            std::cout << "Audio Codec: " << pLocalCodecParameters->channels << " channels, sample rate " << pLocalCodecParameters->sample_rate << "\n";
        }
        std::cout << "   Codec " << pLocalCodec->name << " ID " << pLocalCodec->id << " bit_rate " << pLocalCodecParameters->bit_rate << "\n";
    }

    if (video_stream_index == -1) {
        std::cerr << "File does not contain a video stream!\n";
        return -1;
    }
    AVCodecContext *pCodecContext = avcodec_alloc_context3(pCodec);
    if (!pCodecContext)
    {
        std::cerr << "Failed to allocated memory for AVCodecContext\n";
        return -1;
    }

    if (avcodec_parameters_to_context(pCodecContext, pCodecParameters) < 0)
    {
        std::cerr << "Failed to copy codec params to codec context\n";
        return -1;
    }

    if (avcodec_open2(pCodecContext, pCodec, NULL) < 0)
    {
        std::cerr << "Failed to open codec through avcodec_open2\n";
        return -1;
    }

    AVFrame *pFrame = av_frame_alloc();
    if (!pFrame)
    {
        std::cerr << "Failed to allocate memory for AVFrame\n";
        return -1;
    }

    AVPacket *pPacket = av_packet_alloc();
    if (!pPacket)
    {
        std::cerr << "Failed to allocate memory for AVPacket\n";
        return -1;
    }

    int response = 0;
    int how_many_packets_to_process = 1;

    while (av_read_frame(pFormatContext, pPacket) >= 0)
    {
        // if it's the video stream
        if (pPacket->stream_index == video_stream_index) {
            std::cout << "AVPacket->pts " << pPacket->pts << "\n";
            response = avcodec_send_packet(pCodecContext, pPacket);

            if (response < 0) {
              std::cerr << "Error while sending a packet to the decoder\n";
              break;
            }

            while (response >= 0)
            {
              // Return decoded output data (into a frame) from a decoder
              // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga11e6542c4e66d3028668788a1a74217c
              response = avcodec_receive_frame(pCodecContext, pFrame);
              if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
                break;
              } else if (response < 0) {
                std::cerr << "Error while receiving a frame from the decoder\n";
                break;
              }

              if (response >= 0) {
                std::cerr << "Decoded\n";
                break;
              }
            }
            // Decode Frame
//             if (response >= 0) {
//                 QImage myImage = getQImageFromFrame(pFrame);
// //                w.setImage(QPixmap::fromImage(myImage));
//             }
            if (--how_many_packets_to_process < 0) break;
        }
        av_packet_unref(pPacket);
    }

    avformat_close_input(&pFormatContext);
    av_packet_free(&pPacket);
    av_frame_free(&pFrame);
    avcodec_free_context(&pCodecContext);
    avformat_close_input(&pFormatContext);
    return 0;
}

