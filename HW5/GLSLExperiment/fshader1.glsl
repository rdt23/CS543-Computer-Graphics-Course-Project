#version 150

in  vec2 texCoord;
out vec4 fColor;

uniform sampler2D texture;
uniform  int effectMode;

vec3 router( vec3 , int );
vec3 LuminanceEffect( vec3 );
vec3 NegativeEffect( vec3 );
vec3 EdgeDetectionEffect( vec3 );
vec3 EmbossingEffect( vec3 );
vec3 ToonRendering( vec3 );

void main() 
{ 
	vec3 color = vec3(texture( texture, texCoord ));
    fColor = vec4(router(color, effectMode), 1.0);

}

vec3 router(vec3 color, int effectMode)
{
	switch(effectMode)
	{
		case 0:
			return color;
		case 1:
			return LuminanceEffect(color);
		
		case 2:
			return NegativeEffect(color);

		case 3:
			return EdgeDetectionEffect(color);

		case 4:
			return EmbossingEffect(color);

		case 5:
			return ToonRendering(color);
		
		case 6:
			return NegativeEffect(color);
		
		case 7:
			return NegativeEffect(color);
		
		case 8:
			return NegativeEffect(color);

		default:
			return color;
	}
}

vec3 ToonRendering(vec3 color)
{

	const vec3 LUMCOEFFS = vec3(0.2125, 0.7154, 0.0721);
	ivec2 ires = textureSize( texture, 0);
	float ResS = float( ires.s );
	float ResT = float( ires.t );

	vec2 stp0 = vec2(1.0/ResS,  0.0     ); //texel offsets
	vec2 st0p = vec2(0.0 ,      1.0/ResT);
	vec2 stpp = vec2(1.0/ResS,  1.0/ResT);
	vec2 stpm = vec2(1.0/ResS, -1.0/ResT);
	
	float i00   = dot( texture( texture, texCoord      ).rgb , LUMCOEFFS);
	float im1m1 = dot( texture( texture, texCoord-stpp ).rgb , LUMCOEFFS);
	float ip1p1 = dot( texture( texture, texCoord+stpp ).rgb , LUMCOEFFS);
	float im1p1 = dot( texture( texture, texCoord-stpm ).rgb , LUMCOEFFS);
	float ip1m1 = dot( texture( texture, texCoord+stpm ).rgb , LUMCOEFFS);
	float im10  = dot( texture( texture, texCoord-stp0 ).rgb , LUMCOEFFS);
	float ip10  = dot( texture( texture, texCoord+stp0 ).rgb , LUMCOEFFS);
	float i0m1  = dot( texture( texture, texCoord-st0p ).rgb , LUMCOEFFS);
	float i0p1  = dot( texture( texture, texCoord+st0p ).rgb , LUMCOEFFS);
	
	float h = -1.0 * im1p1 - 2.0 * i0p1 - 1.0 * ip1p1 + 1.0 * im1m1 + 2.0 * i0m1 + 1.0 * ip1m1;
	float v = -1.0 * im1m1 - 2.0 * im10 - 1.0 * im1p1 + 1.0 * ip1m1 + 2.0 * ip10 + 1.0 * ip1p1;

	float mag = sqrt( h*h + v*v );

	// if is not used here, evey logic goes into else block
	if( mag > 5.0)
	{
		return vec3( 0.0, 0.0, 0.0);
	}
	else
	{
		vec3 uQuantize = vec3(3.3, 3.3, 3.3);
		color.rgb *= 	uQuantize;
		color.rgb += vec3( 0.5, 0.5, 0.5 );
		ivec3 irgb = ivec3( color.rgb );
		color.rgb = vec3( irgb ) / uQuantize;
		return color;
	}

} 
vec3 EmbossingEffect(vec3 color)
{
	ivec2 ires = textureSize( texture, 0);
	float ResS = float( ires.s );
	float ResT = float( ires.t );

	vec2 stp0 = vec2(1.0/ResS,  0.0     ); //texel offsets
	vec2 stpp = vec2(1.0/ResS,  1.0/ResT);

	vec3 c00   = color;
	vec3 cp1p1 = texture( texture, texCoord+stpp ).rgb;

	c00   = LuminanceEffect(c00);
	cp1p1 = LuminanceEffect(cp1p1);

	vec3 diffs = c00 - cp1p1;

	float max = diffs.r;
	if(abs(diffs.g) > abs(max))
	{
		max = diffs.g;
	}
	if(abs(diffs.b) > abs(max))
	{
		max = diffs.b;
	}

	float gray = clamp(max + 0.5, 0.0, 1.0);
	vec3 grayVersion = vec3(gray, gray, gray);
	vec3 colorVersion = vec3(gray*c00);

	return mix(grayVersion, colorVersion, texCoord.y-0.77);
}

