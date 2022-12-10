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
        int open_file();

    private:
        std::string filepath;
  
    // static  getQImageFromFrame(const AVFrame* pFrame);

//   ptrdiff_t parse(const unsigned char* in_data, ptrdiff_t in_size);
//   bool is_frame_available() const;
//   const AVFrame& decode_frame();
};
