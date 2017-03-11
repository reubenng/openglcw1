#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

char* filetobuf(char *file);

void check(char *where);

GLuint loadpicture(const char* path);