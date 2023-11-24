

#include "libavcodec/avcodec.h"
#include "libavutil/mathematics.h"
#include "libavutil/avutil.h"
#include "stdio.h"

#define INBUF_SIZE 4096
//#define INBUF_SIZE 1000000


static void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,
                     char *filename)
{
    FILE *f;
    int i;

    f = fopen(filename,"w");
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}

static int decode_write_frame(const char *outfilename, AVCodecContext *avctx,
                              AVFrame *frame, int *frame_count, AVPacket *pkt, int last)
{
    int len, got_frame;
    char buf[1024];
    int ret;

    ret = avcodec_send_packet(avctx, pkt);
    if (ret < 0) {
        fprintf(stderr, "Error sending a packet for decoding\n");
        exit(1);
    }
        
    //len = avcodec_decode_video2(avctx, frame, &got_frame, pkt);
    while (ret >= 0) {  
        ret = avcodec_receive_frame(avctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return 0;
        else if (ret < 0) {
            fprintf(stderr, "Error during decoding\n");
            exit(1);
        }

        fflush(stdout);
         /* the picture is allocated by the decoder. no need to
           free it */        
        pgm_save(frame->data[0], frame->linesize[0],
                 frame->width, frame->height, buf);
    }
    return 0;
}

int decode_video(const char *filename){
    const AVCodec *codec;
    AVCodecContext *context = NULL;
    FILE *f;
    AVFrame *frame;
            

    AVPacket packet;
    static uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];

    av_init_packet(&packet);

    //codec = avcodec_find_decoder(AV_CODEC_ID_MPEG1VIDEO);
    codec = avcodec_find_decoder(AV_CODEC_ID_MPEG4);
    if(!codec){
        printf("aaaaa\n");        
        return 1;
    }

    context = avcodec_alloc_context3(codec);

    int error = avcodec_open2(context, codec, NULL);
    printf("open error: %d\n", error);

    f = fopen(filename, "rb");
    if(f == NULL){
        return -1;
    }

    frame = av_frame_alloc();
    int ret = avcodec_send_packet(context, &packet);
    if(ret < 0)
        exit(1);

    int frame_count = 0;
    for(;;){
        packet.size = fread(inbuf, 1, INBUF_SIZE, f);
        if(packet.size == 0) break;

        packet.data = inbuf;        

        while(packet.size > 0){
            if(decode_write_frame(filename, context, frame, &frame_count, &packet, 0) < 0){
                exit(1);
            }
            printf("width %d\n", frame->width);
            printf("height %d\n", frame->height);
            //widget->resize(frame->width, frame->height);
            //QImage image(packet.buf, frame->width, frame->height, QImage::Format::For)
            //widget->setImage(packet.buf);
        }

    }
    return 0;
}