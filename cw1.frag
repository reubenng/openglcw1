#version 400

precision highp float;

in vec4 normal;
in vec2 TexCoord;

out vec4 frag_colour;

uniform sampler2D mytexture;
uniform int texture;

void main() {
	if(texture == 1){
		frag_colour = texture(mytexture, TexCoord);
	}else{
   	 	frag_colour = vec4(1.0, 0.5, 0.2, 1.0); // red green blue alpha
    }
}
