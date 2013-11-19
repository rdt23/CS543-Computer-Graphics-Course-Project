#ifdef USER_INPUT
// Include the C++ iostreams classes for use when collecting user input
#include <iostream>
#endif 

#include "Angel.h"
#include "bmpread.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <stack>

// remember to prototype
void init( void );
void display( void );
void keyboard( unsigned char, int, int );
void plyFileLoad(int);
void readFile(int);
void doIteration(int);
void normalize( void );
void drawTree( int );
float RandomNumber(float , float );
void drawSphere( void );
void drawCylinder( void );
void flush( void );
void drawForest( void );
void drawGround( void );
void drawCar( void );
void drawBox( void );
void setAndLoadTexture( int );
void cubeMapInit( void );
void setGLPolygonMode( int );

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;
typedef Angel::vec4  angle4;
// handle to program
GLuint program;
// Use the C++ standard complex-number class.  It's a C++ template, so
//   we create a typedef of simplify our code.
#include <complex>
typedef std::complex<float>  Complex;

inline float Random() { return float(rand())/RAND_MAX; }

GLuint Projection;

#define CENTERX -0.5
#define CENTERY 0.5
#define HEIGHT 0.5
#define WIDTH 0.5
#define MAX_ITER 100

/* N x M array to be generated */

#define N 1280
#define M 800

float height = HEIGHT;          /* size of window in complex plane */
float width = WIDTH;


int n = N;
int m = M;

using namespace std;

typedef struct linklist
{
	char value;
	struct linklist *next;
} *linkList,linkNode;

typedef struct 			
{
	int vertex1;		//X point index
	int vertex2;		//Y point index
	int vertex3;		//Z point index
} face3;

int len = 1;
int iteration = 3;
float rotaionX = 0;
float rotaionY = 0;
float rotaionZ = 0;
char start = 'F';
char formula[32];
// link list used to extend the formula for certain iterations
linkList ll = (linkNode*)malloc(sizeof(linkNode));

char fileName[5][10] = {"lsys1.txt","lsys2.txt","lsys3.txt","lsys4.txt","lsys5.txt"};
char plyFileName[4][20] = {"cylinder.ply","sphere.ply","big_porsche.ply", "box.ply"};
char textureFileName[2][15] = {"grass.bmp","stones.bmp"};
static int countOfVertex[3];
static int countOfFace[4]; 

//store points from ply files
point4 points[6000];
//store mesh information from ply files
face3  face[10500];

point4  carPointsBuf[32000];
color4  carColorsBuf[32000];
point4 carNormalsBuf[32000];

point4  spherePointsBuf[1000];
color4  sphereColorsBuf[1000];
point4 sphereNormalsBuf[1000];

point4  cylinderPointsBuf[200];
color4  cylinderColorsBuf[200];
point4 cylinderNormalsBuf[200];

point4 boxPointsBuf[36];
color4 boxColorsBuf[36];
point4 boxNormalsBuf[36];

/* store current state for translation */
point4 currentPoint;
angle4 currentAngle;

static int fileIndex = 0;
static int textureIndex = 0;
float xMax[4], xMin[4], yMax[4], yMin[4], zMax[4], zMin[4];
bmpread_t bitmap;
static  GLuint  texture = 0;
vec2 textureCoordinates[6] = {
		vec2( 0.0,50.0),
		vec2( 0.0, 0.0),
		vec2(50.0, 0.0),
		vec2(50.0, 0.0),
		vec2(50.0,50.0),
		vec2( 0.0,50.0)
	};
point4 pointsGround[6] = {
	point4( -200.0, 0.0, -500.0, 1.0 ),
	point4( -200.0, 0.0,  500.0, 1.0 ),
	point4(  200.0, 0.0,  500.0, 1.0 ),
	point4(  200.0, 0.0,  500.0, 1.0 ),
	point4(  200.0, 0.0, -500.0, 1.0 ),
	point4( -200.0, 0.0, -500.0, 1.0 )
    };
vec4 eyeMatrix = vec4(0,37,70,1);
mat4 viewMat = LookAt(eyeMatrix, vec4(0,0,0,1), vec4(0,1,0,0));

int fogParameter = 0;// [used in shader] - 0:diable; 1:enabled with liner increment; 2:enabled with exponential increment
int enableShadows = 0;
float light[3] = {70,300,-200}; // location of light
static int textureModeValue = 1;

