#version 150

in  vec4  interpolatedColor;
in  vec2 texCoord;
in float zValue;
in vec3 R;
in vec3 T;

out vec4 fColor;

uniform  int textureMode;
uniform  int enableFogWithSpecialIncrement;
uniform sampler2D texture;
uniform samplerCube texMap;

void main() 
{
	float factForFog = 1.0f;
	vec4 fogColor = vec4(1.0, 1.0, 1.0, 1.0);

	// for reflection use
	//vec4 texColor = textureCube(texMap, R);


	if(textureMode == 1)
	{
		fColor = interpolatedColor;
	}
	else if(textureMode == 2)
	{
		fColor = textureCube(texMap, R);
	}
	else if(textureMode == 3)
	{
		vec4 refractColor = textureCube(texMap, T); // look up texture map using T
		fColor = mix(refractColor, vec4(1.0, 1.0, 1.0, 1.0), 0.3); 
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