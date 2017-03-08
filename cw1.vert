#version 400

precision highp float;

in  vec3 vp; // vector made from 3 floats
uniform mat4 mvpmatrix;

void main() {
    gl_Position = mvpmatrix * vec4(vp, 1.0);
}
