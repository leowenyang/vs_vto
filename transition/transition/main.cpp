#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "ffmlib.h"
#include "opglib.h"

int main(int argc, char **argv)
{
	int ret;
	int got_frame = 0;
	AVPacket packet;
	packet.data = NULL;
	packet.size = 0;
	AVFrame *frame = av_frame_alloc();
	AVFrame *rgb_frame = NULL;

	uint8_t *src_data[4], *dst_data[4];
	int src_linesize[4], dst_linesize[4];
	int src_w = 720, src_h = 404, dst_w, dst_h;
	enum AVPixelFormat src_pix_fmt = AV_PIX_FMT_YUV420P, dst_pix_fmt = AV_PIX_FMT_RGB24;

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

	swsContext = sws_getContext(
		src_w, src_h, src_pix_fmt,
		src_w, src_h, dst_pix_fmt, SWS_BILINEAR, NULL, NULL, NULL);
	if (!swsContext) {
		printf("sws_getCachedContext failed!\n");
		return NULL;
	}

	if ((ret = av_image_alloc(dst_data, dst_linesize,
		src_w, src_h, dst_pix_fmt, 1)) < 0) {
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
				//rgb_frame = transfer_frame(&packet, frame, swsContext);

				/* convert to destination format */
				sws_scale(swsContext, frame->data,
					frame->linesize, 0, src_h, dst_data, dst_linesize);

				glUseProgram(shaderProgram);
				texture2 = loadFrameTexture((GLchar *)dst_data[0], frame->width, frame->height);
				glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);
				//ret = filter_encode_write_frame(frame, packet.stream_index);
				//av_frame_free(&frame);
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
			}
		}
		else {
			goto end;
		}
		av_packet_unref(&packet);


	}

	close_opengl_window(window);

	write_file_tail();
end:
	sws_freeContext(swsContext);
	av_frame_free(&frame);
	close_filters();
	close_input_file();
	close_output_file();
}
