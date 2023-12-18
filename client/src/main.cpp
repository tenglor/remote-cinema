

#include <iostream>
#include <QWindow>
#include <QApplication>
//#include <QMainWindow>
#include "imageviewwidget.h"
#include <png.h>
#include <QWindow>
#include <QPixmap>
#include <QByteArray>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}

extern "C" int decode_video(const char *filename);


static void save_gray_frame(unsigned char *buf, int wrap, int xsize, int ysize, char *filename)
{
    FILE *f;
    int i;
    f = fopen(filename,"w");
    // writing the minimal required header for a pgm file format
    // portable graymap format -> https://en.wikipedia.org/wiki/Netpbm_format#PGM_example
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);

    // writing line by line
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}

#define VIDEO_FILENAME "../data/afwfwf.mp4"
//#define VIDEO_FILENAME "/home/ivan/Videos/1702475584-760766239-15856a34-99e2-4d35-a03e-a0495c75c832.mp4"

int main(int argc, char *argv[]){
	QApplication app(argc, argv);

    ImageViewWidget widget;

    

    AVFormatContext *pFormatContext = avformat_alloc_context();    
    if(avformat_open_input(&pFormatContext, VIDEO_FILENAME, NULL, NULL) != 0){
        printf("It's error, bro\n");
        exit(1);
    }

    printf("Format %s, duration %" PRIu64 " s\n", pFormatContext->iformat->long_name, pFormatContext->duration);

    int a = avformat_find_stream_info(pFormatContext, NULL);
    printf("a: %d\n", a);

    AVCodecParameters *pCodecParameters = pFormatContext->streams[0]->codecpar;
    const AVCodec *pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
    if(pCodec == 0){
        printf("Cannot find codec\n");
        exit(1);
    }

    AVCodecContext *pCodecContext = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(pCodecContext, pCodecParameters);
    avcodec_open2(pCodecContext, pCodec, NULL);

    AVPacket *pPacket = av_packet_alloc();
    AVFrame *pFrame = av_frame_alloc();
    
    widget.show();

    int width = 0;
    int height = 0;
    widget.resize(800, 600);
    
    while(av_read_frame(pFormatContext, pPacket) >= 0){
        
        int response = avcodec_send_packet(pCodecContext, pPacket);

        while(response >= 0){
            response = avcodec_receive_frame(pCodecContext, pFrame);
            if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
                break;
            } else if (response < 0) {
                //char *err_str = av_err2str(response);
                //printf("Error while receiving a frame from the decoder: %s\n", err_str);
                return response;
            }            
                        
            printf(
                "Frame %d (type=%c, size=%d bytes, format=%d) pts %d key_frame %d [DTS %d]\n",
                pCodecContext->frame_number,
                av_get_picture_type_char(pFrame->pict_type),
                pFrame->pkt_size,
                pFrame->format,
                pFrame->pts,
                pFrame->key_frame,
                pFrame->coded_picture_number
            );

            save_gray_frame(pFrame->data[0], pFrame->linesize[0], pFrame->width, pFrame->height, "aaaa.pgm");
            
            QImage image(pFrame->width, pFrame->height, QImage::Format_Grayscale8);                 
            
            for(int y = 0, i = 0; y < pFrame->height; y++){
                for(int x = 0; x < pFrame->width; x++, i++){                                        
                    image.setPixel(x, y, pFrame->data[0][y * pFrame->linesize[0] + x]);
                }
            }            
            widget.setImage(image.scaled(800, 600, Qt::AspectRatioMode::KeepAspectRatio));
            
            QApplication::processEvents();

            // writing line by line            
        }
    }

    av_frame_free(&pFrame);
    av_packet_free(&pPacket);
    avformat_close_input(&pFormatContext);
    //encode_video();    
    // decode_video("../data/afwfwf.mp4");


    //widget.resize(256,256);
	
    return app.exec();
}
