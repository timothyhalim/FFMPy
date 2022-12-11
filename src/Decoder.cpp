#include "Decoder.h"

Decoder::Decoder() {
    this->debug(false);
    this->_reset();
};

Decoder::Decoder(std::string filepath) {
    this->debug(false);
    this->set_filepath(filepath);
};

Decoder::~Decoder() {
    if (this->_debug) {
        std::cout << "DESTROYED!\n";
    }
    this->_reset();
};

bool Decoder::debug(bool state) {
    this->_debug = state;
    return this->_debug;
}

void Decoder::_reset() {
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
    this->_reset();
    this->filepath = filepath;
    AVFormatContext* pFormatContext = this->open();
    this->_get_file_info(pFormatContext);
    this->close(pFormatContext);
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

void Decoder::_get_file_info(AVFormatContext* pFormatContext) {
    int video_stream_index = -1;

    for (unsigned int i = 0; i < pFormatContext->nb_streams; i++)
    {
        AVCodecParameters *pLocalCodecParameters =  NULL;
        pLocalCodecParameters = pFormatContext->streams[i]->codecpar;

        const AVCodec *pLocalCodec = NULL;

        pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);
        if (pLocalCodec==NULL) {
            std::cerr << "[ERROR] Unsupported codec!\n";
            continue;
        }
        if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
            if (video_stream_index == -1) {
                video_stream_index = i;
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
}

AVFormatContext* Decoder::open() {
    this->_is_open = false;

    if (this->filepath.empty()){
        std::cerr << "[ERROR] Filepath is not set yet!\n";
    };

    AVFormatContext* pFormatContext = avformat_alloc_context();
    if (!pFormatContext) {
        std::cerr << "[ERROR] Could not allocate memory for Format Context\n";
    }

    if (avformat_open_input(&pFormatContext, this->filepath.c_str(), NULL, NULL) != 0) {
        std::cerr << "[ERROR] Could not open the file\n";
    }

    if (avformat_find_stream_info(pFormatContext,  NULL) < 0) {
        std::cerr << "[ERROR] Could not get the stream info";
    }
    
    this->_is_open = true;
    return pFormatContext;
}

int Decoder::close(AVFormatContext* pFormatContext) {
    avformat_close_input(&pFormatContext);
    this->_is_open = false;
    return 0;
}

PyObject* Decoder::extract_frame(int64_t frame) {
    if (this->_debug) {
        std::cout << "Extracting frame " << frame <<"\n";
    }

    if (frame >= this->video_frame_count) {
        frame = this->video_frame_count - 1;
    } else if (frame < 0) {
        frame = 0;
    }

    PyObject* byte_list = PyList_New(static_cast<Py_ssize_t>(0));
    
    AVFormatContext *pFormatContext = this->open();

    const AVCodec *pCodec = NULL;
    AVCodecParameters *pCodecParameters = NULL;
    AVStream* pVideoStream = NULL;
    int video_stream_index = -1;

    for (unsigned int i = 0; i < pFormatContext->nb_streams; i++)
    {
        AVCodecParameters *pLocalCodecParameters =  NULL;
        pLocalCodecParameters = pFormatContext->streams[i]->codecpar;

        const AVCodec *pLocalCodec = NULL;

        pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

        if (pLocalCodec==NULL) {
            std::cout << "ERROR unsupported codec!";
            continue;
        }
        if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
            if (video_stream_index == -1) {
                video_stream_index = i;
                pCodec = pLocalCodec;
                pCodecParameters = pLocalCodecParameters;
                pVideoStream = pFormatContext->streams[video_stream_index];
            }
        } else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
        }
    }

    if (video_stream_index == -1) {
        std::cerr << "[ERROR] File does not contain a video stream!";
    }
    AVCodecContext *pCodecContext = avcodec_alloc_context3(pCodec);
    if (!pCodecContext)
    {
        std::cerr << "[ERROR] Failed to allocated memory for AVCodecContext";
    }

    if (avcodec_parameters_to_context(pCodecContext, pCodecParameters) < 0)
    {
        std::cerr << "[ERROR] Failed to copy codec params to codec context";
    }

    if (avcodec_open2(pCodecContext, pCodec, NULL) < 0)
    {
        std::cerr << "[ERROR] Failed to open codec through avcodec_open2";
    }

    AVFrame *pFrame = av_frame_alloc();
    if (!pFrame)
    {
        std::cerr << "[ERROR] Failed to allocate memory for AVFrame";
    }

    AVPacket *pPacket = av_packet_alloc();
    if (!pPacket)
    {
        std::cerr << "[ERROR] Failed to allocate memory for AVPacket";
    }

    avcodec_flush_buffers(pCodecContext);
    // SEEK TO FRAME
    
    auto time_base = pVideoStream->time_base;
    auto frame_base = pVideoStream->avg_frame_rate;
    auto seekTimeStamp = pVideoStream->start_time 
                        + av_rescale(
                            av_rescale(frame, time_base.den, time_base.num), 
                            frame_base.den, frame_base.num
                        );
    if (this->_debug) {
        std::cout << "Seeking to frame " << frame << ", timestamp " << seekTimeStamp << "\n";
    }
    if (av_seek_frame(pFormatContext, video_stream_index, seekTimeStamp, AVSEEK_FLAG_BACKWARD) < 0) {
        std::cerr << "AV seek frame fail!" << "\n";
        av_seek_frame(pFormatContext, -1, 0, AVSEEK_FLAG_BACKWARD);
    }

    int response = 0;
    bool found = false;

    while (av_read_frame(pFormatContext, pPacket) >= 0)
    {
        // if it's the video stream
        if (pPacket->stream_index == video_stream_index) {
            if (this->_debug) {
                std::cout << "AVPacket->pts " << pPacket->pts << "\n";
            }
            response = avcodec_send_packet(pCodecContext, pPacket);
            if (response < 0) {
                std::cerr << "Error while sending a packet to the decoder";
                break;
            }

            SwsContext *swsCtx = sws_getContext(
                    pCodecContext->width, pCodecContext->height, pCodecContext->pix_fmt, 
                    pCodecContext->width, pCodecContext->height, AV_PIX_FMT_RGB24, 
                    SWS_BICUBIC, NULL, NULL, NULL
                );

            while (response >= 0){
                    response = avcodec_receive_frame(pCodecContext, pFrame);
                if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
                    break;
                } else if (response < 0) {
                    std::cerr << "Error while receiving a frame from the decoder";
                    break;
                }

                if (response >= 0) {
                    if (pPacket->pts < seekTimeStamp) {
                        break;
                    }

                    AVFrame *pFrameRGB = av_frame_alloc();
                    av_frame_copy_props(pFrameRGB, pFrame);
                    pFrameRGB->width = pFrame->width;
                    pFrameRGB->height = pFrame->height;
                    pFrameRGB->format = AV_PIX_FMT_RGB24;
                    av_frame_get_buffer(pFrameRGB, 0);

                    sws_scale(swsCtx, pFrame->data, pFrame->linesize, 0, 
                              pCodecContext->height, pFrameRGB->data, pFrameRGB->linesize);
    
                    for(int y = 0; y < pCodecContext->height; ++y)
                    {
                        for(int x = 0; x < pCodecContext->width; ++x)
                        {
                            int p = x * 3 + y * pFrameRGB->linesize[0];
                            int r = pFrameRGB->data[0][p];
                            int g = pFrameRGB->data[0][p+1];
                            int b = pFrameRGB->data[0][p+2];
                            PyList_Append(byte_list, PyLong_FromLong(r));
                            PyList_Append(byte_list, PyLong_FromLong(g));
                            PyList_Append(byte_list, PyLong_FromLong(b));
                        }
                    }
                    found = true;
                    break;
                }
                av_frame_unref(pFrame);
            }
        }
        av_packet_unref(pPacket);
        if (found){
            break;
        }
    }

    avformat_close_input(&pFormatContext);
    av_packet_free(&pPacket);
    av_frame_free(&pFrame);
    avcodec_free_context(&pCodecContext);

    PyObject* bytes = PyBytes_FromObject(byte_list);
    return bytes;
}