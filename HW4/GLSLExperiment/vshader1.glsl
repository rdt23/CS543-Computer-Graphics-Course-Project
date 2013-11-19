#version 150

in  vec4 vPosition;
in  vec2 vTexCoord;
in  vec4 vColor;
in  vec4 Normal;

out vec2 texCoord;
out vec4 interpolatedColor;
out float zValue;
out vec3 R;
out vec3 T;

uniform mat4 Projection;
uniform mat4 model_matrix;

void main() 
{
    // We only need the s and t values of the texture coordinate
    texCoord = vTexCoord.st;
    gl_Position = Projection * model_matrix * vPosition;
	interpolatedColor = vColor;
	zValue = gl_Position.z;

	
	vec3 eyePos = vPosition.xyz; 
	vec4 NN = model_matrix * Normal; 
	vec3 N = normalize(NN.xyz); 

	R = reflect(eyePos.xyz, N); 

	T = refract(eyePos.xyz, N, 1.47); 
}