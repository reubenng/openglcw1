// #define STB_IMAGE_IMPLEMENTATION
// #define STBI_ASSERT(x)
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
// #include "stb_image.h"
#include <iostream>
using namespace std;

char* filetobuf(char *file);

void check(char *where);

GLuint loadpicture(const char* path);
// GLuint loadpicture(GLuint * texture, const char* path, int* w, int* h, int* comp);