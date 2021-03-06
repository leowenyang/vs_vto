#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


#include "ffmlib.h"
#include "opglib.h"

int main(int argc, char **argv)
{
	int ret;
	float progress = 0.0;
	int got_frame = 0;
	AVPacket packet;
	packet.data = NULL;
	packet.size = 0;
	AVFrame *frame = av_frame_alloc();

	AVPacket packet2;
	packet2.data = NULL;
	packet2.size = 0;
	AVFrame *frame2 = av_frame_alloc();

	AVFrame *gl_frame = av_frame_alloc();

	uint8_t *src_data[4], *dst_data[4];
	int src_linesize[4], dst_linesize[4];

	double pts;

	if (argc != 4) {
		printf("Usage: %s <input file 1> <input file 2> <output file>\n", argv[0]);
		return 1;
	}

	ffmpeg_init();

	if ((ret = open_input_file(argv[1])) < 0) {
		printf("Usage: %s <input file 1> <input file 2> <output file>\n", argv[0]);
		return 1;
	}

	if ((ret = open_input_file(argv[2])) < 0) {
		printf("Usage: %s <input file 1> <input file 2> <output file>\n", argv[0]);
		return 1;
	}

	if ((ret = open_output_file(argv[3])) < 0) {
		printf("Usage: %s <input file 1> <input file 2> <output file>\n", argv[0]);
		return 1;
	}

	if ((ret = init_filters()) < 0) {
		printf("Usage: %s <input file 1> <input file 2> <output file>\n", argv[0]);
		return 1;
	}

	unsigned int src_w = input_codec_context[0]->width;
	unsigned int src_h = input_codec_context[0]->height;
	enum AVPixelFormat src_pix_fmt = input_codec_context[0]->pix_fmt;

	struct SwsContext* swsContext = NULL;
	struct SwsContext* gl_swsContext = NULL;

	swsContext = sws_getContext(
		src_w, src_h, src_pix_fmt,
		src_w, src_h, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);
	if (!swsContext) {
		printf("sws_getCachedContext failed!\n");
		return NULL;
	}

	// SET FRAME BUFFER SIZE
	int width = WINDOW_WIDTH;
	int height = WINDOW_HEIGHT;
	GLFWwindow *window = opengl_window_init(width, height);

	opengl_init(width, height);

	GLuint VAO, VBO, EBO;
	load_VBO_VAO_EBO(VBO, VAO, EBO);
	GLuint shaderProgram;
	// shaderProgram = loadShaders("./shader/vertexshader.glsl", "./shader/fragmentshader.glsl");
	shaderProgram = loadShaders("./shader/vertexshader.glsl", "./shader/1-directional_frag.glsl");

	glUseProgram(shaderProgram);

	GLubyte *pixelBuffer = createPixelBuffer(width, height);
	gl_swsContext = sws_getContext(
		width, height, AV_PIX_FMT_RGB24,
		src_w, src_h, src_pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);
	if (!gl_swsContext) {
		printf("sws_getCachedContext failed!\n");
		return NULL;
	}

	gl_frame->width = width;
	gl_frame->height = height;
	gl_frame->format = AV_PIX_FMT_RGB24;
	if ((ret = av_image_fill_arrays(gl_frame->data, gl_frame->linesize, pixelBuffer,
		AV_PIX_FMT_RGB24, width, height,  16)) < 0) {
		fprintf(stderr, "Could not allocate destination image\n");
	}

	while (!glfwWindowShouldClose(window))
	{
		printf("loop");

		glfwGetFramebufferSize(window, &width, &height);
		
		// video 1
		set_input_file(0);
		ret = read_frame_from_video(&packet, frame);
		//if (ret == READ_FRAME_PACKET_ERROR) break;
		if (ret == READ_FRAME_FRAME_ERROR) goto end;
		if (!check_to_end_time(frame, 10.0)) {
			ret = filter_encode_write_frame(frame, packet.stream_index);
			av_packet_unref(&packet);
			pts = frame->pts + 1;
			continue;
		}

		if ((ret = av_image_alloc(src_data, src_linesize,
			src_w, src_h, AV_PIX_FMT_RGB24, 16)) < 0) {
			fprintf(stderr, "Could not allocate destination image\n");
		}

		/* convert to destination format */
		sws_scale(swsContext, frame->data,
			frame->linesize, 0, src_h, src_data, src_linesize);

		glUseProgram(shaderProgram);
		GLuint texture1 = loadFrameTexture((GLchar *)src_data[0], frame->width, frame->height);
		av_freep(&src_data[0]);
		glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

		// video 2
		set_input_file(1);
		ret = read_frame_from_video(&packet2, frame2);
		if (ret == READ_FRAME_PACKET_ERROR) break;
		if (ret == READ_FRAME_FRAME_ERROR) goto end;

		if ((ret = av_image_alloc(dst_data, dst_linesize,
			src_w, src_h, AV_PIX_FMT_RGB24, 16)) < 0) {
			fprintf(stderr, "Could not allocate destination image\n");
		}

		/* convert to destination format */
		sws_scale(swsContext, frame2->data,
			frame2->linesize, 0, src_h, dst_data, dst_linesize);

		glUseProgram(shaderProgram);
		GLuint texture2 = loadFrameTexture((GLchar *)dst_data[0], frame2->width, frame2->height);
		av_freep(&dst_data[0]);
		glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);

		if (progress < 1.0) 
		{
			progress += 0.01;
			glUniform1f(glGetUniformLocation(shaderProgram, "progress"), progress);

			// ������
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture2);
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			glDeleteTextures(0, &texture1);
			glDeleteTextures(1, &texture2);

			// STEP 9: GLFW SWAP BUFFER
			glfwSwapBuffers(window);
			// STEP 10: EVENT 
			glfwPollEvents();

			snap_shot(width, height, (GLubyte *)gl_frame->data[0]);

			AVFrame *filt_frame;
			filt_frame = av_frame_alloc();
			filt_frame->format = frame->format;
			filt_frame->width = frame->width;
			filt_frame->height = frame->height;
			filt_frame->pts = pts + frame2->pts;
			//filt_frame->pts = frame->pts;
			av_frame_get_buffer(filt_frame, 0);
			av_frame_make_writable(filt_frame);

			/* convert to destination format */
			ret = sws_scale(gl_swsContext, gl_frame->data,
				gl_frame->linesize, 0, height, filt_frame->data, filt_frame->linesize);
			ret = filter_encode_write_frame(filt_frame, packet.stream_index);
			av_frame_free(&filt_frame);
		}
		else {
			frame2->pts += pts;
			ret = filter_encode_write_frame(frame2, packet2.stream_index);
		}

		av_packet_unref(&packet);
		av_packet_unref(&packet2);
	}

	write_file_tail();
end:
	freePixelBuffer(pixelBuffer);
	close_opengl_window(window);

	sws_freeContext(swsContext);
	sws_freeContext(gl_swsContext);
	av_frame_free(&gl_frame);
	av_frame_free(&frame);
	av_frame_free(&frame2);

	close_filters();
	close_input_codec();
	close_input_file();
	close_output_codec();
	close_output_file();
}
