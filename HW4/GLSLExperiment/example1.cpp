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
void do_iteration(int);
int get_count( void );
void normalize( void );
void drawTree( int );
float RandomNumber(float , float );
void drawSphere( void );
void drawCylinder( void );
void flush( void );
void drawForest( void );
void drawGround( void );
void drawCar( void );
void drawCow( void );
void drawSmallCar( void );
void setAndLoadTexture( int );

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

/* default data*/
/* can enter other values via command line arguments */

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
char plyFileName[3][20] = {"cylinder.ply","sphere.ply","big_porsche.ply"};
char textureFileName[2][15] = {"grass.bmp","stones.bmp"};
static int countOfVertex[4];
static int countOfFace[4]; 

//store points from ply files
point4 points[200];
//store mesh information from ply files
face3    face[10500];

point4 carPointsBuf[32000];
color4 carColorsBuf[32000];

point4 spherePointsBuf[1000];
color4 sphereColorsBuf[1000];
point4 cylinderPointsBuf[200];
color4 cylinderColorsBuf[200];

/* store current state for translation */
point4 currentPoint;
angle4 currentAngle;

static int fileIndex = 0;
static int textureIndex = 0;
float xMax[4], xMin[4], yMax[4], yMin[4], zMax[4], zMin[4];
bmpread_t bitmap;
static  GLuint  texture = 0;
vec2 textureCoordinates[6] = {
		vec2( 0.0,30.0),
		vec2( 0.0, 0.0),
		vec2(30.0, 0.0),
		vec2(30.0, 0.0),
		vec2(30.0,30.0),
		vec2( 0.0,30.0)
	};
point4 pointsGround[6] = {
	point4( -100.0, 0.0, -200.0, 1.0 ),
	point4( -100.0, 0.0,  200.0, 1.0 ),
	point4(  100.0, 0.0,  200.0, 1.0 ),
	point4(  100.0, 0.0,  200.0, 1.0 ),
	point4(  100.0, 0.0, -200.0, 1.0 ),
	point4( -100.0, 0.0, -200.0, 1.0 )
    };

mat4 viewMat = LookAt(vec4(0,0,70,1), vec4(0,0,0,1), vec4(0,1,0,0));

int fogParameter = 0;// [used in shader] - 0:diable; 1:enabled with liner increment; 2:enabled with exponential increment

