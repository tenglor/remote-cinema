

#include <iostream>
#include <QWindow>
#include <QApplication>
//#include <QMainWindow>
#include "imageviewwidget.h"

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}

extern "C" int decode_video(const char *filename);

int main(int argc, char *argv[]){
	// QApplication app(argc, argv);

    // ImageViewWidget widget;
    // widget.show();

    AVFormatContext *pFormatContext = avformat_alloc_context();
    if(avformat_open_input(&pFormatContext, "../data/afwfwf.mp4", NULL, NULL) != 0){
        printf("It's error, bro\n");
        exit(1);
    }

    printf("Format %s, duration %lld s\n", pFormatContext->iformat->long_name, pFormatContext->duration);

    int a = avformat_find_stream_info(pFormatContext, NULL);
    printf("a: %d\n", a);

    avformat_close_input(&pFormatContext);
    //encode_video();    
    // decode_video("../data/afwfwf.mp4");


    //widget.resize(256,256);
	
    return 0;
}
