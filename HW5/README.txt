1. Name:	Hao Zhou

2. Project Name:  image manipulation

3. How my program works(Keyboard events are case-insensitive):

	1). Key 'O': Display original picture (usain_bolt.bmp -- colorful without transformation)
	2). Key 'L': Display picture with Luminance effect (gray scale)
	3). Key 'N': Display image negative
	4). Key 'D': Display image with edge detection effect [Two versions: one is done in color, the other in luminance]
	5). Key 'E': Display image with embossing effect
	6). Key 'T': Display image with Toon rendering
	7). Key 'W': Display image with Twirl Transformation
	8). Key 'P': Display image with Ripple Transformation
	9). Key 'S': Display image with Spherical Transformation

4. Attention for keyboar issues 'D':
	1). Edge Detection in color:       If the previous keyboard event is 'O', or no previous keyboard event, then when you press 'D' or 'd', it will trigger edge detection in color
	2). Edge Detection in luminance:   Otherwise, it will trigger edge detection in luminance

5. In "fshader1.glsl", I implement each effect transformation as a separate function as following (One more function named router() calls proper function according to different parameter):

	vec3 LuminanceEffect( vec3 );
	vec3 NegativeEffect( vec3 );
	vec3 EdgeDetectionEffect( vec3 );
	vec3 EmbossingEffect( vec3 );
	vec3 ToonRendering( vec3 );
	vec3 TwirlTransformation( vec3 );
	vec3 RippleTransformation( vec3 );
	vec3 SphericalTransformation( vec3 );

6. If you have any questions, please feel free to contact: hzhou@wpi.edu. Thanks :-)