//============================================================================
// Name        : cw1.cpp
// Author      : rdcn1g14
// Description : COMP3214 Coursework 1
/*
A. Draw a wire - frame sphere by calculating all the vertex positions and drawing lines between them.
B. For the sphere, work out the surface normal direction 1 and augment your wire - frame drawing with short lines representing the normal direction of each vertex The sphere should now appear to be a hedgehog .
C. Use the normal information calculated in (b ) above work out the amount of illumination falling on each vertex. Assume that the light source is at infinity and is co-axial with the viewpoint. Use this to draw a shaded sphere.
D. Develop a simple animation showing a number of cones and spheres moving along regular path s. These can be wireframe or solid.
E. Draw a textured object, such as a rectangle (plane) , box or sphere .
*/
//============================================================================

// #define STB_IMAGE_IMPLEMENTATION
// #define STBI_ASSERT(x)
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utils.h"
// #include "stb_image.h"
#include "spherecone.h"
#include <vector>
#include <iostream>
using namespace std;

GLuint spherevbo, spherevao, sphereebo, normalvao, normalvbo;
GLuint fragmentshader, vertexshader, shaderprogramme;
GLuint conevbo, conevao; // for cones
vector<GLfloat> spherevert, norm, conevert; // for storing sphere vertices and normal
vector<GLint> ind; // drawing indices
GLint rings = 25; // number of rings and segments that make sphere
GLint segments = 50;
int screen = 1; // starts with screen 1 wireframe sphere

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
	window = glfwCreateWindow(640, 480, "COMP3214 - Coursework 1 - Reuben Ng (rdcn1g14)", NULL, NULL);
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
	// GLuint texture;
	// int w, h, comp; // for texture loading
	// loadpicture(&texture, "./abeir_toril.bmp", &w, &h, &comp);
	loadpicture("./abeir_toril.bmp");
	setupshaders();
	setupgeometry();
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
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

