#version 150

in  vec4  interpolatedColor;
in  vec2 texCoord;
in float zValue;

out vec4 fColor;

uniform  int enableTreeColor;
uniform  int enableFogWithSpecialIncrement;
uniform sampler2D texture;

void main() 
{
	float factForFog = 1.0f;
	vec4 fogColor = vec4(1.0, 1.0, 1.0, 1.0);

	if(enableTreeColor == 1)
	{
		fColor = interpolatedColor;
	}
	else
	{
		fColor = texture2D( texture, texCoord );
	}

	switch(enableFogWithSpecialIncrement)
	{
		case 1:
			factForFog = (125-zValue) * 0.01;
			break;
		case 2:
			factForFog = exp(-zValue * 0.0125);
			break;
		default:
			factForFog = 1.0;
			break;

	}

	factForFog = clamp(factForFog, 0.0, 1.0);
	fColor = mix(fogColor, fColor, factForFog);

}