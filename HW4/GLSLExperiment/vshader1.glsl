#version 150

in  vec4 vPosition;
in  vec2 vTexCoord;
in  vec4 vColor;

out vec2 texCoord;
out vec4 interpolatedColor;
out float zValue;

uniform mat4 Projection;
uniform mat4 model_matrix;

void main() 
{
    // We only need the s and t values of the texture coordinate
    texCoord = vTexCoord.st;
    gl_Position = Projection * model_matrix * vPosition;
	interpolatedColor = vColor;
	zValue = gl_Position.z;
}