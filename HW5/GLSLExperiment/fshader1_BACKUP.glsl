#version 150

in  vec2 texCoord;
out vec4 fColor;

uniform sampler2D texture;
uniform  int effectMode;

vec4 router( vec2 , int );
vec4 LuminanceEffect( vec2 );
vec4 NegativeEffect( vec2 );
vec4 EdgeDetectionEffect( vec2 );

void main() 
{ 
	//vec2 texCoordTmp = vec3(texture( texture, texCoord ));
    fColor = router(texCoord, effectMode);
	//fColor = router(texCoord, 3);
}

vec4 router(vec2 texCoordTmp, int effectModeTmp)
{
	switch(effectModeTmp)
	{
		case 0:
			return texture( texture, texCoordTmp );
		case 1:
			return LuminanceEffect(texCoordTmp);
		
		case 2:
			return NegativeEffect(texCoordTmp);

		case 3:
			//color = vec3(LuminanceEffect(color));
			return EdgeDetectionEffect(texCoordTmp);

		default:
			return texture( texture, texCoordTmp );
	}
}

vec4 EdgeDetectionEffect(vec2 texCoordTmp)
{
	const vec3 LUMCOEFFS = vec3(0.2125, 0.7154, 0.0721);
	ivec2 ires = textureSize( texture, 0);
	float ResS = float( ires.s );
	float ResT = float( ires.t );
	vec3  irgb = vec3(LuminanceEffect(texCoordTmp));//texture(texture, texCoord ).rgb;

	vec2 stp0 = vec2(1.0/ResS,  0.0 ); //texel offsets
	vec2 st0p = vec2(0.0 ,      1.0/ResT);
	vec2 stpp = vec2(1.0/ResS,  1.0/ResT);
	vec2 stpm = vec2(1.0/ResS, -1.0/ResT);

	float i00   = dot( texture( texture, texCoordTmp      ).rgb , LUMCOEFFS);
	float im1m1 = dot( texture( texture, texCoordTmp-stpp ).rgb , LUMCOEFFS);
	float ip1p1 = dot( texture( texture, texCoordTmp+stpp ).rgb , LUMCOEFFS);
	float im1p1 = dot( texture( texture, texCoordTmp-stpm ).rgb , LUMCOEFFS);
	float ip1m1 = dot( texture( texture, texCoordTmp+stpm ).rgb , LUMCOEFFS);
	float im10  = dot( texture( texture, texCoordTmp-stp0 ).rgb , LUMCOEFFS);
	float ip10  = dot( texture( texture, texCoordTmp+stp0 ).rgb , LUMCOEFFS);
	float i0m1  = dot( texture( texture, texCoordTmp-st0p ).rgb , LUMCOEFFS);
	float i0p1  = dot( texture( texture, texCoordTmp+st0p ).rgb , LUMCOEFFS);

	float h = -1.0 * im1p1 - 2.0 * i0p1 - 1.0 * ip1p1 + 1.0 * im1m1 + 2.0 * i0m1 + 1.0 * ip1m1;
	float v = -1.0 * im1m1 - 2.0 * im10 - 1.0 * im1p1 + 1.0 * ip1m1 + 2.0 * ip10 + 1.0 * ip1p1;

	float mag = sqrt(h*h + v*v);
	vec3 target = vec3(mag, mag, mag);
	return vec4( mix(irgb, target, texCoordTmp.y), 1.0);
}

vec4 NegativeEffect(vec2 texCoordTmp)
{
	vec4 color = texture( texture, texCoordTmp );
	return vec4( 1.0 - color.x, 1.0 - color.y, 1.0 - color.z, 1.0);
}

vec4 LuminanceEffect(vec2 texCoordTmp)
{
	const vec3 LUMCOEFFS = vec3(0.2125, 0.7154, 0.0721);
	vec4 color = texture( texture, texCoordTmp );
	float luminance = dot(color, LUMCOEFFS);
	return vec4( luminance, luminance,luminance, 1.0);
}

//vec4 EmbossingEffect()
//{

//}



/*
ToonRendering
TwirlTransformation
RippleTransformation
SphericalTransformation
*/