#pragma once
#ifndef FFMLIB_H
#define FFMLIB_H

#define __STDC_CONSTANT_MACROS
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libavutil/imgutils.h>
#include <libavutil/parseutils.h>
}


void ffmpeg_init(void);

int open_input_file(const char *filename);
int open_output_file(const char *filename);
int init_filters(void);

void close_filters(void);
void close_input_file(void);
void close_output_file(void);

bool read_video_package(AVPacket *packet);
bool get_frame_from_package(AVPacket *packet, AVFrame *frame, int *got_frame);
AVFrame *transfer_frame(AVPacket *packet, AVFrame *frame, SwsContext* swsContext);
bool read_frame();
int filter_encode_write_frame(AVFrame *frame, unsigned int stream_index);
int encode_write_frame(AVFrame *filt_frame, unsigned int stream_index, int *got_frame);
int flush_encoder(unsigned int stream_index);
void write_file_tail(void);

#endif
