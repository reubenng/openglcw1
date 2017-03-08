//============================================================================
// Name        : cw1.cpp
// Author      : rdcn1g14
// Version     :
// Copyright   : 
// Description : COMP3214 Coursework 1
/*
A. Draw a wire - frame sphere by calculating all the vertex positions and drawing lines between them.
B. For the sphere, work out the surface normal direction 1 and augment your wire - frame drawing with short lines representing the normal direction of each vertex The sphere should now appear to be a hedgehog .
C. Use the nor mal information calculated in (b ) above work out the amount of illumination falling on each vertex. Assume that the light source is at infinity and is co - axial with the viewpoint. Use this to draw a shaded sphere.
D. Develop a simple animation showing a number of cones and spheres moving along regular path s. These can be wireframe or solid.
E. Draw a textured object, suc h as a rec tangle (plane) , box or sphere .
*/
//============================================================================

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utils.h"
#include "sphere.h"
#include <vector>
#include <iostream>
using namespace std;

GLuint vbo, vao, fragmentshader, vertexshader, shaderprogramme;
vector<GLfloat> spherevertices; // for storing sphere vertices
vector<GLint> sphereindices;
GLint rings = 20; // number of rings and segments that make sphere
GLint segments = 20;

/*float points[] = {
	0.0f,  0.5f,  0.0f,
	0.5f, -0.5f,  -0.5f,
	-0.5f, -0.5f,  -0.5f,
	0.0f,  0.5f,  0.0f,
	0.0f, -0.5f,  0.5f,
	0.5f, -0.5f,  -0.5f,
	0.0f,  0.5f,  0.0f,
	-0.5f, -0.5f,  -0.5f,
	0.0f, -0.5f, 0.5f,
	0.5f, -0.5f,  -0.5f,
	-0.5f, -0.5f,  -0.5f,
	0.0f, -0.5f,  0.5f

	0.0f,  0.5f,  0.0f,
   0.5f, -0.5f,  0.0f,
  -0.5f, -0.5f,  0.0f
};*/

void setupshaders();
void setupgeometry();
void render(int k);
static void keypress(GLFWwindow* window, int key, int scancode, int action, int mods);

int main(){
	if(!glfwInit()){ // initialise GLFW
		cout << "Fail to start GLFW" << endl;
		exit(EXIT_FAILURE);	
	}

	GLFWwindow* window; // opengl window
	window = glfwCreateWindow(640, 480, "COMP3214 - Coursework 1", NULL, NULL);
	if(!window){
		cout << "Fail to open window" << endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window); // make the window current context
	
	glewExperimental = GL_TRUE;
	int err = glewInit();
	if(GLEW_OK != err) cout << "Fail to start GLEW: " << err << endl;
	glfwSetKeyCallback(window, keypress);

	 // tell GL to only draw onto a pixel if the shape is closer to the viewer
	// glEnable(GL_DEPTH_TEST); // enable depth-testing
	// glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

	setupshaders();
	setupgeometry();

	// glClearColor(1., 1., 1., 1.); // colour white background
	int k = 0;
	while(!glfwWindowShouldClose(window)){
		check((char *)"Window Loop");
		render(k);	
		//cout << k << endl;
		k++;
		glfwSwapBuffers(window); // display to window
		glfwPollEvents(); // update non-graphic input
	}

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_FAILURE);
}

void setupshaders(){
  	check((char*)"setupshaders");
	int length;
	char text[1000];
	// colour surface
	const char* fragment_shader = filetobuf((char*)"./cw1.frag");
	fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentshader, 1, &fragment_shader, NULL);
	glCompileShader(fragmentshader);

	// vertex display
	const char* vertex_shader = filetobuf((char*)"./cw1.vert");
	vertexshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexshader, 1, &vertex_shader, NULL);
	glCompileShader(vertexshader);

	shaderprogramme = glCreateProgram(); // create empty program
	glAttachShader(shaderprogramme, fragmentshader); // attach both
	glAttachShader(shaderprogramme, vertexshader);
	glLinkProgram(shaderprogramme); // link together

	glGetProgramInfoLog(shaderprogramme, 1000, &length, text);
	if(length > 0) cout << "Validate Shader Program " << text << endl;
  	glUseProgram(shaderprogramme); // use program
}

void setupgeometry(){
  	check((char*)"setupgeometry");
	sphere(&spherevertices, rings, segments);
	
	glGenVertexArrays(1, &vao); // generate vao
	glBindVertexArray(vao); // set to current vao

	glGenBuffers(1, &vbo); // generate vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // set to current vbo

	glEnableVertexAttribArray(0); // attribute 0
	glBufferData(GL_ARRAY_BUFFER, spherevertices.size() * sizeof(GLfloat), &spherevertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// attribute 0, vec3 from every 3 GL_FLOAT in buffer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}

void render(int k){
	GLfloat angle;
    glm::mat4 Projection = glm::perspective(45.0f, 1.0f, 0.1f, 100.0f);
    float t = glfwGetTime();
    float p = 400.;
    t = fmod(t, p);
    angle = t * 360. / p;
    glm::mat4 View = glm::mat4(1.);
    View = glm::translate(View, glm::vec3(0.f, 0.f, -5.0f));
    View = glm::rotate(View, angle * -1.0f, glm::vec3(1.f, 0.f, 0.f));
    View = glm::rotate(View, angle * 0.5f, glm::vec3(0.f, 1.f, 0.f));
    View = glm::rotate(View, angle * 0.5f, glm::vec3(0.f, 0.f, 1.f));
    glm::mat4 Model = glm::mat4(1.0);
    glm::mat4 MVP = Projection * View * Model;
    glUniformMatrix4fv(glGetUniformLocation(shaderprogramme, "mvpmatrix"), 1, GL_FALSE, glm::value_ptr(MVP));
    /* Bind our modelmatrix variable to be a uniform called mvpmatrix in our shaderprogram */
    
	check((char*)"Render");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear screen
	glBindVertexArray(vao); // set vao as input for drawing
	glDrawArrays(GL_LINE_LOOP, 0, spherevertices.size()); // draw from point 0, for 3 points
	// glDrawArrays(GL_TRIANGLES, 0, spherevertices.size()); // draw from point 0, for 3 points
	glFlush();
}

static void keypress(GLFWwindow* window, int key, int scancode, int action, int mods){
	if((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE); // if q pressed, close window
}