#version 150

in  vec2 texCoord;
out vec4 fColor;

uniform sampler2D texture;
uniform  int effectMode;

const vec3 LUMCOEFFS = vec3(0.2125, 0.7154, 0.0721);
const float PI = 3.14159265;

vec3 router( vec3 , int );
vec3 LuminanceEffect( vec3 );
vec3 NegativeEffect( vec3 );
vec3 EdgeDetectionEffect( vec3 );
vec3 EmbossingEffect( vec3 );
vec3 ToonRendering( vec3 );
vec3 TwirlTransformation( vec3 );
vec3 RippleTransformation( vec3 );
vec3 SphericalTransformation( vec3 );

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
			return TwirlTransformation(color);
		
		case 7:
			return RippleTransformation(color);
		
		case 8:
			return SphericalTransformation(color);

		default:
			return color;
	}
}

vec3 SphericalTransformation(vec3 color)
{
	/*
	ivec2 ires = textureSize( texture, 0);
	float Res = float( ires.s );	// assume it’s a square texture image
	float rMax = Res/2.0;
	float p = 1.8f;
	vec2 center = vec2(Res/2.0,Res/2.0);

	vec2 st = texCoord;
	vec2 d = vec2(st.x - center.x, st.y - center.y);
	float r = sqrt(d.x*d.x + d.y*d.y);
	float z = sqrt(rMax*rMax + r*r);
	
	vec2 b = vec2((1-1/p)*asin());

	float Radius = Res * uR;
	vec2 xy = Res * st;				// pixel coordinates from texture coords
	vec2 dxy = xy - Res/2.0;			// twirl center is (Res/2, Res/2)
	
	float r = sqrt( dxy.x * dxy.x + dxy.y * dxy.y );

	float beta = atan( dxy.y, dxy.x) + radians(uD) * (Radius - r)/Radius;

	vec2 xy1 = xy;
	if(r <= Radius)
	{
		xy1 = Res/2.0 + r * vec2( cos(beta), sin(beta) );
	}
	st = xy1/Res;					// restore coordinates
	
	return vec3(texture( texture, st ));
	*/
	return color;
}

vec3 RippleTransformation(vec3 color)
{
	float tx = 120.0f;
	float ty = 250.0f;
	float ax = 10.0f;
	float ay = 15.0f;

	ivec2 ires = textureSize( texture, 0);
	float Res = float( ires.s );	// assume it’s a square texture image
	vec2 st = Res * texCoord;

	vec2 xy = vec2( st.x + ax * sin(2*PI*st.y/tx),	st.y + ay * sin(2*PI*st.x/ty)  );

	st = xy/Res;

	return vec3(texture( texture, st ));
}

vec3 TwirlTransformation(vec3 color)
{
	float uD = 45.0; 
	float uR = 0.50;

	ivec2 ires = textureSize( texture, 0);
	float Res = float( ires.s );	// assume it’s a square texture image

	vec2 st = texCoord;
	float Radius = Res * uR;
	vec2 xy = Res * st;				// pixel coordinates from texture coords
	vec2 dxy = xy - Res/2.0;			// twirl center is (Res/2, Res/2)
	float r = sqrt( dxy.x * dxy.x + dxy.y * dxy.y );
	float beta = atan( dxy.y, dxy.x) + radians(uD) * (Radius - r)/Radius;

	vec2 xy1 = xy;
	if(r <= Radius)
	{
		xy1 = Res/2.0 + r * vec2( cos(beta), sin(beta) );
	}
	st = xy1/Res;					// restore coordinates

	return vec3(texture( texture, st ));
}

vec3 ToonRendering(vec3 color)
{
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
		//vec3 uQuantize = vec3(3.3, 3.3, 3.3);
		float uQuantize = 6.0;
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

	return mix(grayVersion, colorVersion, -0.33);
}

vec3 EdgeDetectionEffect(vec3 color)
{
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

