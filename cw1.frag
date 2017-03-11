#version 400

precision highp float;

in vec4 normal;
in vec2 TexCoord;

out vec4 frag_colour;

uniform vec3 colour;
uniform sampler2D mytexture;
uniform int texturemode;
uniform int shade;

void main() {
	if(texturemode == 1){
		frag_colour = texture(mytexture, TexCoord);
	}else if(shade == 1){
   	 	vec4 originalcolour = vec4(colour, 0.0); // red green blue alpha
   	 	vec4 lightsource = vec4(0.0, 0.0, -1.0, 0.0); // co-axial with viewpoint
   	 	
   	 	vec4 N = normalize(normal);
   	 	vec4 AmbientColor = vec4(0.1) * originalcolour; // dark colour
      vec4 DiffuseColor = originalcolour * clamp(dot(N, lightsource), 0, 1);
   	 	frag_colour = AmbientColor + DiffuseColor;

  }else{
    frag_colour = vec4(colour, 1.0f);
  }
}