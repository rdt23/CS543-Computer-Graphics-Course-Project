#version 150

in  vec2 texCoord;
out vec4 fColor;

uniform sampler2D texture;
uniform  int effectMode;

vec4 router( vec3 , int );
vec4 LuminanceEffect( vec3 );
vec4 NegativeEffect( vec3 );
vec4 EdgeDetectionEffect( vec3 );

void main() 
{ 
	vec3 color = vec3(texture( texture, texCoord ));
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

vec4 EdgeDetectionEffect(vec3 color)
{
	const vec3 LUMCOEFFS = vec3(0.2125, 0.7154, 0.0721);

	ivec2 ires = textureSize( texture, 0);
	float ResS = float( ires.s );
	float ResT = float( ires.t );
	vec3 irgb = texture(texture, texCoord ).rgb;

	vec2 stp0 = vec2(1.0/ResS,  0.0 ); //texel offsets
	vec2 st0p = vec2(0.0 ,      1.0/ResT);
	vec2 stpp = vec2(1.0/ResS,  1.0/ResT);
	vec2 stpm = vec2(1.0/ResS, -1.0/ResT);

	// 3x3 pixel colors next
	vec3 i00   = texture( texture, texCoord ).rgb;
	vec3 im1m1 = texture( texture, texCoord-stpp ).rgb;
	vec3 ip1p1 = texture( texture, texCoord+stpp ).rgb;
	vec3 im1p1 = texture( texture, texCoord-stpm ).rgb;
	vec3 ip1m1 = texture( texture, texCoord+stpm ).rgb;
	vec3 im10  = texture( texture, texCoord-stp0 ).rgb;
	vec3 ip10  = texture( texture, texCoord+stp0 ).rgb;
	vec3 i0m1  = texture( texture, texCoord-st0p ).rgb;
	vec3 i0p1  = texture( texture, texCoord+st0p ).rgb;

	return vec4( 1.0 - color.x, 1.0 - color.y, 1.0 - color.z, 1.0);
}

vec4 NegativeEffect(vec3 color)
{
	return vec4( 1.0 - color.x, 1.0 - color.y, 1.0 - color.z, 1.0);
}

vec4 LuminanceEffect(vec3 color)
{
	const vec3 W = vec3(0.2125, 0.7154, 0.0721);
	float luminance = dot(color, W);
	return vec4( luminance, luminance,luminance, 1.0);
}





/*
Embossing effect
ToonRendering
TwirlTransformation
RippleTransformation
SphericalTransformation
*/