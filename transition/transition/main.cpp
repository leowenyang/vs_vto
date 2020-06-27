#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <windows.h> 

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
	AVFrame *rgb_frame = NULL;
	AVFrame *gl_frame = av_frame_alloc();

	uint8_t *src_data[4], *dst_data[4];
	int src_linesize[4], dst_linesize[4];
	int src_w = 720, src_h = 404, dst_w, dst_h;
	enum AVPixelFormat src_pix_fmt = AV_PIX_FMT_YUV420P, 
		dst_pix_fmt = AV_PIX_FMT_RGB24,
		gl_pix_fmt = AV_PIX_FMT_RGB24;

	if (argc != 4) {
		printf("Usage: %s <input file 1> <input file 2> <output file>\n", argv[0]);
		return 1;
	}

	ffmpeg_init();

	if ((ret = open_input_file(argv[1])) < 0) {
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

	struct SwsContext* swsContext = NULL;
	struct SwsContext* gl_swsContext = NULL;

	swsContext = sws_getContext(
		src_w, src_h, src_pix_fmt,
		src_w, src_h, dst_pix_fmt, SWS_BILINEAR, NULL, NULL, NULL);
	if (!swsContext) {
		printf("sws_getCachedContext failed!\n");
		return NULL;
	}

	if ((ret = av_image_alloc(dst_data, dst_linesize,
		src_w, src_h, dst_pix_fmt, 16)) < 0) {
		fprintf(stderr, "Could not allocate destination image\n");
	}

	// SET FRAME BUFFER SIZE
	int width = 800;
	int height = 600;
	GLFWwindow *window = opengl_window_init(width, height);

	opengl_init(width, height);

	GLuint VAO, VBO, EBO;
	load_VBO_VAO_EBO(VBO, VAO, EBO);
	GLuint shaderProgram;
	shaderProgram = loadShaders("vertexshader.glsl", "fragmentshader.glsl");

	glUseProgram(shaderProgram);
	GLuint texture1 = loadImgTexture("baby.jpg");
	glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
	GLuint texture2;

	GLubyte *pixelBuffer = createPixelBuffer(width, height);
	gl_swsContext = sws_getContext(
		width, height, gl_pix_fmt,
		src_w, src_h, src_pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);
	if (!gl_swsContext) {
		printf("sws_getCachedContext failed!\n");
		return NULL;
	}

	if ((ret = av_image_fill_arrays(gl_frame->data, gl_frame->linesize, pixelBuffer,
		gl_pix_fmt, width, height,  16)) < 0) {
		fprintf(stderr, "Could not allocate destination image\n");
	}
	gl_frame->width = width;
	gl_frame->height = height;
	gl_frame->format = gl_pix_fmt;


	while (!glfwWindowShouldClose(window))
	{
		printf("loop");

		glfwGetFramebufferSize(window, &width, &height);


		
		if (!read_video_package(&packet))
		{
			break;
		}
		if (get_frame_from_package(&packet, frame, &got_frame)) {
			if (got_frame && (packet.stream_index == 0)) {
				progress += 0.01;
				//rgb_frame = transfer_frame(&packet, frame, swsContext);

				/* convert to destination format */
				sws_scale(swsContext, frame->data,
					frame->linesize, 0, src_h, dst_data, dst_linesize);

				glUseProgram(shaderProgram);
				texture2 = loadFrameTexture((GLchar *)dst_data[0], frame->width, frame->height);
				glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);
				glUniform1f(glGetUniformLocation(shaderProgram, "progress"), progress);

				if (ret < 0) goto end;
				
				// °ó¶¨ÎÆÀí
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture1);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, texture2);
				glBindVertexArray(VAO);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

				// STEP 9: GLFW SWAP BUFFER
				glfwSwapBuffers(window);
				// STEP 10: EVENT 
				glfwPollEvents();

				//av_freep(&rgb_frame->data[0]);
				//av_frame_free(&rgb_frame);
				snap_shot(width, height, (GLubyte *)gl_frame->data[0]);
				//ret = filter_encode_write_frame(frame, packet.stream_index);
				//av_frame_free(&frame);
				AVFrame *filt_frame;
				filt_frame = av_frame_alloc();
				if ((ret = av_image_alloc(filt_frame->data, filt_frame->linesize,
					src_w, src_h, src_pix_fmt, 16)) < 0) {
					fprintf(stderr, "Could not allocate destination image\n");
				}
				filt_frame->format = frame->format;
				filt_frame->width = frame->width;
				filt_frame->height = frame->height;
				filt_frame->channels = frame->channels;
				filt_frame->channel_layout = frame->channel_layout;
				filt_frame->nb_samples = frame->nb_samples;
				filt_frame->pts = frame->pts;

				/* convert to destination format */
				ret = sws_scale(gl_swsContext, gl_frame->data,
					gl_frame->linesize, 0, height, filt_frame->data, filt_frame->linesize);
				ret = filter_encode_write_frame(filt_frame, packet.stream_index);
				//av_frame_free(&frame);
				av_frame_free(&filt_frame);
				av_frame_unref(frame);
	
			}
		}
		else {
			goto end;
		}
		av_packet_unref(&packet);


	}

	freePixelBuffer(pixelBuffer);
	close_opengl_window(window);

	write_file_tail();
end:
	sws_freeContext(swsContext);
	sws_freeContext(gl_swsContext);
	av_frame_free(&frame);
	close_filters();
	close_input_file();
	close_output_file();
}
