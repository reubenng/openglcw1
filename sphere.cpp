#include "sphere.h"
#include "utils.h"

vector<GLfloat>* sphere (vector<GLfloat>* verts, GLint rings, GLint segments){ // UV sphere
	for(int i = 0; i < rings; i++){ // for every ring around the sphere
		// calculating theta
		float v = i / (float) rings;
		float t = (v * M_PI) - (M_PI / 2); // -90 < theta < 90
		
		for(int j = 0; j < segments; j++){ // each segment in a ring
			// calculating phi
			float u = j / (float) segments;
			float p = u * 2 * M_PI; // 0 < phi < 360

			// refered to sphere generation http://paulbourke.net/geometry/circlesphere/
			// equations of points on sphere surface
			float x = cosf(t) * cosf(p); // x = cos theta cos phi
			float y = sinf(t); // y = sin theta
			float z = cosf(t) * sinf(p); // z = cos theta sin phi

			glm::vec3 vertex(x, y, z); // sphere vertex
			// normalise points
			// add new points to the end of vector, 1 vert every 3 points
			verts->push_back(vertex.x);
			verts->push_back(vertex.y);
			verts->push_back(vertex.z);
		}
	}
	check((char*)"sphere");
	return verts;
}