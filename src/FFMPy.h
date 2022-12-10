#ifndef FFMPy
#define FFMPy

#include <Python.h>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/mem.h>
    #include <libswscale/swscale.h>
}

/* For backward compatibility with release 9 or so of libav */
#if (LIBAVCODEC_VERSION_MAJOR <= 54) 
#  define av_frame_alloc avcodec_alloc_frame
#  define av_frame_free  avcodec_free_frame  
#endif


#endif // !FFMPy