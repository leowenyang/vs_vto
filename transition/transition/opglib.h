#ifndef OPGLIB_H
#define OPGLIB_H

// GLEW
#define GLEW_STATIC //  π”√ æ≤Ã¨ø‚ glew32s.lib
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <gl/GL.h>

// Data

// Function
GLubyte *createPixelBuffer(GLuint w, GLuint h);
GLvoid freePixelBuffer(GLubyte* buffer);

GLuint loadImgTexture(const GLchar *filename);
GLuint loadFrameTexture(const GLchar *filename, GLuint width, GLuint height);
GLuint loadShaders(const GLchar *vertex_file_path, const GLchar *fragment_file_path);
GLvoid snap_shot(GLuint w, GLuint h, GLubyte *buffer);

GLFWwindow *opengl_window_init(GLuint w, GLuint h);
GLvoid close_opengl_window(GLFWwindow *window);

GLvoid opengl_init(GLuint w, GLuint h);
GLvoid load_VBO_VAO_EBO(GLuint &VBO, GLuint &VAO, GLuint &EBO);
#endif
