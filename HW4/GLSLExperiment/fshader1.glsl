#version 150

in  vec4  interpolatedColor;
in  vec2 texCoord;
out vec4 fColor;

uniform  int enableTreeColor;
uniform sampler2D texture;

void main() 
{ 
	if(enableTreeColor == 1)
	{
		fColor = interpolatedColor;
	}
	else
	{
		fColor = texture2D( texture, texCoord );
	}
	//clamp()
	//exp
	//mix(fogcolor, .xyz, parameter)
}