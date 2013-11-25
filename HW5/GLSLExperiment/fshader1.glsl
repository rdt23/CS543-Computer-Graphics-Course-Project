#version 150

in  vec2 texCoord;
out vec4 fColor;

uniform sampler2D texture;
uniform  int effectMode;

vec4 router( vec3 , int );
vec4 LuminanceEffect( vec3 );
vec4 NegativeEffect( vec3 );

void main() 
{ 
	vec3 color = vec3(texture2D( texture, texCoord ));
    fColor = router(color, effectMode);

}

vec4 router(vec3 color, int effectMode)
{
	switch(effectMode)
	{
		case 0:
			return vec4(color, 1.0);
		case 1:
			return LuminanceEffect(color);
		
		case 2:
			return NegativeEffect(color);

		default:
			return vec4(color, 1.0);
	}
}

vec4 LuminanceEffect(vec3 color)
{
	const vec3 W = vec3(0.2125, 0.7154, 0.0721);
	float luminance = dot(color, W);
	return vec4( luminance, luminance,luminance, 1.0);
}

vec4 NegativeEffect(vec3 color)
{
	return vec4( 1.0 - color.x, 1.0 - color.y, 1.0 - color.z, 1.0);
}