void setAndLoadTexture( int texttureIndex )
{
	if(!bmpread(textureFileName[texttureIndex], 0, &bitmap))
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
		fscanf(inStream,"%f %f %f", &z, &x, &y);
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
		//printf("%d %d %d\n", vertex1,vertex2,vertex3);
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
				cylinderColorsBuf[i] = color4( 0.0, 1.0, 0.0, 1.0 );
				cylinderColorsBuf[i+1] = color4( 0.0, 1.0, 0.0, 1.0 );
				cylinderColorsBuf[i+2] = color4( 0.0, 1.0, 0.0, 1.0 );
			}
			break;
		case 1:
			for(int i = 0; i < countOfFace[fileIndex]*3; i=i+3)
			{
				spherePointsBuf[i] = points[face[i/3].vertex1];
				spherePointsBuf[i+1] = points[face[i/3].vertex2];
				spherePointsBuf[i+2] = points[face[i/3].vertex3];
				sphereColorsBuf[i] = color4( 0.0, 1.0, 0.0, 1.0 );
				sphereColorsBuf[i+1] = color4( 0.0, 1.0, 0.0, 1.0 );
				sphereColorsBuf[i+2] = color4( 0.0, 1.0, 0.0, 1.0 );
			}
			break;
		case 2:
			for(int i = 0; i < countOfFace[fileIndex]*3; i=i+3)
			{
				carPointsBuf[i] = points[face[i/3].vertex1];
				carPointsBuf[i+1] = points[face[i/3].vertex2];
				carPointsBuf[i+2] = points[face[i/3].vertex3];
				carColorsBuf[i]   = color4(1.0, 1.0, 0.0, 1.0 );
				carColorsBuf[i+1] = color4(1.0, 1.0, 0.0, 1.0 );
				carColorsBuf[i+2] = color4(1.0, 1.0, 0.0, 1.0 );
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

	//mat4 viewMat = LookAt(vec4(0,0,100,1), vec4(0,0,0,1), vec4(0,1,0,0));
	Angel::mat4 modelMat = Angel::identity();
	modelMat = modelMat * Angel::Translate(currentPoint.x , currentPoint.y , currentPoint.z) * Angel::RotateZ(0.0f) * Angel::RotateY(0.0f) * Angel::RotateX(0.0f);
	modelMat = viewMat * modelMat;
	// set up projection matricies
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_TRUE, modelMat);

	GLint enableTreeColor = glGetUniformLocation(program, "enableTreeColor");
    glUniform1i( enableTreeColor, 1);

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
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	Angel::mat4 modelMat = Angel::identity();
	//mat4 viewMat = LookAt(vec4(0,0,100,1), vec4(0,0,0,1), vec4(0,1,0,0));
	modelMat = modelMat * Angel::Translate(currentPoint.x , currentPoint.y , currentPoint.z) * Angel::RotateZ(currentAngle.z) * Angel::RotateY(currentAngle.y-90.0f) * Angel::RotateX(currentAngle.x);
	
	// update currentPoint now
	currentPoint = modelMat*point4(0,1.0f,0,1);
	modelMat = viewMat * modelMat;
	//currentAngle.w = 1.0f;
	currentPoint.w = 1.0f;
	
	// set up projection matricies
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_TRUE, modelMat );

	glBufferData( GL_ARRAY_BUFFER, sizeof(cylinderPointsBuf) + sizeof(cylinderColorsBuf), NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(cylinderPointsBuf), cylinderPointsBuf );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(cylinderPointsBuf), sizeof(cylinderColorsBuf), cylinderColorsBuf );

	GLint enableTreeColor = glGetUniformLocation(program, "enableTreeColor");
    glUniform1i( enableTreeColor, 1);

	GLuint vColor = glGetAttribLocation( program, "vColor" ); 
	glEnableVertexAttribArray( vColor );
	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cylinderPointsBuf)) );

	glEnable( GL_DEPTH_TEST );
    glDrawArrays( GL_TRIANGLES, 0, countOfFace[0]*3 );
	glDisable( GL_DEPTH_TEST );


}
void drawCar( void )
{
	float xRand = RandomNumber(7,10);
	float zRand = RandomNumber(0,2);
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	Angel::mat4 modelMat = Angel::identity();
	
	modelMat = modelMat *Angel::Translate((xMax[2]+xMin[2]+xRand), -(yMax[2]+yMin[2])/2-3.20, 
				-sqrt(pow(xMax[2]-xMin[2],2)+pow(yMax[2]-yMin[2],2)+pow(zMax[2]-zMin[2],2))+zRand) * 
					Angel::RotateY(90.0f) * Angel::RotateZ(90.0f);
	modelMat = modelMat *Angel::Scale(0.7,0.7,0.7);

	float modelMatrixf[16];
	modelMatrixf[0] = modelMat[0][0];modelMatrixf[4] = modelMat[0][1];
	modelMatrixf[1] = modelMat[1][0];modelMatrixf[5] = modelMat[1][1];
	modelMatrixf[2] = modelMat[2][0];modelMatrixf[6] = modelMat[2][1];
	modelMatrixf[3] = modelMat[3][0];modelMatrixf[7] = modelMat[3][1];

	modelMatrixf[8] = modelMat[0][2];modelMatrixf[12] = modelMat[0][3];
	modelMatrixf[9] = modelMat[1][2];modelMatrixf[13] = modelMat[1][3];
	modelMatrixf[10] = modelMat[2][2];modelMatrixf[14] = modelMat[2][3];
	modelMatrixf[11] = modelMat[3][2];modelMatrixf[15] = modelMat[3][3];
	
	// set up projection matricies
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, modelMatrixf );

	glBufferData( GL_ARRAY_BUFFER, sizeof(carPointsBuf) + sizeof(carColorsBuf), NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(carPointsBuf), carPointsBuf );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(carPointsBuf), sizeof(carColorsBuf), carColorsBuf );
	GLuint vColor = glGetAttribLocation( program, "vColor" ); 
	glEnableVertexAttribArray( vColor );
	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(carColorsBuf)) );

	glEnable( GL_DEPTH_TEST );
    glDrawArrays( GL_TRIANGLES, 0, countOfFace[2]*3 );
	glDisable( GL_DEPTH_TEST );
}
void drawSmallCar( void )
{
	// randomize the location for small car
	float xRand = RandomNumber(-7,-3);
	float zRand = RandomNumber(-3,2);
	color4 randColor = color4(RandomNumber(0,2.0), RandomNumber(0,2.0) , RandomNumber(0,2.0), 1.0f);
	for(int i = 0, j = 0; i < countOfFace[2]*3; i++)
	{
		carColorsBuf[i] = randColor;//color4( 0.0, 1.0, 0.0, 1.0 );
	}
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	Angel::mat4 modelMat = Angel::identity();
	
	modelMat = modelMat *Angel::Translate((xMax[2]+xMin[2]+xRand), -(yMax[2]+yMin[2])/2-3.20, 
				-sqrt(pow(xMax[2]-xMin[2],2)+pow(yMax[2]-yMin[2],2)+pow(zMax[2]-zMin[2],2))+zRand) * 
					Angel::RotateY(0.0f) * Angel::RotateZ(90.0f);
	modelMat = modelMat *Angel::Scale(0.5,0.5,0.5);

	float modelMatrixf[16];
	modelMatrixf[0] = modelMat[0][0];modelMatrixf[4] = modelMat[0][1];
	modelMatrixf[1] = modelMat[1][0];modelMatrixf[5] = modelMat[1][1];
	modelMatrixf[2] = modelMat[2][0];modelMatrixf[6] = modelMat[2][1];
	modelMatrixf[3] = modelMat[3][0];modelMatrixf[7] = modelMat[3][1];

	modelMatrixf[8] = modelMat[0][2];modelMatrixf[12] = modelMat[0][3];
	modelMatrixf[9] = modelMat[1][2];modelMatrixf[13] = modelMat[1][3];
	modelMatrixf[10] = modelMat[2][2];modelMatrixf[14] = modelMat[2][3];
	modelMatrixf[11] = modelMat[3][2];modelMatrixf[15] = modelMat[3][3];
	
	// set up projection matricies
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, modelMatrixf );

	glBufferData( GL_ARRAY_BUFFER, sizeof(carPointsBuf) + sizeof(carColorsBuf), NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(carPointsBuf), carPointsBuf );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(carPointsBuf), sizeof(carColorsBuf), carColorsBuf );

	GLint enableTreeColor = glGetUniformLocation(program, "enableTreeColor");
    glUniform1i( enableTreeColor, 1);

	GLuint vColor = glGetAttribLocation( program, "vColor" ); 
	glEnableVertexAttribArray( vColor );
	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(carColorsBuf)) );

	glEnable( GL_DEPTH_TEST );
    glDrawArrays( GL_TRIANGLES, 0, countOfFace[2]*3 );
	glDisable( GL_DEPTH_TEST );
}
//extend the formula from L-systems file, and store it into a link list
void do_iteration(int index)
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
//return the length of the above link list (test use only)
int get_count( void )
{
	linkList cursor = ll->next;
	int count = 0;
	while(cursor != NULL)
	{
		count ++;
		//printf("%c",cursor->value);
		cursor = cursor->next;
		
	}
	return count;
}
void init( void )
{	
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

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    // Since our texture coordinates match our vertex positions, we
    // can reuse the position data for our texture coordinates.
    GLuint vTexCoord = glGetAttribLocation( program, "vTexCoord" ); 
    glEnableVertexAttribArray( vTexCoord );
    glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(pointsGround)) );

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
	modelMat = modelMat * Angel::Translate(0.0 , -20.0 ,0.0) * Angel::RotateZ(0) * Angel::RotateY(0) * Angel::RotateX(0);
	mat4 m = viewMat * modelMat;
    GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
    glUniformMatrix4fv( modelMatrix, 1, GL_TRUE, m );
	
	GLint enableTreeColor = glGetUniformLocation(program, "enableTreeColor");
    glUniform1i( enableTreeColor, 0);

	glBufferData( GL_ARRAY_BUFFER, sizeof(pointsGround) + sizeof(textureCoordinates), NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(pointsGround), pointsGround );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(pointsGround), sizeof(textureCoordinates), textureCoordinates );
	
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );   
	glEnable( GL_DEPTH_TEST );
    glDrawArrays( GL_TRIANGLES, 0, 6 );
	glDisable( GL_DEPTH_TEST );
}
void drawTree( int fileIndex)
{
	if(fileIndex == -1)
	{
		drawForest();
	}
	else
	{
		stack<point4> currentPointHistory;
		stack<angle4> currentAngleHistory;
		do_iteration(fileIndex);

		linkList cursor = ll->next;
		currentAngle.x = 0;
		currentAngle.y = 0;
		currentAngle.z = 0;

		currentPoint.y = -20.0;
		switch(fileIndex)
		{
			case 0:
				currentPoint.y = -25.0;
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
				currentPoint.x = RandomNumber(-50, 50);
				currentPoint.z = RandomNumber(-50, -25);
				break;
			case 3:
				currentPoint.x = RandomNumber(-20, 20);
				currentPoint.z = RandomNumber(-1, -0.5);
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
				currentPoint.x = -17;
				currentPoint.z = 20;
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
					drawSphere();
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
	drawTree(0);
	drawTree(1);
	drawTree(4);
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

	//printf("fogParameter = %d\n", fogParameter);

	setAndLoadTexture(textureIndex);

    drawGround();
	drawTree(-1);
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

// keyboard handler
void keyboard( unsigned char key, int x, int y )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     // clear the window

    switch ( key ) 
	{
		case 'a':
			textureIndex = (textureIndex+1) % 2;
			display();
			break;

		case 'f':
			fogParameter = (fogParameter+1) % 3;
			display();
			break;

		case 033:
			exit( EXIT_SUCCESS );
			break;
		default:
			//flag = 0;
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
	//plyFileLoad(2);
	/* normalize the points for sphere and cylinder, 
	   so that they will be at the same scale */
	normalize();
	/* initialize random seed: */
	srand (time(NULL));
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
