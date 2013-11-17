#version 150

in  vec4 vPosition;
in  vec4 vTexCoord;
out vec2 texCoord;
//out vec4 interpolatedColor;

uniform mat4 Projection;
uniform mat4 model_matrix;

void main() 
{
    // We only need the s and t values of the texture coordinate
    texCoord = vTexCoord.st;
    gl_Position = Projection * model_matrix * vPosition;
	//interpolatedColor = vColor;
}