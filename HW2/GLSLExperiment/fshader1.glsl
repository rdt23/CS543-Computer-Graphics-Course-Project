#version 150

in  vec4  interpolatedColor;
out vec4  fColor;

void main() 
{ 
    fColor = interpolatedColor;
	//fColor = vec4( 0.0, 0.0, 1.0, 1.0 );
} 