void setAndLoadTexture( int textureIndex )
{
	if(!bmpread(textureFileName[textureIndex], 0, &bitmap))
	{
		fprintf(stderr, "%s:error loading bitmap file\n", "grass.bmp");
		exit(1);
	}

    glActiveTexture( GL_TEXTURE0 );
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, bitmap.width, bitmap.height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap.rgb_data );
	bmpread_free(&bitmap);

	 // Set our texture samples to the active texture unit
    glUniform1i( glGetUniformLocation(program, "texture"), 0 );
    glBindTexture(GL_TEXTURE_2D, texture);

}
void setGLPolygonMode( int textureModeValue )	
{
	if(textureModeValue == 1)
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}
	else
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	}
}
//load info from ply file into arrays
void plyFileLoad(int fileIndex)
{
	char line[256];
	FILE *inStream;
	int lineNum = 0;
	float x,y,z;
	int vertex1, vertex2, vertex3;

	xMax[fileIndex] = yMax[fileIndex] = zMax[fileIndex] = -999999;
	xMin[fileIndex] = yMin[fileIndex] = zMin[fileIndex] = FLT_MAX;

	if((inStream = fopen(plyFileName[fileIndex], "rt")) == NULL) // Open The File
	{
		printf("File does not exist!");
		exit(0);
	}
	
	while(!feof(inStream))
	{
		//Just go through file header
		memset(line, 0, 256);
		fscanf(inStream, "%s",line);
		if(strcmp(line, "vertex") == 0)
		{
			fscanf(inStream, "%d",&countOfVertex[fileIndex]);
		}
		else if(strcmp(line, "face") == 0)
		{
			fscanf(inStream, "%d",&countOfFace[fileIndex]);

		}
		else if(strcmp(line, "end_header") == 0)
		{
			break;
		}
		else
		{
			continue;
		}

	}
	
	for(int j = 0; j < countOfVertex[fileIndex]; j++)
	{	//read each vertex
		if(fileIndex == 2)
		{
			fscanf(inStream,"%f %f %f", &x, &y, &z);
		}
		else
		{
			fscanf(inStream,"%f %f %f", &z, &x, &y);
		}

		if(j == 0)
		{
			xMax[fileIndex] = xMin[fileIndex] = x;
			yMax[fileIndex] = yMin[fileIndex] = y;
			zMax[fileIndex] = zMin[fileIndex] = z;
		}
		points[j] =  point4( x, y, z, 1.0 );
		//printf("%f %f %f\n", x,y,z);
		if(xMax[fileIndex] < x) xMax[fileIndex] = x;
		if(yMax[fileIndex] < y) yMax[fileIndex] = y;
		if(zMax[fileIndex] < z) zMax[fileIndex] = z;
		if(xMin[fileIndex] > x) xMin[fileIndex] = x;
		if(yMin[fileIndex] > y) yMin[fileIndex] = y;
		if(zMin[fileIndex] > z) zMin[fileIndex] = z;
	}

	for(int j = 0; j < countOfFace[fileIndex]; j++)
	{	//read each vertex
		fscanf(inStream,"%d %d %d %d ", &lineNum, &vertex1, &vertex2, &vertex3);
		face[j].vertex1 =  vertex1;
		face[j].vertex2 =  vertex2;
		face[j].vertex3 =  vertex3;
	}

	fclose(inStream);
	// Load different ply files into different point buffers  
	//   cylinder, sphere, car, cow[cow doesn't work here] 

	switch(fileIndex)
	{
		case 0:
			for(int i = 0; i < countOfFace[fileIndex]*3; i=i+3)
			{
				cylinderPointsBuf[i] = points[face[i/3].vertex1];
				cylinderPointsBuf[i+1] = points[face[i/3].vertex2];
				cylinderPointsBuf[i+2] = points[face[i/3].vertex3];
			}
			break;
		case 1:
			for(int i = 0; i < countOfFace[fileIndex]*3; i=i+3)
			{
				spherePointsBuf[i] = points[face[i/3].vertex1];
				spherePointsBuf[i+1] = points[face[i/3].vertex2];
				spherePointsBuf[i+2] = points[face[i/3].vertex3];
			}
			break;
		case 2:
			for(int i = 0; i < countOfFace[fileIndex]*3; i=i+3)
			{
				carPointsBuf[i] = points[face[i/3].vertex1];
				carPointsBuf[i+1] = points[face[i/3].vertex2];
				carPointsBuf[i+2] = points[face[i/3].vertex3];
			}
			break;
		case 3:
			for(int i = 0; i < countOfFace[fileIndex]*3; i=i+3)
			{
				boxPointsBuf[i] = points[face[i/3].vertex1];
				boxPointsBuf[i+1] = points[face[i/3].vertex2];
				boxPointsBuf[i+2] = points[face[i/3].vertex3];
			}
			break;
		default:
			break;
	}
	
}
//load L-systems file into memory
void readFile(int index)
{
	char line[64];
    FILE *inStream;

	linkList cursor = ll;
    if((inStream = fopen(fileName[index], "rt")) == NULL) // Open The File
    {
        printf("File does not exist!");
        exit(0);
    }

    while(!feof(inStream))
    {
         //Just go through file header
         memset(line, 0, 64);
         fscanf(inStream, "%s",line);
         if(strcmp(line, "len:") == 0)
         {
			 // load useful date into memory
             fscanf(inStream, "%d",&len);
			 fscanf(inStream, "%s %d",line, &iteration);
			 fscanf(inStream, "%s %f %f %f",line, &rotaionX, &rotaionY, &rotaionZ);
			 fscanf(inStream, "%s %c",line, &start);
			 fscanf(inStream, "%s %s",line, formula);
			 for(int i = 0; i < strlen(formula); i++)
			 {
				 linkList tmpNode = (linkNode*)malloc(sizeof(linkNode));
				 tmpNode->value = formula[i];
				 tmpNode->next = NULL;
				 cursor->next = tmpNode;
				 cursor = cursor->next;
			 }
			 break;
         }
		 
     }
        
	fclose(inStream);
}
void drawSphere( void )
{
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	Angel::mat4 modelMat = Angel::identity();
	modelMat = modelMat * Angel::Translate(currentPoint.x , currentPoint.y , currentPoint.z) * Angel::RotateZ(0.0f) * Angel::RotateY(0.0f) * Angel::RotateX(0.0f);
	modelMat = viewMat * modelMat;
	// set up projection matricies
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_TRUE, modelMat);

	GLint textureMode = glGetUniformLocation(program, "textureMode");
    glUniform1i( textureMode, 1);

	glBufferData( GL_ARRAY_BUFFER, sizeof(spherePointsBuf) + sizeof(sphereColorsBuf), NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(spherePointsBuf), spherePointsBuf );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(spherePointsBuf), sizeof(sphereColorsBuf), sphereColorsBuf );
	GLuint vColor = glGetAttribLocation( program, "vColor" ); 
	glEnableVertexAttribArray( vColor );
	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(spherePointsBuf)) );

	glEnable( GL_DEPTH_TEST );
    glDrawArrays( GL_TRIANGLES, 0, countOfFace[1]*3 );
	glDisable( GL_DEPTH_TEST );
}
void drawCylinder( void )
{
	for(int i = 0; i < countOfFace[0]*3; i++)
	{
		cylinderNormalsBuf[i] = vec4(normalize(cross(cylinderPointsBuf[i- i%3 + 1] - cylinderPointsBuf[i- i%3 ], 
													 cylinderPointsBuf[i- i%3 + 2] - cylinderPointsBuf[i- i%3 + 1])), 1.0f);
	}

	point4 shadowPoint[2];
	color4 shadowColor[2] = {color4(0,0,0,1),color4(0,0,0,1)};
	mat4 m; // shadow projection matrix initially identity
	m[3][1] = -1.0/light[1]; 
	m[3][3] = 0; 
	

	Angel::mat4 modelMat = Angel::identity();
	modelMat = modelMat * Angel::Translate(currentPoint.x , currentPoint.y , currentPoint.z) 
						* Angel::RotateZ(currentAngle.z) * Angel::RotateY(currentAngle.y-90.0f) * Angel::RotateX(currentAngle.x);

	// for draw shadows use
	shadowPoint[0] =currentPoint;

	// update currentPoint now
	currentPoint = modelMat * point4(0,1.0f,0,1);
	Angel::mat4 shadowsModelView = viewMat * Translate(0, 1, 0) * Translate(light[0], light[1], light[2]) * m * Translate(-light[0], -light[1], -light[2]);
	modelMat = viewMat * modelMat;
	currentPoint.w = 1.0f;
	
	// for draw shadows use
	shadowPoint[1] = currentPoint;

	// set up projection matricies
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_TRUE, modelMat );

	if(textureModeValue == 1)
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glBufferData( GL_ARRAY_BUFFER, sizeof(cylinderPointsBuf) + sizeof(cylinderColorsBuf), NULL, GL_STATIC_DRAW );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(cylinderPointsBuf), cylinderPointsBuf );
		glBufferSubData( GL_ARRAY_BUFFER, sizeof(cylinderPointsBuf), sizeof(cylinderColorsBuf), cylinderColorsBuf );
	}
	else
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glBufferData( GL_ARRAY_BUFFER, sizeof(cylinderPointsBuf) + sizeof(cylinderNormalsBuf), NULL, GL_STATIC_DRAW );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(cylinderPointsBuf), cylinderPointsBuf );
		glBufferSubData( GL_ARRAY_BUFFER, sizeof(cylinderPointsBuf), sizeof(cylinderNormalsBuf), cylinderNormalsBuf );
	}

	GLint textureMode = glGetUniformLocation(program, "textureMode");
	glUniform1i( textureMode, textureModeValue);

	GLuint vColor = glGetAttribLocation( program, "vColor" ); 
	glEnableVertexAttribArray( vColor );
	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cylinderColorsBuf)) );
	
	GLuint vNormal = glGetAttribLocation( program, "Normal" ); 
	glEnableVertexAttribArray( vNormal );
	glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cylinderNormalsBuf)) );



	glEnable( GL_DEPTH_TEST );
    glDrawArrays( GL_TRIANGLES, 0, countOfFace[0]*3 );
	glDisable( GL_DEPTH_TEST );

	if(enableShadows == 1)
	{
		GLuint shadowModelMatrix = glGetUniformLocationARB(program, "model_matrix");
		glUniformMatrix4fv( shadowModelMatrix, 1, GL_TRUE, shadowsModelView );

		glBufferData( GL_ARRAY_BUFFER, sizeof(shadowPoint) + sizeof(shadowColor) , NULL, GL_STATIC_DRAW );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(shadowPoint), shadowPoint );
		glBufferSubData( GL_ARRAY_BUFFER, sizeof(shadowPoint), sizeof(shadowColor), shadowColor );

		textureMode = glGetUniformLocation(program, "textureMode");
		glUniform1i( textureMode, 1);
		vColor = glGetAttribLocation( program, "vColor" ); 
		glEnableVertexAttribArray( vColor );
		glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(shadowPoint)) );


		glEnable( GL_DEPTH_TEST );
		glDrawArrays( GL_LINES, 0, 2 );
		glDisable( GL_DEPTH_TEST );
	}

}
void drawCar( void )
{
	for(int i = 0; i < countOfFace[2]*3; i++)
	{
		carColorsBuf[i] = color4(1.0, 0.0, 1.0, 1.0 );
		carNormalsBuf[i] = vec4(normalize(cross(carPointsBuf[i- i%3] - carPointsBuf[i- i%3 + 1], carPointsBuf[i- i%3 + 2] - carPointsBuf[i- i%3 + 1])), 1.0f);
	}

	//setGLPolygonMode(textureModeValue);

	Angel::mat4 modelMat = Angel::identity();
	mat4 m; // shadow projection matrix initially identity
	m[3][1] = -1.0/light[1]; 
	m[3][3] = 0;
	
	modelMat = modelMat *Angel::Translate(-20, 7, 20) * Angel::RotateY(0.0f) * Angel::RotateZ(0.0f);
	modelMat = modelMat *Angel::Scale(2.7,2.7,2.7);
	Angel::mat4 shadowsModelView = viewMat * Translate(0, 1, 0) * Translate(light[0], light[1], light[2]) * m * Translate(-light[0], -light[1], -light[2]) * modelMat;
	modelMat = viewMat * modelMat;
	
	// set up projection matricies
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_TRUE, modelMat );

	if(textureModeValue == 1)
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glBufferData( GL_ARRAY_BUFFER, sizeof(carPointsBuf) + sizeof(carColorsBuf), NULL, GL_STATIC_DRAW );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(carPointsBuf), carPointsBuf );
		glBufferSubData( GL_ARRAY_BUFFER, sizeof(carPointsBuf), sizeof(carColorsBuf), carColorsBuf );
	}
	else
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glBufferData( GL_ARRAY_BUFFER, sizeof(carPointsBuf) + sizeof(carNormalsBuf), NULL, GL_STATIC_DRAW );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(carPointsBuf), carPointsBuf );
		glBufferSubData( GL_ARRAY_BUFFER, sizeof(carPointsBuf), sizeof(carNormalsBuf), carNormalsBuf );
	}

	GLint textureMode = glGetUniformLocation(program, "textureMode");
	if(textureModeValue == 10)
	{
		int randMode = rand() % 2 + 2;
		glUniform1i( textureMode, randMode);
		textureModeValue = 10;
	}
	else
	{
		glUniform1i( textureMode, textureModeValue);
	}

	GLuint vColor = glGetAttribLocation( program, "vColor" ); 
	glEnableVertexAttribArray( vColor );
	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(carColorsBuf)) );

	GLuint vNormal = glGetAttribLocation( program, "Normal" ); 
	glEnableVertexAttribArray( vNormal );
	glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(carNormalsBuf)) );

	glEnable( GL_DEPTH_TEST );
    glDrawArrays( GL_TRIANGLES, 0, countOfFace[2]*3 );
	glDisable( GL_DEPTH_TEST );

	if(enableShadows == 1)
	{
		GLuint shadowModelMatrix = glGetUniformLocationARB(program, "model_matrix");
		glUniformMatrix4fv( shadowModelMatrix, 1, GL_TRUE, shadowsModelView );

		textureMode = glGetUniformLocation(program, "textureMode");
		glUniform1i( textureMode, 1);
		for(int i = 0; i < countOfFace[2]*3; i++)
		{
			carColorsBuf[i] = color4(0.0, 0.0, 0.0, 1.0 );
		}
		glBufferData( GL_ARRAY_BUFFER, sizeof(carPointsBuf) + sizeof(carColorsBuf), NULL, GL_STATIC_DRAW );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(carPointsBuf), carPointsBuf );
		glBufferSubData( GL_ARRAY_BUFFER, sizeof(carPointsBuf), sizeof(carColorsBuf), carColorsBuf );

		glEnable( GL_DEPTH_TEST );
		glDrawArrays( GL_TRIANGLES, 0, countOfFace[2]*3 );
		glDisable( GL_DEPTH_TEST );
	}

}
void drawBox( void )
{
	for(int i = 0; i < countOfFace[3]*3; i++)
	{
		boxColorsBuf[i] = color4(1.0, 1.0, 0.0, 1.0 );
		boxNormalsBuf[i] = vec4(normalize(cross(boxPointsBuf[i- i%3 + 1] - boxPointsBuf[i- i%3 ], boxPointsBuf[i- i%3 + 2] - boxPointsBuf[i- i%3 + 1])), 1.0f);
	}


	Angel::mat4 modelMat = Angel::identity();
	mat4 m; // shadow projection matrix initially identity
	m[3][1] = -1.0/light[1]; 
	m[3][3] = 0;
	
	modelMat = modelMat *Angel::Translate(-0, 13, -20) * Angel::RotateX(-90.0f) * Angel::RotateY(0.0f) * Angel::RotateZ(90.0f);
	modelMat = modelMat *Angel::Scale(11,11,11);
	Angel::mat4 shadowsModelView = viewMat * Translate(0, 1, 0) * Translate(light[0], light[1], light[2]) * m * Translate(-light[0], -light[1], -light[2]) * modelMat;
	modelMat = viewMat * modelMat;
	
	// set up projection matricies
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_TRUE, modelMat );

	if(textureModeValue == 1)
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glBufferData( GL_ARRAY_BUFFER, sizeof(boxPointsBuf) + sizeof(boxColorsBuf), NULL, GL_STATIC_DRAW );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(boxPointsBuf), boxPointsBuf );
		glBufferSubData( GL_ARRAY_BUFFER, sizeof(boxPointsBuf), sizeof(boxColorsBuf), boxColorsBuf );
	}
	else
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glBufferData( GL_ARRAY_BUFFER, sizeof(boxPointsBuf) + sizeof(boxNormalsBuf), NULL, GL_STATIC_DRAW );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(boxPointsBuf), boxPointsBuf );
		glBufferSubData( GL_ARRAY_BUFFER, sizeof(boxPointsBuf), sizeof(boxNormalsBuf), boxNormalsBuf );
	}
	
	GLint textureMode = glGetUniformLocation(program, "textureMode");
	if(textureModeValue == 10)
	{
		int randMode = rand() % 2 + 2;
		glUniform1i( textureMode, randMode);
		textureModeValue = 10;
	}
	else
	{
		glUniform1i( textureMode, textureModeValue);
	}

	GLuint vColor = glGetAttribLocation( program, "vColor" ); 
	glEnableVertexAttribArray( vColor );
	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(boxColorsBuf)) );
	
	GLuint vNormal = glGetAttribLocation( program, "Normal" ); 
	glEnableVertexAttribArray( vNormal );
	glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(boxNormalsBuf)) );

	glEnable( GL_DEPTH_TEST );
    glDrawArrays( GL_TRIANGLES, 0, countOfFace[3]*3 );
	glDisable( GL_DEPTH_TEST );

	if(enableShadows == 1)
	{
		GLuint shadowModelMatrix = glGetUniformLocationARB(program, "model_matrix");
		glUniformMatrix4fv( shadowModelMatrix, 1, GL_TRUE, shadowsModelView );

		for(int i = 0; i < countOfFace[3]*3; i++)
		{
			boxColorsBuf[i] = color4(0.0, 0.0, 0.0, 1.0); 
		}
		GLint textureMode = glGetUniformLocation(program, "textureMode");
		glUniform1i( textureMode, 1);

		glBufferData( GL_ARRAY_BUFFER, sizeof(boxPointsBuf) + sizeof(boxColorsBuf), NULL, GL_STATIC_DRAW );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(boxPointsBuf), boxPointsBuf );
		glBufferSubData( GL_ARRAY_BUFFER, sizeof(boxPointsBuf), sizeof(boxColorsBuf), boxColorsBuf );

		glEnable( GL_DEPTH_TEST );
		glDrawArrays( GL_TRIANGLES, 0, countOfFace[3]*3 );
		glDisable( GL_DEPTH_TEST );
	}
}
//extend the formula from L-systems file, and store it into a link list
void doIteration(int index)
{
	readFile(index);
	linkList cursor;
	for(int i = 1 ; i < iteration; i++) // from 1
	{
		cursor = ll->next;
		while(cursor != NULL)
		{
			if(cursor->value == 'F')
			{
				int formulaLength = strlen(formula);
				for(int j = 1; j < formulaLength; j++)
				{
					linkList tmpNode = (linkNode*)malloc(sizeof(linkNode));
					tmpNode->value = formula[j];
					tmpNode->next = cursor->next;
					cursor->next = tmpNode;

					cursor = cursor->next;
				}
				cursor = cursor->next;
			}
			else
			{
				cursor = cursor->next;
			}
		}
	}
}
void init( void )
{	
	//colorcube();
    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );

    // Load shaders and use the resulting shader program
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );
    glUseProgram( program );

	cubeMapInit();

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    // Since our texture coordinates match our vertex positions, we
    // can reuse the position data for our texture coordinates.
    GLuint vTexCoord = glGetAttribLocation( program, "vTexCoord" ); 
    glEnableVertexAttribArray( vTexCoord );
    glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(pointsGround)) );

	GLuint eyeMatrix = glGetAttribLocation( program, "eyeMatrix" ); 
	glEnableVertexAttribArray( eyeMatrix );
	glVertexAttribPointer( eyeMatrix, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(eyeMatrix)) );

	// sets the default color to clear screen
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}
float RandomNumber(float Min, float Max)
{
    return ((float(rand()) / float(RAND_MAX)) * (Max - Min)) + Min;
}
void drawGround( void )
{
	mat4 modelMat = Angel::identity();;
	modelMat = modelMat * Angel::Translate(0.0 , 0.0 ,0.0) * Angel::RotateZ(0) * Angel::RotateY(0) * Angel::RotateX(0);
	mat4 m = viewMat * modelMat;
    GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
    glUniformMatrix4fv( modelMatrix, 1, GL_TRUE, m );
	
	GLint textureMode = glGetUniformLocation(program, "textureMode");
    glUniform1i( textureMode, 0);

	glBufferData( GL_ARRAY_BUFFER, sizeof(pointsGround) + sizeof(textureCoordinates), NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(pointsGround), pointsGround );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(pointsGround), sizeof(textureCoordinates), textureCoordinates );
	
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );   
	glEnable( GL_DEPTH_TEST );
    glDrawArrays( GL_TRIANGLES, 0, 6 );
	glDisable( GL_DEPTH_TEST );
}
void drawTree( int fileIndex )
{
	if(fileIndex == -1)
	{
		drawForest();
	}
	else
	{
		stack<point4> currentPointHistory;
		stack<angle4> currentAngleHistory;
		doIteration(fileIndex);

		linkList cursor = ll->next;
		currentAngle.x = 0;
		currentAngle.y = 0;
		currentAngle.z = 0;

		currentPoint.y = 0.0;
		switch(fileIndex)
		{
			case 0:
				currentPoint.x = -77;
				currentPoint.z = -227;
				for(int i = 0; i < countOfFace[0]*3; i++)
				{
					cylinderColorsBuf[i] = color4( 0.0, 1.0, 0.0, 1.0 );
				}
				for(int i = 0; i < countOfFace[1]*3; i++)
				{
					sphereColorsBuf[i] = color4( 0.0, 1.0, 0.0, 1.0 );
				}
				break;
			case 1:
				currentPoint.x = 33;
				currentPoint.z = -30;
				for(int i = 0; i < countOfFace[0]*3; i++)
				{
					cylinderColorsBuf[i] = color4( 1.0, 0.0, 0.0, 1.0 );
				}
				for(int i = 0; i < countOfFace[1]*3; i++)
				{
					sphereColorsBuf[i] = color4( 1.0, 0.0, 0.0, 1.0 );
				}
				
				break;
			case 2:
				currentPoint.x = 17;
				currentPoint.z = 40;
				for(int i = 0; i < countOfFace[0]*3; i++)
				{
					cylinderColorsBuf[i] = color4( 0.0, 1.0, 0.0, 1.0 );
				}
				for(int i = 0; i < countOfFace[1]*3; i++)
				{
					sphereColorsBuf[i] = color4( 0.0, 1.0, 0.0, 1.0 );
				}
				break;
			case 3:
				currentPoint.x = 17;
				currentPoint.z = 10;
				for(int i = 0; i < countOfFace[0]*3; i++)
				{
					cylinderColorsBuf[i] = color4( 0.0, 1.0, 0.0, 1.0 );
				}
				for(int i = 0; i < countOfFace[1]*3; i++)
				{
					sphereColorsBuf[i] = color4( 0.0, 1.0, 0.0, 1.0 );
				}
				break;
			case 4:
				for(int i = 0; i < countOfFace[0]*3; i++)
				{
					cylinderColorsBuf[i] = color4( 0.0, 0.0, 1.0, 1.0 );
				}
				for(int i = 0; i < countOfFace[1]*3; i++)
				{
					sphereColorsBuf[i] = color4( 0.0, 0.0, 1.0, 1.0 );
				}
				currentPoint.x = 17;
				currentPoint.z = 37;
				break;

		}

		currentPoint.w = 1.0f;
		drawSphere();
		while(cursor != NULL)
		{
			switch(cursor->value)
			{
				case 'F':
					drawCylinder();
					if(textureModeValue == 1)
					{
						drawSphere();
					}
					break;
				case '+':
					currentAngle.x += rotaionX;
					break;
				case '-':
					currentAngle.x -= rotaionX;
					break;
				case '&':
					currentAngle.y += rotaionY;
					break;
				case '^':
					currentAngle.y -= rotaionY;
					break;
				case '\\':
					currentAngle.z += rotaionZ;
					break;
				case '/':
					currentAngle.z -= rotaionZ;
					break;
				case '[':
					currentPointHistory.push(currentPoint);
					currentAngleHistory.push(currentAngle);
					break;
				case ']':
					currentPoint = currentPointHistory.top();
					currentAngle = currentAngleHistory.top();
					currentPointHistory.pop();
					currentAngleHistory.pop();
					break;
				case '|':
					currentAngle.x += 180;//-currentAngle.x;
					currentAngle.y += 180;//= -currentAngle.y;
					currentAngle.z += 180;//= -currentAngle.z;
					break;
				default:
					break;
			}
			cursor = cursor->next;
		}
	}
}
void drawForest( void )
{
	if(textureModeValue == 10)
	{
		textureModeValue = rand() % 2 + 2;
		drawTree(3);
		textureModeValue = rand() % 2 + 2;
		drawTree(1);
		textureModeValue = rand() % 2 + 2;
		drawTree(4);
		textureModeValue = 10;
	}
	else
	{
		drawTree(3);
		drawTree(1);
		drawTree(4);
	}
}
void flush( void )
{
	glFlush();
	glutSwapBuffers();
}
void display()
{
	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)90.0, (GLfloat)width/(GLfloat)height, (GLfloat)0.1, (GLfloat) 9998.0);
    Projection = glGetUniformLocationARB( program, "Projection" );

	glUniformMatrix4fv( Projection, 1, GL_TRUE, perspectiveMat);
	GLint enableFogWithSpecialIncrement = glGetUniformLocation(program, "enableFogWithSpecialIncrement");
    glUniform1i( enableFogWithSpecialIncrement, fogParameter);

	setAndLoadTexture(textureIndex);
    drawGround();
	drawCar();
	drawBox();
	drawForest();
    flush();
}
 //normalize cylinder and sphere, so that they have the same diameter, and the length of cylinder will be 1 (also change the start point of cylinder to the buttom).
