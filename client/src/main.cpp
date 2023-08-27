#include <iostream>
#include <QWindow>
#include <QApplication>
//#include <QMainWindow>

extern "C"{
#   include "libavcodec/avcodec.h"
#   include "libavutil/mathematics.h"
#   include "libavcodec/avcodec.h"
}

int main(int argc, char *argv[]){
	QApplication app(argc, argv);

    AVCodec *codec;
    AVCodecContext *c = NULL;
    int i, out_size, size, x, y, outbuf_size;
    FILE *f;
    AVFrame *picture;
    uint8_t *outbuf, *picture_buf;
    AVPacket packet;

    codec = (AVCodec*)avcodec_find_encoder(AV_CODEC_ID_MPEG1VIDEO);
    if(!codec){
        fprintf(stderr, "codec not found");
        exit(1);
    }

    c = avcodec_alloc_context3(codec);
    picture = av_frame_alloc();

    c->bit_rate = 400000;
    c->width = 352;
    c->height = 288;
    c->time_base = (AVRational){1, 25};
    c->gop_size = 10;
    c->max_b_frames = 1;
    c->pix_fmt = AV_PIX_FMT_YUV420P;



    if(avcodec_open2(c, codec, NULL) < 0){
        fprintf(stderr, "coudld not open codec");
    }

    f = fopen("temp.mpg", "wb");
    if(!f){
        fprintf(stderr, "could not open file\n");
        exit(1);
    }

    outbuf_size = 100000;
    outbuf = (uint8_t *)malloc(outbuf_size);
    size = c->width * c->height;
    picture_buf = (uint8_t *)malloc((size * 3)/2); /* size for YUV 420*/

    picture->data[0] = picture_buf;
    picture->data[1] = picture->data[0] + size;
    picture->data[2] = picture->data[1] + size / 4;

    picture->linesize[0] = c->width;
    picture->linesize[1] = c->width / 2;
    picture->linesize[2] = c->width / 2;

    for(int i = 0; i < 25; i++){
        fflush(stdout);
        for(y=0;y<c->height;y++) {
            for(x=0;x<c->width;x++) {
                picture->data[0][y * picture->linesize[0] + x] = x + y + i * 3;
            }
        }

        /* Cb and Cr */
        for(y=0;y<c->height/2;y++) {
            for(x=0;x<c->width/2;x++) {
                picture->data[1][y * picture->linesize[1] + x] = 128 + y + i * 2;
                picture->data[2][y * picture->linesize[2] + x] = 64 + x + i * 5;
            }
        }

        /* encode the image */

        AVPacket packet;
        packet.data = outbuf;
        packet.size = outbuf_size;
        av_init_packet(&packet);

        out_size = avcodec_encode_video2(c, &packet, picture);
        printf("encoding frame %3d (size=%5d)\n", i, out_size);
        fwrite(outbuf, 1, out_size, f);
    }

	QWindow window;
	window.resize(256,256);
	window.show();
	return app.exec();
}
