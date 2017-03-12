#version 400

precision highp float;

layout (location = 0) in vec3 vp; // vector made from 3 floats
layout (location = 1) in vec3 inNorm;
layout (location = 2) in vec3 intexcoord;

out vec4 normal;
out vec2 TexCoord;

uniform mat4 mvpmatrix;

void main() {
    gl_Position = mvpmatrix * vec4(vp, 1.0);
    normal = mvpmatrix * vec4(inNorm, 0.0);
    TexCoord = vec2(intexcoord.x, 1 - intexcoord.y); // flip picture by doing 1-
}
