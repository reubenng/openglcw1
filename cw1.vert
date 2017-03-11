#version 400

precision highp float;

layout (location = 0) in vec3 vp; // vector made from 3 floats
layout (location = 1) in vec3 norm;
layout (location = 2) in vec3 texcoord;

out vec4 normal;
out vec2 TexCoord;

uniform mat4 mvpmatrix;

void main() {
    gl_Position = mvpmatrix * vec4(vp, 1.0);
    normal = mvpmatrix * vec4(norm, 0.0);
    TexCoord = vec2(texcoord.x, 1 - texcoord.y); // flip picture by doing 1-
}
