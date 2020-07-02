#ifndef FFMLIB_H
#define FFMLIB_H

#ifdef FFMLIB_C
#define X_FFMLIB
#else
#define X_FFMLIB extern
#endif

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

/***** Public constant definition *****/
#define MAX_FILES 20
#define MAX_STREAMS 20
X_FFMLIB AVFormatContext *input_files[MAX_FILES];
X_FFMLIB AVCodecContext *input_codec_context[MAX_FILES*MAX_STREAMS];
X_FFMLIB unsigned int nb_input_files;
X_FFMLIB unsigned int nb_icodec_context;

X_FFMLIB AVFormatContext *output_files;
X_FFMLIB AVCodecContext *output_codec_context[MAX_STREAMS];
X_FFMLIB unsigned int nb_ocodec_context;

X_FFMLIB AVFormatContext *ifmt_ctx;
X_FFMLIB AVFormatContext *ofmt_ctx;
X_FFMLIB unsigned int video_stream_index;

typedef struct FilteringContext {
	AVFilterContext *buffersink_ctx;
	AVFilterContext *buffersrc_ctx;
	AVFilterGraph *filter_graph;
} FilteringContext;
X_FFMLIB FilteringContext *filter_ctx;

typedef struct StreamContext {
	AVCodecContext *dec_ctx;
	AVCodecContext *enc_ctx;
} StreamContext;
X_FFMLIB StreamContext *stream_ctx;

/***** Public function prototype definition  ******/
X_FFMLIB void ffmpeg_init(void);

X_FFMLIB void set_cur_input_file(unsigned int index);
X_FFMLIB int open_input_file(const char *filename);
X_FFMLIB int open_output_file(const char *filename);
X_FFMLIB int init_filters(void);

X_FFMLIB void close_filters(void);
X_FFMLIB void close_input_file(void);
X_FFMLIB void close_output_file(void);

X_FFMLIB bool read_video_package(AVPacket *packet);
X_FFMLIB bool get_frame_from_package(AVPacket *packet, AVFrame *frame, int *got_frame);
X_FFMLIB AVFrame *transfer_frame(AVPacket *packet, AVFrame *frame, SwsContext* swsContext);
X_FFMLIB int filter_encode_write_frame(AVFrame *frame, unsigned int stream_index);
X_FFMLIB int encode_write_frame(AVFrame *filt_frame, unsigned int stream_index, int *got_frame);
X_FFMLIB int flush_encoder(unsigned int stream_index);
X_FFMLIB void write_file_tail(void);

#endif