void normalize( void )
{
	static float var = 0.01; 
	float ratio = (xMax[0]-xMin[0])/(xMax[1]-xMin[1])*var;
	for(int i = 0; i < countOfFace[0]*3; i++)
	{
		cylinderPointsBuf[i].x *= var;
		cylinderPointsBuf[i].y = cylinderPointsBuf[i].y * 0.5 + 0.5;
		cylinderPointsBuf[i].z *= var;
		
	}
	xMax[0] *= var;
	xMin[0] *= var;
	yMax[0] = 1;
	yMin[0] = 0;
	zMax[0] *= var;
	zMin[0] *= var;

	for(int i = 0; i < countOfFace[1]*3; i++)
	{
		spherePointsBuf[i].x *= ratio;
		spherePointsBuf[i].y *= ratio;
		spherePointsBuf[i].z *= ratio;
	}
	xMax[1] *= (xMax[0]-xMin[0])/(xMax[1]-xMin[1]);
	xMin[1] = -xMax[1];
	yMax[1] = xMax[1];
	yMin[1] = -xMax[1];
	zMax[1] = xMax[1];
	zMin[1] = -xMax[1];
}

void cubeMapInit( void )
{
	bmpread_t bitmap1, bitmap2, bitmap3, bitmap4, bitmap5, bitmap6;
	GLuint texMapLocation;
	GLuint tex[1];
	bmpread("nvnegx.bmp", 0, &bitmap2);
	bmpread("nvnegy.bmp", 0, &bitmap4);
	bmpread("nvnegz.bmp", 0, &bitmap6);
	bmpread("nvposx.bmp", 0, &bitmap1);
	bmpread("nvposy.bmp", 0, &bitmap3);
	bmpread("nvposz.bmp", 0, &bitmap5);

	glEnable(GL_TEXTURE_CUBE_MAP);
	// Create texture object
	glGenTextures(1, tex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex[0]);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X ,0, GL_RGB, bitmap1.width, bitmap1.height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap1.rgb_data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X ,0, GL_RGB, bitmap2.width, bitmap2.height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap2.rgb_data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y ,0, GL_RGB, bitmap3.width, bitmap3.height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap3.rgb_data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y ,0, GL_RGB, bitmap4.width, bitmap4.height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap4.rgb_data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z ,0, GL_RGB, bitmap5.width, bitmap5.height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap5.rgb_data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z ,0, GL_RGB, bitmap6.width, bitmap6.height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap6.rgb_data);

	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	texMapLocation = glGetUniformLocation(program, "texMap"); 
	glUniform1i(texMapLocation, tex[0]);

	bmpread_free(&bitmap1);
	bmpread_free(&bitmap2);
	bmpread_free(&bitmap3);
	bmpread_free(&bitmap4);
	bmpread_free(&bitmap5);
	bmpread_free(&bitmap6);

}
// keyboard handler
void keyboard( unsigned char key, int x, int y )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     // clear the window
	//textureModeValue = 1;
    switch ( key ) 
	{
		case 'd':
			enableShadows = (enableShadows+1) % 2;
			display();
			break;

		case 'a':
			textureIndex = (textureIndex+1) % 2;
			display();
			break;

		case 'f':
			fogParameter = (fogParameter+1) % 3;
			display();
			break;

		case 't':
			textureModeValue = 2;
			display();
			break;

		case 'v':
			textureModeValue = 3;
			display();
			break;

		case 'x':
			textureModeValue = 1;
			display();
			break;

		case 'k':
			textureModeValue = 10;
			display();
			break;

		case 033:
			exit( EXIT_SUCCESS );
			break;

		default:
			break;
    }

}
//----------------------------------------------------------------------------
// entry point
int main( int argc, char **argv )
{
	/* initial my linkList, and the first node is not used, so just store 'F' in it for fun */
	ll->value = 'F';
	ll->next = NULL;
	/* load sphere and cylinder into array */
	plyFileLoad(0);
	plyFileLoad(1);
	plyFileLoad(2);
	plyFileLoad(3);
	/* normalize the points for sphere and cylinder, 
	   so that they will be at the same scale */
	normalize();
	/* initial current variables */
	currentPoint.w = 1.0f;

	// init glut
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( N, M );
	

    glutInitContextVersion( 3,3 );
    glutInitContextProfile( GLUT_CORE_PROFILE );
	glewExperimental = GL_TRUE;
    glutCreateWindow( "HW4 -- Hao" );

	// init glew
    glewInit();

    init();
	// assign handlers
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
	
    glutMainLoop();
    return 0;
}