vec3 EdgeDetectionEffect(vec3 color)
{
	const vec3 LUMCOEFFS = vec3(0.2125, 0.7154, 0.0721);
	ivec2 ires = textureSize( texture, 0);
	float ResS = float( ires.s );
	float ResT = float( ires.t );

	vec2 stp0 = vec2(1.0/ResS,  0.0     ); //texel offsets
	vec2 st0p = vec2(0.0 ,      1.0/ResT);
	vec2 stpp = vec2(1.0/ResS,  1.0/ResT);
	vec2 stpm = vec2(1.0/ResS, -1.0/ResT);
	/*
	float i00   = dot( texture( texture, texCoord      ).rgb , LUMCOEFFS);
	float im1m1 = dot( texture( texture, texCoord-stpp ).rgb , LUMCOEFFS);
	float ip1p1 = dot( texture( texture, texCoord+stpp ).rgb , LUMCOEFFS);
	float im1p1 = dot( texture( texture, texCoord-stpm ).rgb , LUMCOEFFS);
	float ip1m1 = dot( texture( texture, texCoord+stpm ).rgb , LUMCOEFFS);
	float im10  = dot( texture( texture, texCoord-stp0 ).rgb , LUMCOEFFS);
	float ip10  = dot( texture( texture, texCoord+stp0 ).rgb , LUMCOEFFS);
	float i0m1  = dot( texture( texture, texCoord-st0p ).rgb , LUMCOEFFS);
	float i0p1  = dot( texture( texture, texCoord+st0p ).rgb , LUMCOEFFS);
	*/

	color = LuminanceEffect(color);
	float i00   = dot( LuminanceEffect(texture( texture, texCoord      ).rgb ), LUMCOEFFS);
	float im1m1 = dot( LuminanceEffect(texture( texture, texCoord-stpp ).rgb ), LUMCOEFFS);
	float ip1p1 = dot( LuminanceEffect(texture( texture, texCoord+stpp ).rgb ), LUMCOEFFS);
	float im1p1 = dot( LuminanceEffect(texture( texture, texCoord-stpm ).rgb ), LUMCOEFFS);
	float ip1m1 = dot( LuminanceEffect(texture( texture, texCoord+stpm ).rgb ), LUMCOEFFS);
	float im10  = dot( LuminanceEffect(texture( texture, texCoord-stp0 ).rgb ), LUMCOEFFS);
	float ip10  = dot( LuminanceEffect(texture( texture, texCoord+stp0 ).rgb ), LUMCOEFFS);
	float i0m1  = dot( LuminanceEffect(texture( texture, texCoord-st0p ).rgb ), LUMCOEFFS);
	float i0p1  = dot( LuminanceEffect(texture( texture, texCoord+st0p ).rgb ), LUMCOEFFS);

	float h = -1.0 * im1p1 - 2.0 * i0p1 - 1.0 * ip1p1 + 1.0 * im1m1 + 2.0 * i0m1 + 1.0 * ip1m1;
	float v = -1.0 * im1m1 - 2.0 * im10 - 1.0 * im1p1 + 1.0 * ip1m1 + 2.0 * ip10 + 1.0 * ip1p1;

	float mag = sqrt(h*h + v*v);
	vec3 target = vec3(mag, mag, mag);
	return mix(color, target, texCoord.y);
}

vec3 NegativeEffect(vec3 color)
{
	return vec3( 1.0 - color.x, 1.0 - color.y, 1.0 - color.z);
}

vec3 LuminanceEffect(vec3 color)
{
	const vec3 LUMCOEFFS = vec3(0.2125, 0.7154, 0.0721);
	float luminance = dot(color, LUMCOEFFS);
	return vec3( luminance, luminance,luminance);
}


/*
ToonRendering
TwirlTransformation
RippleTransformation
SphericalTransformation
*/