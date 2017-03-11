//============================================================================
// Name        : cw1.cpp
// Author      : rdcn1g14
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

GLuint spherevbo, spherevao, sphereebo, normalvao, normalvbo, fragmentshader, vertexshader, shaderprogramme;
vector<GLfloat> spherevertices, norm; // for storing sphere vertices and normal
vector<GLint> ind;
GLint rings = 20; // number of rings and segments that make sphere
GLint segments = 40;
int screen = 1;

void setupshaders();
void setupgeometry();
void render();
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
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	
	// Toril world map from http://img12.deviantart.net/3948/i/2008/110/9/7/abeir_toril_by_ikaazu.jpg
	loadpicture("./abeir_toril.bmp");
	setupshaders();
	setupgeometry();
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// glClearColor(1., 1., 1., 1.); // colour white background
	
	while(!glfwWindowShouldClose(window)){
		check((char *)"Window Loop");
		render();	
		glfwSwapBuffers(window); // display to window
		glfwPollEvents(); // update non-graphic input
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_FAILURE);
}

void setupshaders(){int length;
	char text[512];
	GLint success;
	// vertex display
	const char* vertex_shader = filetobuf((char*)"./cw1.vert");
	vertexshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexshader, 1, &vertex_shader, NULL);
	glCompileShader(vertexshader);
	glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &success);
	glGetShaderInfoLog(vertexshader, 512, NULL, text);
	if(!success) cout << "Validate vertexshader " << text << endl;

	// colour surface
	const char* fragment_shader = filetobuf((char*)"./cw1.frag");
	fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentshader, 1, &fragment_shader, NULL);
	glCompileShader(fragmentshader);
	glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &success);
	glGetShaderInfoLog(fragmentshader, 512, NULL, text);
	if(!success) cout << "Validate fragmentshader " << text << endl;

	shaderprogramme = glCreateProgram(); // create empty program
	glAttachShader(shaderprogramme, fragmentshader); // attach both
	glAttachShader(shaderprogramme, vertexshader);
	glLinkProgram(shaderprogramme); // link together

	glGetProgramInfoLog(shaderprogramme, 512, &length, text);
	if(length > 0) cout << "Validate shaderprogramme " << text << endl;
  	glUseProgram(shaderprogramme); // use program
	glDeleteShader(vertexshader); // delete linked shader objects
	glDeleteShader(fragmentshader); 

  	check((char*)"setupshaders");
	}

void setupgeometry(){
	sphere(&spherevertices, &ind, &norm, rings, segments);
	
	glGenVertexArrays(1, &spherevao); // generate vao
	glGenBuffers(1, &spherevbo); // generate vbo
	glGenBuffers(1, &sphereebo); // index for drawing sphere

	glGenVertexArrays(1, &normalvao); // generate vao
	glGenBuffers(1, &normalvbo); // generate vbo for normal vertices

	glBindVertexArray(spherevao); // set to current vao
	glBindBuffer(GL_ARRAY_BUFFER, spherevbo); // set to current vbo
	glBufferData(GL_ARRAY_BUFFER, spherevertices.size() * sizeof(GLfloat), &spherevertices[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * sizeof(GLint), &ind[0], GL_STATIC_DRAW); // for drawing sequence
	
	// attribute 0, [vertex, vertex, vertex, texcoord, texcoord]
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(0); // attribute 0
	// attribute 1 for normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1); // attribute 1
	// attribute 2 for texture, every 2 points
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2); // attribute 2

	glBindVertexArray(normalvao); // set to normal vao
	glBindBuffer(GL_ARRAY_BUFFER, normalvbo); // set to normal vbo
	glBufferData(GL_ARRAY_BUFFER, norm.size() * sizeof(GLfloat), &norm[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(0); // attribute 1

	glBindVertexArray(0); // unbind
  	check((char*)"setupgeometry");
	}

void render(){
	GLfloat angle;
    glm::mat4 Projection = glm::perspective(45.0f, 1.0f, 0.1f, 100.0f);
    float t = glfwGetTime();
    float p = 400.;
    t = fmod(t, p);
    angle = t * 360. / p;
    glm::mat4 View = glm::mat4(1.);
    View = glm::translate(View, glm::vec3(0.f, 0.f, -5.0f));
    View = glm::rotate(View, angle * 0.5f, glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 Model = glm::mat4(1.0);
    glm::mat4 MVP = Projection * View * Model;
    glUniformMatrix4fv(glGetUniformLocation(shaderprogramme, "mvpmatrix"), 1, GL_FALSE, glm::value_ptr(MVP));
    /* Bind our modelmatrix variable to be a uniform called mvpmatrix in our shaderprogram */

    glUniform3fv(glGetUniformLocation(shaderprogramme, "colour"), 1, glm::value_ptr(glm::vec3(1.0, 0.5, 0.2)));
    glUniform1i(glGetUniformLocation(shaderprogramme, "texturemode"), 0); // off texture
    glUniform1i(glGetUniformLocation(shaderprogramme, "shade"), 0);
	check((char*)"Render");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear screen
	if(screen == 1){
		glBindVertexArray(spherevao); // set vao as input for drawing
		glDrawElements(GL_LINE_STRIP, ind.size(), GL_UNSIGNED_INT, 0);
	}else if(screen == 2){
		glBindVertexArray(spherevao); // set vao as input for drawing
		glDrawElements(GL_LINE_STRIP, ind.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(normalvao); // set vao as input for drawing
		glDrawArrays(GL_LINES, 0, norm.size()); // draw from point 0, for 3 points
	}else if(screen == 3){
        glUniform1i(glGetUniformLocation(shaderprogramme, "shade"), 1);
		glBindVertexArray(spherevao); // set vao as input for drawing
		glDrawElements(GL_TRIANGLES, ind.size(), GL_UNSIGNED_INT, 0); // draw from point 0, for 3 points
	}else if(screen == 4){
		glBindVertexArray(spherevao); // set vao as input for drawing
		glDrawElements(GL_LINE_STRIP, ind.size(), GL_UNSIGNED_INT, 0);
		// glDrawArrays(GL_LINE_STRIP, 0, spherevertices.size()); // draw from point 0, for 3 points
	}else if(screen == 5){
        glUniform1i(glGetUniformLocation(shaderprogramme, "texturemode"), 1); // on texture
		glBindVertexArray(spherevao); // set vao as input for drawing
		glDrawElements(GL_TRIANGLES, ind.size(), GL_UNSIGNED_INT, 0);
	}
	glFlush();
	glBindVertexArray(0); // unbind
}

static void keypress(GLFWwindow* window, int key, int scancode, int action, int mods){
	if((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE); // if q pressed, close window
	else if(key == GLFW_KEY_A) screen = 1; // sphere
	else if(key == GLFW_KEY_B) screen = 2; // sphere with normal
	else if(key == GLFW_KEY_C) screen = 3; // shaded sphere
	else if(key == GLFW_KEY_D) screen = 4; // spheres and cones animation
	else if(key == GLFW_KEY_E) screen = 5; // sphere with texture
	else screen = 1; // sphere
}