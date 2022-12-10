#pragma once
#include "FFMPy.h"

class Decoder
{
    AVCodecContext        *context;
    AVFrame               *frame;
    AVCodec               *codec;
    AVCodecParserContext  *parser;
    
    AVPacket              *pkt;
public:
    Decoder();
    ~Decoder();
  
    // static int open_file(char *file);
    // static  getQImageFromFrame(const AVFrame* pFrame);

//   ptrdiff_t parse(const unsigned char* in_data, ptrdiff_t in_size);
//   bool is_frame_available() const;
//   const AVFrame& decode_frame();
};
