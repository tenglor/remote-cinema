#include <iostream>
#include <QWindow>
#include <QApplication>
//#include <QMainWindow>
#include "imageviewwidget.h"

extern "C"{
#   include "libavcodec/avcodec.h"
#   include "libavutil/mathematics.h"
#   include "libavcodec/avcodec.h"
}

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

    len = avcodec_decode_video2(avctx, frame, &got_frame, pkt);
    if (len < 0) {
        fprintf(stderr, "Error while decoding frame %d\n", *frame_count);
        return len;
    }
    if (got_frame) {
        printf("Saving %sframe %3d\n", last ? "last " : "", *frame_count);
        fflush(stdout);

        /* the picture is allocated by the decoder, no need to free it */
        snprintf(buf, sizeof(buf), "%s-%d", outfilename, *frame_count);
        pgm_save(frame->data[0], frame->linesize[0],
                 frame->width, frame->height, buf);
        (*frame_count)++;
    }
    if (pkt->data) {
        pkt->size -= len;
        pkt->data += len;
    }
    return 0;
}

int decode_video(ImageViewWidget *widget){
    AVCodec *codec;
    AVCodecContext *c = NULL;
    FILE *f;
    AVFrame *frame;

    AVPacket packet;
    static uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];

    av_init_packet(&packet);

    codec = avcodec_find_decoder(AV_CODEC_ID_MPEG1VIDEO);
    if(!codec){
        std::cerr << "aaaaa" << std::endl;
        return 1;
    }

    c = avcodec_alloc_context3(codec);

    avcodec_open2(c, codec, NULL);

    f = fopen("temp.mpg", "rb");

    frame = av_frame_alloc();

    int frame_count = 0;
    for(;;){
        packet.size = fread(inbuf, 1, INBUF_SIZE, f);
        if(packet.size == 0) break;

        packet.data = inbuf;

        while(packet.size > 0){
            if(decode_write_frame("temp.mpg", c, frame, &frame_count, &packet, 0) < 0){
                exit(1);
            }
            printf("width %d\n", frame->width);
            printf("height %d\n", frame->height);
            widget->resize(frame->width, frame->height);
            //QImage image(packet.buf, frame->width, frame->height, QImage::Format::For)
            //widget->setImage(packet.buf);
        }

    }

}

int encode_video(){
    AVCodec *codec;
    AVCodecContext *c = NULL;
    int i, out_size, size, x, y, outbuf_size;
    FILE *f;
    AVFrame *frame;
    uint8_t *outbuf, *picture_buf;
    AVPacket packet;
    int ret;


    codec = (AVCodec*)avcodec_find_encoder(AV_CODEC_ID_MPEG1VIDEO);
    if(!codec){
        fprintf(stderr, "codec not found");
        exit(1);
    }

    c = avcodec_alloc_context3(codec);

    c->bit_rate = 400000;
    c->width = 352;
    c->height = 288;
    c->time_base = (AVRational){1, 25};
    c->framerate = (AVRational){25, 1};
    c->gop_size = 10;
    c->max_b_frames = 1;
    //c->pix_fmt = AV_PIX_FMT_RGB8;
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
    //picture_buf = (uint8_t *)malloc((size * 3)/2); /* size for YUV 420*/
    picture_buf = (uint8_t *)malloc((size * 4)); /* size for RGB 8*/

    frame = av_frame_alloc();
    frame->format = c->pix_fmt;
    frame->width = c->width;
    frame->height = c->height;

    ret = av_frame_get_buffer(frame, 32);
    if(ret < 0){
        fprintf(stderr, "could not allocate something\n");
        exit(1);
    }



    int got_output = 0;

    for(int i = 0; i < 25; i++){
        av_init_packet(&packet);
        packet.data = NULL;
        packet.size = 0;

        fflush(stdout);

        av_frame_make_writable(frame);
        if(ret < 0) exit(1);

        for(y=0;y<c->height;y++) {
            for(x=0;x<c->width;x++) {
                frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
//                frame->data[0][y * frame->linesize[0] + x + 0] = x + y + i * 3;
//                frame->data[0][y * frame->linesize[0] + x + 1] = x + y + i * 3;
//                frame->data[0][y * frame->linesize[0] + x + 2] = x + y + i * 3;
            }
        }

        /* Cb and Cr */
        for(y=0;y<c->height/2;y++) {
            for(x=0;x<c->width/2;x++) {
                frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
                frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
            }
        }

        /* encode the image */

        ret = avcodec_encode_video2(c, &packet, frame, &got_output);
        printf("encoding frame %3d (size=%5d)\n", i, out_size);
        fwrite(outbuf, 1, out_size, f);

        if (got_output) {
            printf("Write frame %3d (size=%5d)\n", i, packet.size);
            fwrite(packet.data, 1, packet.size, f);
            av_packet_unref(&packet);
        }

    }

    for(; out_size; i++) {
        fflush(stdout);

        out_size = avcodec_encode_video2(c, &packet, frame, &got_output);
        printf("write frame %3d (size=%5d)\n", i, out_size);
        fwrite(outbuf, 1, out_size, f);
    }


    outbuf[0] = 0x00;
    outbuf[1] = 0x00;
    outbuf[2] = 0x01;
    outbuf[3] = 0xFF;

    fwrite(outbuf, 1, 4, f);
    fclose(f);
    free(picture_buf);
    free(outbuf);

    avcodec_close(c);
    av_free(c);
    av_free(frame);
}

int main(int argc, char *argv[]){
	QApplication app(argc, argv);

    ImageViewWidget widget;
    widget.show();

    encode_video();
    return 0;
    //decode_video(&widget);


    //widget.resize(256,256);

	return app.exec();
}
