#ifndef OPGLIB_H
#define OPGLIB_H

#ifdef OPGLLIB_C
#define X_OPGLIB
#else
#define X_OPGLIB extern
#endif

// GLEW
#define GLEW_STATIC //  π”√ æ≤Ã¨ø‚ glew32s.lib
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// GL
#include <gl/GL.h>

/***** Public constant definition *****/
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

/***** Public function prototype definition  ******/
X_OPGLIB GLubyte *createPixelBuffer(GLuint w, GLuint h);
X_OPGLIB GLvoid freePixelBuffer(GLubyte* buffer);

X_OPGLIB GLuint loadImgTexture(const GLchar *filename);
X_OPGLIB GLuint loadFrameTexture(GLchar *data, GLuint width, GLuint height);
X_OPGLIB GLuint loadShaders(const GLchar *vertex_file_path, const GLchar *fragment_file_path);
X_OPGLIB GLvoid snap_shot(GLuint w, GLuint h, GLubyte *buffer);

X_OPGLIB GLFWwindow *opengl_window_init(GLuint w, GLuint h);
X_OPGLIB GLvoid close_opengl_window(GLFWwindow *window);

X_OPGLIB GLvoid opengl_init(GLuint w, GLuint h);
X_OPGLIB GLvoid load_VBO_VAO_EBO(GLuint &VBO, GLuint &VAO, GLuint &EBO);
#endif