void setupshaders(){
	int length;
	char text[512];
	GLint success;
	// vertex display
	const char* vertex_shader = filetobuf((char*)"./main.vert");
	vertexshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexshader, 1, &vertex_shader, NULL);
	glCompileShader(vertexshader);
	glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &success);
	glGetShaderInfoLog(vertexshader, 512, NULL, text);
	if(!success) cout << "Validate vertexshader " << text << endl;

	// colour surface
	const char* fragment_shader = filetobuf((char*)"./main.frag");
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
	sphere(&spherevert, &ind, &norm, rings, segments);
	cone(&conevert, rings, segments);
	
	glGenVertexArrays(1, &spherevao); // generate vao
	glGenBuffers(1, &spherevbo); // generate vbo
	glGenBuffers(1, &sphereebo); // index for drawing sphere

	glGenVertexArrays(1, &normalvao); // generate vao
	glGenBuffers(1, &normalvbo); // generate vbo for normal vertices

	glBindVertexArray(spherevao); // set to current vao
	glBindBuffer(GL_ARRAY_BUFFER, spherevbo); // set to current vbo
	glBufferData(GL_ARRAY_BUFFER, spherevert.size() * sizeof(GLfloat), &spherevert[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereebo); // for drawing sequence
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * sizeof(GLint), &ind[0], GL_STATIC_DRAW);
	
	// attribute 0, [vertex, vertex, vertex, normal, normal, normal, texcoord, texcoord]
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(0); // attribute 0
	// attribute 1 for normal, this normal is for calculating shading
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1); // attribute 1
	// attribute 2 for texture, every 2 points
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2); // attribute 2

	// a new vao created just for drawing normal lines, not used for shading
	glBindVertexArray(normalvao); // set to normal vao
	glBindBuffer(GL_ARRAY_BUFFER, normalvbo); // set to normal vbo
	glBufferData(GL_ARRAY_BUFFER, norm.size() * sizeof(GLfloat), &norm[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(0);

	glGenVertexArrays(1, &conevao); // generate vao for cones
	glGenBuffers(1, &conevbo); // generate vbo
	glBindVertexArray(conevao); // set to current vao
	glBindBuffer(GL_ARRAY_BUFFER, conevbo); // set to current vbo
	glBufferData(GL_ARRAY_BUFFER, conevert.size() * sizeof(GLfloat), &conevert[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(0); // attribute 0

	glBindVertexArray(0); // unbind
  	check((char*)"setupgeometry");
	}

void render(){
	// animation (rotation and movements)
	GLfloat angle;
    glm::mat4 Projection = glm::perspective(45.0f, 1.0f, 0.1f, 100.0f);
    float p = 400.;
    float t = fmod(glfwGetTime(), p); // use time for animation
    angle = t * 360. / p; // time to produce angle
    glm::mat4 View = glm::translate(glm::mat4(1.), glm::vec3(0.f, 0.f, -5.0f));
    View = glm::rotate(View, angle * 0.5f, glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 Model = glm::mat4(1.0);
    glm::mat4 MVP = Projection * View * Model;
    glUniformMatrix4fv(glGetUniformLocation(shaderprogramme, "mvpmatrix"), 1, GL_FALSE, glm::value_ptr(MVP));
    /* Bind modelmatrix variable to be a uniform called mvpmatrix in our shaderprogram */

    glUniform3fv(glGetUniformLocation(shaderprogramme, "colour"), 1, glm::value_ptr(glm::vec3(1.0, 0.5, 0.2)));
    glUniform1i(glGetUniformLocation(shaderprogramme, "texturemode"), 0); // disable texture
    glUniform1i(glGetUniformLocation(shaderprogramme, "shade"), 0); // disable shading
	check((char*)"Render");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear screen
	if(screen == 1){ // wireframe sphere
		glBindVertexArray(spherevao); // set vao as input for drawing
		glDrawElements(GL_LINE_STRIP, ind.size(), GL_UNSIGNED_INT, 0);
	}else if(screen == 2){ // wireframe with normals
		glBindVertexArray(spherevao); // set sphere vao as input for drawing
		glDrawElements(GL_LINE_STRIP, ind.size(), GL_UNSIGNED_INT, 0); // draw sphere first
		glBindVertexArray(normalvao); // set normal vao as input for drawing
		glDrawArrays(GL_LINES, 0, norm.size()); // then draw normals
	}else if(screen == 3){ // shaded sphere
        glUniform1i(glGetUniformLocation(shaderprogramme, "shade"), 1); // enable shade
		glBindVertexArray(spherevao); // set vao as input for drawing
		glDrawElements(GL_TRIANGLES, ind.size(), GL_UNSIGNED_INT, 0);
	}else if(screen == 4){ // animation
		// big sphere
	    glBindVertexArray(spherevao); // set sphere vao as input for drawing
		MVP = glm::scale(glm::mat4(1.), glm::vec3(0.25));
	    MVP = glm::translate(MVP, glm::vec3(sin(angle), cos(angle), 0.0));
	    glUniformMatrix4fv(glGetUniformLocation(shaderprogramme, "mvpmatrix"), 1, GL_FALSE, glm::value_ptr(MVP));
		glDrawElements(GL_LINE_STRIP, ind.size(), GL_UNSIGNED_INT, 0);
		// small sphere
	    MVP = glm::scale(glm::mat4(1.), glm::vec3(0.75));
	    MVP = glm::translate(MVP, glm::vec3(sin(-t), cos(-t), 0.0));
	    MVP = glm::scale(MVP, glm::vec3(0.1));
	    glUniformMatrix4fv(glGetUniformLocation(shaderprogramme, "mvpmatrix"), 1, GL_FALSE, glm::value_ptr(MVP));
		glDrawElements(GL_LINE_STRIP, ind.size(), GL_UNSIGNED_INT, 0);
		glUniform3fv(glGetUniformLocation(shaderprogramme, "colour"), 1, glm::value_ptr(glm::vec3(1.0, 0.0, 0.0)));
		// big cone
	    glBindVertexArray(conevao); // set cones vao as input for drawing
		MVP = Projection * View * Model;
	    MVP = glm::scale(MVP, glm::vec3(0.9));
	    MVP = glm::translate(MVP, glm::vec3(sin(t), cos(t), 0.0));
	    MVP = glm::scale(MVP, glm::vec3(0.1));
	    glUniformMatrix4fv(glGetUniformLocation(shaderprogramme, "mvpmatrix"), 1, GL_FALSE, glm::value_ptr(MVP));
		glDrawArrays(GL_LINE_LOOP, 0, ind.size());
		// small cone
	    MVP = Projection * View * Model;
	    MVP = glm::scale(MVP, glm::vec3(0.8));
	    MVP = glm::translate(MVP, glm::vec3(sin(-angle), cos(-angle), 0.0));
	    MVP = glm::scale(MVP, glm::vec3(0.1));
	    glUniformMatrix4fv(glGetUniformLocation(shaderprogramme, "mvpmatrix"), 1, GL_FALSE, glm::value_ptr(MVP));
		glDrawArrays(GL_LINE_LOOP, 0, ind.size());
	}else if(screen == 5){ // textured sphere
        glUniform1i(glGetUniformLocation(shaderprogramme, "texturemode"), 1); // enable texture
		glBindVertexArray(spherevao); // set vao as input for drawing
		glDrawElements(GL_TRIANGLES, ind.size(), GL_UNSIGNED_INT, 0);
	}
	glFlush();
	glBindVertexArray(0); // unbind
}

static void keypress(GLFWwindow* window, int key, int scancode, int action, int mods){
	if((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
	else if(key == GLFW_KEY_A && action == GLFW_PRESS) screen = 1; // sphere
	else if(key == GLFW_KEY_B && action == GLFW_PRESS) screen = 2; // sphere with normal
	else if(key == GLFW_KEY_C && action == GLFW_PRESS) screen = 3; // shaded sphere
	else if(key == GLFW_KEY_D && action == GLFW_PRESS) screen = 4; // spheres and cones animation
	else if(key == GLFW_KEY_E && action == GLFW_PRESS) screen = 5; // sphere with texture
}