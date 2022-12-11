#include "Decoder.h"

Decoder::Decoder() {
    this->debug(false);
    this->reset();
};

Decoder::Decoder(std::string filepath) {
    this->debug(false);
    this->filepath = filepath;
    int ret = this->open_file();
    if (ret < 0) {
        this->reset();
    }
};

Decoder::~Decoder() {
};

void Decoder::reset() {
    this->filepath.clear();
    this->video_width = 0;
    this->video_height = 0;
    this->video_codec.clear();
    this->video_bit_rate = 0;
    this->video_frame_rate = 0;
    this->video_time_base = 0;
    this->video_frame_count = 0;
    this->video_duration = 0;
    this->audio_codec.clear();
    this->audio_channels = 0;
    this->audio_sample_rate = 0;
    this->audio_bit_rate = 0;
}

std::string Decoder::set_filepath(std::string filepath) {
    this->filepath = filepath;
    int ret = this->open_file();
    if (ret < 0) {
        this->filepath.clear();
    }
    return this->filepath;
};

std::string Decoder::get_filepath() {
    return this->filepath;
};

PyObject* Decoder::get_file_info() {
    auto res = PyDict_New();
    std::string key;
    PyObject* val = nullptr;

    // Fill the values.
    key.assign("filepath");
    val = Py_BuildValue("s", this->filepath.c_str());
    PyDict_SetItemString(res, key.c_str(), val);
    Py_DECREF(val);

    key.assign("video_codec");
    val = Py_BuildValue("s", this->video_codec.c_str());
    PyDict_SetItemString(res, key.c_str(), val);
    Py_DECREF(val);
    key.assign("video_width");
    val = Py_BuildValue("i", this->video_width);
    PyDict_SetItemString(res, key.c_str(), val);
    Py_DECREF(val);
    key.assign("video_height");
    val = Py_BuildValue("i", this->video_height);
    PyDict_SetItemString(res, key.c_str(), val);
    Py_DECREF(val);
    key.assign("video_bit_rate");
    val = Py_BuildValue("L", this->video_bit_rate);
    PyDict_SetItemString(res, key.c_str(), val);
    Py_DECREF(val);
    key.assign("video_duration");
    val = Py_BuildValue("d", this->video_duration);
    PyDict_SetItemString(res, key.c_str(), val);
    Py_DECREF(val);
    key.assign("video_frame_rate");
    val = Py_BuildValue("d", this->video_frame_rate);
    PyDict_SetItemString(res, key.c_str(), val);
    Py_DECREF(val);
    key.assign("video_frame_count");
    val = Py_BuildValue("L", this->video_frame_count);
    PyDict_SetItemString(res, key.c_str(), val);
    Py_DECREF(val);

    key.assign("audio_codec");
    val = Py_BuildValue("s", this->audio_codec.c_str());
    PyDict_SetItemString(res, key.c_str(), val);
    Py_DECREF(val);
    key.assign("audio_channels");
    val = Py_BuildValue("i", this->audio_channels);
    PyDict_SetItemString(res, key.c_str(), val);
    Py_DECREF(val);
    key.assign("audio_sample_rate");
    val = Py_BuildValue("i", this->audio_sample_rate);
    PyDict_SetItemString(res, key.c_str(), val);
    Py_DECREF(val);
    key.assign("audio_bit_rate");
    val = Py_BuildValue("L", this->audio_bit_rate);
    PyDict_SetItemString(res, key.c_str(), val);
    Py_DECREF(val);


    // key.assign("frameRate");
    // val = Py_BuildValue("(ii)", frameRate.num, frameRate.den);
    // PyDict_SetItemString(res, key.c_str(), val);
    // Py_DECREF(val);
    // if (PStreamContex.enc) {
    //     key.assign("nthread");
    //     val = Py_BuildValue("i", PStreamContex.enc->thread_count);
    //     PyDict_SetItemString(res, key.c_str(), val);
    //     Py_DECREF(val);
    // }
    // else {
    //     key.assign("nthread");
    //     val = Py_BuildValue("i", nthread);
    //     PyDict_SetItemString(res, key.c_str(), val);
    //     Py_DECREF(val);
    // }
    return res;
}

bool Decoder::debug(bool state) {
    this->_debug = state;
    return this->_debug;
}

