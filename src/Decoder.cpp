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
    this->close_stream();
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
    
    this->_is_open = false;
    this->_is_streaming = false;
}

std::string Decoder::set_filepath(std::string filepath) {
    this->_reset();
    this->filepath = filepath;
    this->open();
    this->_get_file_info();
    this->close();
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

    return res;
}

void Decoder::_get_file_info() {
    int video_stream_index = -1;

    for (unsigned int i = 0; i < this->pFormatContext->nb_streams; i++) {
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
            }
            this->video_width = pLocalCodecParameters->width;
            this->video_height = pLocalCodecParameters->height;
            this->video_codec = pLocalCodec->name;
            this->video_bit_rate = pLocalCodecParameters->bit_rate;
            this->video_frame_rate = this->pFormatContext->streams[video_stream_index]->r_frame_rate.num 
                                   / this->pFormatContext->streams[video_stream_index]->r_frame_rate.den;

            this->video_time_base = this->pFormatContext->streams[video_stream_index]->time_base.num 
                                  * this->pFormatContext->streams[video_stream_index]->time_base.den;
            this->video_duration = this->pFormatContext->streams[video_stream_index]->duration / this->video_time_base;
            this->video_frame_count = av_rescale(
                                    static_cast<int64_t>(this->video_duration * 0xFFFF), 
                                    this->pFormatContext->streams[video_stream_index]->r_frame_rate.num, 
                                    this->pFormatContext->streams[video_stream_index]->r_frame_rate.den
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

int Decoder::open() {
    this->_is_open = 0;

    if (this->filepath.empty()) {
        std::cerr << "[ERROR] Filepath is not set yet!\n";
    };

    this->pFormatContext = avformat_alloc_context();
    if (!pFormatContext) {
        std::cerr << "[ERROR] Could not allocate memory for Format Context\n";
        return this->_is_open;
    }

    if (avformat_open_input(&this->pFormatContext, this->filepath.c_str(), NULL, NULL) != 0) {
        std::cerr << "[ERROR] Could not open the file\n";
        return this->_is_open;
    }

    if (avformat_find_stream_info(this->pFormatContext,  NULL) < 0) {
        std::cerr << "[ERROR] Could not get the stream info";
        return this->_is_open;
    }
    
    this->_is_open = 1;
    return this->_is_open;
}

int Decoder::close() {
    if (this->_is_open) {
        avformat_close_input(&this->pFormatContext);
    }
    this->_is_open = false;
    this->pFormatContext = NULL;
    return 0;
}

int Decoder::open_stream() {
    if (this->_debug) {
        std::cout << "Opening stream\n";
    }
    this->_is_streaming = 0;
    
    this->open();

    const AVCodec *pCodec = NULL;
    AVCodecParameters *pCodecParameters = NULL;

    this->pVideoStream = NULL;
    this->video_stream_index = -1;

    for (unsigned int i = 0; i < this->pFormatContext->nb_streams; i++) {
        AVCodecParameters *pLocalCodecParameters =  NULL;
        pLocalCodecParameters = this->pFormatContext->streams[i]->codecpar;

        const AVCodec *pLocalCodec = NULL;

        pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

        if (pLocalCodec==NULL) {
            std::cerr << "[ERROR] Unsupported codec!";
            continue;
        }
        if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
            if (video_stream_index == -1) {
                video_stream_index = i;
                pCodec = pLocalCodec;
                pCodecParameters = pLocalCodecParameters;
                this->pVideoStream = this->pFormatContext->streams[video_stream_index];
            }
        } else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {

        }
    }

    if (video_stream_index == -1) {
        std::cerr << "[ERROR] File does not contain a video stream!";
        return this->_is_streaming;
    }
    
    this->pCodecContext = avcodec_alloc_context3(pCodec);
    if (!this->pCodecContext) {
        std::cerr << "[ERROR] Failed to allocated memory for AVCodecContext";
        return this->_is_streaming;
    }

    if (avcodec_parameters_to_context(this->pCodecContext, pCodecParameters) < 0) {
        std::cerr << "[ERROR] Failed to copy codec params to codec context";
        return this->_is_streaming;
    }

    if (avcodec_open2(this->pCodecContext, pCodec, NULL) < 0) {
        std::cerr << "[ERROR] Failed to open codec through avcodec_open2";
        return this->_is_streaming;
    }

    this->pFrame = av_frame_alloc();
    if (!this->pFrame) {
        std::cerr << "[ERROR] Failed to allocate memory for AVFrame";
        return this->_is_streaming;
    }

    this->pPacket = av_packet_alloc();
    if (!this->pPacket) {
        std::cerr << "[ERROR] Failed to allocate memory for AVPacket";
        return this->_is_streaming;
    }

    this->swsCtx = sws_getContext(
            this->pCodecContext->width, this->pCodecContext->height, this->pCodecContext->pix_fmt, 
            this->pCodecContext->width, this->pCodecContext->height, AV_PIX_FMT_RGB24, 
            SWS_BICUBIC, NULL, NULL, NULL
        );

    avcodec_flush_buffers(this->pCodecContext);
    this->_is_streaming = 1;
    return this->_is_streaming;
}

int Decoder::close_stream() {
    if (this->_is_streaming) {
        avformat_close_input(&this->pFormatContext);
        av_packet_free(&this->pPacket);
        av_frame_free(&this->pFrame);
        avcodec_free_context(&this->pCodecContext);
    }
    if (this->_debug) {
        std::cout << "Closing stream\n";
    }
    this->close();
    this->_is_streaming = 0;
    return true;
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
    
    bool extract_one_frame = false;
    if (!this->_is_streaming){
        extract_one_frame = true;
        this->open_stream();
    }

    if (this->_is_streaming) {
        // SEEK TO FRAME
        
        auto time_base = this->pVideoStream->time_base;
        auto frame_base = this->pVideoStream->avg_frame_rate;
        auto seekTimeStamp = this->pVideoStream->start_time 
                            + av_rescale(
                                av_rescale(frame, time_base.den, time_base.num), 
                                frame_base.den, frame_base.num
                            );
        if (this->_debug) {
            std::cout << "Seeking to frame " << frame << ", timestamp " << seekTimeStamp << "\n";
        }
        if (av_seek_frame(this->pFormatContext, video_stream_index, seekTimeStamp, AVSEEK_FLAG_BACKWARD) < 0) {
            std::cerr << "AV seek frame fail!" << "\n";
            av_seek_frame(this->pFormatContext, -1, 0, AVSEEK_FLAG_BACKWARD);
        }

        int response = 0;
        bool found = false;

        while (av_read_frame(this->pFormatContext, this->pPacket) >= 0) {
            // if it's the video stream
            if (this->pPacket->stream_index == video_stream_index) {
                if (this->_debug) {
                    std::cout << "AVPacket->pts " << this->pPacket->pts << "\n";
                }
                response = avcodec_send_packet(this->pCodecContext, this->pPacket);
                if (response < 0) {
                    std::cerr << "Error while sending a packet to the decoder";
                    break;
                }

                while (response >= 0){
                    response = avcodec_receive_frame(this->pCodecContext, this->pFrame);
                    if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
                        break;
                    } else if (response < 0) {
                        std::cerr << "Error while receiving a frame from the decoder";
                        break;
                    }

                    if (response >= 0) {
                        if (this->pFrame->pts < seekTimeStamp) {
                            break;
                        }

                        AVFrame *pFrameRGB = av_frame_alloc();
                        av_frame_copy_props(pFrameRGB, this->pFrame);
                        pFrameRGB->width = this->pFrame->width;
                        pFrameRGB->height = this->pFrame->height;
                        pFrameRGB->format = AV_PIX_FMT_RGB24;
                        av_frame_get_buffer(pFrameRGB, 0);

                        sws_scale(this->swsCtx, this->pFrame->data, this->pFrame->linesize, 0, 
                                this->pCodecContext->height, pFrameRGB->data, pFrameRGB->linesize);
        
                        if (this->_debug) {
                            std::cout << "Frame linesize " << pFrameRGB->linesize[0] << "\n";
                            std::cout << "Frame width " << pFrameRGB->width << "\n";
                            std::cout << "Frame height " << pFrameRGB->height << "\n";
                        }

                        // This looping method seems slow
                        for(int y = 0; y < pFrameRGB->height; ++y) {
                            for(int x = 0; x < pFrameRGB->width; ++x) {
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
                        av_frame_free(&pFrameRGB);
                        break;
                    }
                    av_frame_unref(this->pFrame);
                }
            }
            av_packet_unref(this->pPacket);
            if (found){
                break;
            }
        }
    }
    if (extract_one_frame) {
        this->close_stream();
    }

    PyObject* bytes = PyBytes_FromObject(byte_list);
    return bytes;
}