int Decoder::open_file() {
    if (this->filepath.empty()){
        std::cerr << "[ERROR] Filepath is not set yet!\n";
        return -1;
    };

    this->pFormatContext = avformat_alloc_context();
    if (!this->pFormatContext) {
        std::cerr << "[ERROR] Could not allocate memory for Format Context\n";
        return -1;
    }

    if (avformat_open_input(&this->pFormatContext, this->filepath.c_str(), NULL, NULL) != 0) {
        std::cerr << "[ERROR] Could not open the file\n";
        return -1;
    }

    if (this->_debug) {
        std::cout << "Duration " << this->pFormatContext->duration << "\n";
    }

    if (avformat_find_stream_info(this->pFormatContext,  NULL) < 0) {
        std::cerr << "[ERROR] Could not get the stream info";
        return -1;
    }

    const AVCodec *pCodec = NULL;
    AVCodecParameters *pCodecParameters = NULL;
    int video_stream_index = -1;

    for (unsigned int i = 0; i < this->pFormatContext->nb_streams; i++)
    {
        AVCodecParameters *pLocalCodecParameters =  NULL;
        pLocalCodecParameters = this->pFormatContext->streams[i]->codecpar;

        const AVCodec *pLocalCodec = NULL;

        pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);
        if (pLocalCodec==NULL) {
            std::cerr << "[ERROR] Unsupported codec!\n";
            continue;
        }
        if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
            if (video_stream_index == -1) {
                video_stream_index = i;
                pCodec = pLocalCodec;
                pCodecParameters = pLocalCodecParameters;
            }
            this->video_width = pLocalCodecParameters->width;
            this->video_height = pLocalCodecParameters->height;
            this->video_codec = pLocalCodec->name;
            this->video_bit_rate = pLocalCodecParameters->bit_rate;
            this->video_frame_rate = pFormatContext->streams[video_stream_index]->r_frame_rate.num 
                                   / pFormatContext->streams[video_stream_index]->r_frame_rate.den;

            this->video_time_base = pFormatContext->streams[video_stream_index]->time_base.num 
                                  * pFormatContext->streams[video_stream_index]->time_base.den;
            this->video_duration = pFormatContext->streams[video_stream_index]->duration / this->video_time_base;
            this->video_frame_count = av_rescale(
                                    static_cast<int64_t>(this->video_duration * 0xFFFF), 
                                    pFormatContext->streams[video_stream_index]->r_frame_rate.num, 
                                    pFormatContext->streams[video_stream_index]->r_frame_rate.den
                                ) / 0xFFFF;
            
            if (this->_debug) {
                std::cout << "Video Codec: resolution " << this->video_width 
                          << " x " << this->video_height << "\n";
                std::cout << "Video duration: " << this->video_duration << "\n";
            }
        
        } else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
            this->audio_channels = pLocalCodecParameters->ch_layout.nb_channels;
            this->audio_sample_rate = pLocalCodecParameters->sample_rate;
            this->audio_codec = pLocalCodec->name;
            this->audio_bit_rate = pLocalCodecParameters->bit_rate;
            
            if (this->_debug) {
                std::cout << "Audio Codec: " << this->audio_channels << " channels,"
                          <<" sample rate " << this->audio_sample_rate << "\n";
            }
        }
        if (this->_debug) {
            std::cout << "      Codec " << pLocalCodec->name 
                      << " ID " << pLocalCodec->id 
                      << " bit_rate " << pLocalCodecParameters->bit_rate << "\n";
        }
    }

    if (video_stream_index == -1) {
        std::cerr << "[ERROR] File does not contain a video stream!\n";
        return -1;
    }
    this->pCodecContext = avcodec_alloc_context3(pCodec);
    if (!this->pCodecContext)
    {
        std::cerr << "[ERROR] Failed to allocated memory for AVCodecContext\n";
        return -1;
    }
    this->pCodecContext->thread_count = 4; // Set Thread

    if (avcodec_parameters_to_context(this->pCodecContext, pCodecParameters) < 0)
    {
        std::cerr << "[ERROR] Failed to copy codec params to codec context\n";
        return -1;
    }

    if (avcodec_open2(this->pCodecContext, pCodec, NULL) < 0)
    {
        std::cerr << "[ERROR] Failed to open codec through avcodec_open2\n";
        return -1;
    }

    AVFrame *pFrame = av_frame_alloc();
    if (!pFrame)
    {
        std::cerr << "[ERROR] Failed to allocate memory for AVFrame\n";
        return -1;
    }

    AVPacket *pPacket = av_packet_alloc();
    if (!pPacket)
    {
        std::cerr << "[ERROR] Failed to allocate memory for AVPacket\n";
        return -1;
    }

    avformat_close_input(&this->pFormatContext);
    if (pPacket) {
        av_packet_free(&pPacket);
    }
    if (pFrame) {
        av_frame_free(&pFrame);
    }
    if (this->pCodecContext) {
        avcodec_free_context(&this->pCodecContext);
    }
    return 0;
}

// PyObject* extract_frame(int64_t frame) {
//     int response = 0;
//     int how_many_packets_to_process = 1;

//     int64_t processed_frame = 0;
//     while (av_read_frame(this->pFormatContext, pPacket) >= 0) {
//         // if it's the video stream
//         if (pPacket->stream_index == video_stream_index) {
//             // std::cout << "AVPacket->pts " << pPacket->pts << "\n";
//             response = avcodec_send_packet(this->pCodecContext, pPacket);

//             if (response < 0) {
//               std::cerr << "[ERROR] Failed to send a packet to the decoder\n";
//               break;
//             }

//             while (response >= 0)
//             {
//               // Return decoded output data (into a frame) from a decoder
//               // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga11e6542c4e66d3028668788a1a74217c
//               response = avcodec_receive_frame(this->pCodecContext, pFrame);
//               if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
//                 break;
//               } else if (response < 0) {
//                 std::cerr << "[ERROR] Failed to receive a frame from the decoder\n";
//                 break;
//               }

//               if (response >= 0) {
//                 break;
//               }
//             }
//             // Decode Frame
// //             if (response >= 0) {
// //                 QImage myImage = getQImageFromFrame(pFrame);
// // //                w.setImage(QPixmap::fromImage(myImage));
// //             }
//             // if (--how_many_packets_to_process < 0) break;
//             ++processed_frame;
//         }
//         av_packet_unref(pPacket);
//     }
//     std::cout << "Processed frame: " << processed_frame << "\n";
    
// }