// Two-Dimensional Sierpinski Gasket       
// Generated using randomly selected vertices and bisection

#include "Angel.h"


//----------------------------------------------------------------------------
int width = 0;
int height = 0;

// remember to prototype
void generateGeometry( void );
void display( void );
void keyboard( unsigned char key, int x, int y );
void quad( int a, int b, int c, int d );
void colorcube(void);
void drawCube(void);
void myDisplay( int );

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

// handle to program
GLuint program;

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
linkList ll = (linkNode*)malloc(sizeof(linkNode));

char fileName[4][10] = {"lsys1.txt","lsys2.txt","lsys3.txt","lsys4.txt"};
char plyFileName[2][15] = {"cylinder.ply","sphere.ply"};

static int countOfVertex[2];
static int countOfFace[2]; 
//store points from ply files
point4 points[200];
//store mesh information from ply files
face3    face[360];

point4 spherePointsBuf[1000];
color4 sphereColorsBuf[1000];
point4 cylinderPointsBuf[200];
color4 cylinderColorsBuf[200];

static int fileIndex = 0;
float xMax[2], xMin[2], yMax[2], yMin[2], zMax[2], zMin[2];

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
	if(fileIndex == 0)
	{
		for(int i = 0, j = 0; i < countOfFace[fileIndex]*3; i=i+3)
		{
			cylinderPointsBuf[i] = points[face[i/3].vertex1];
			cylinderPointsBuf[i+1] = points[face[i/3].vertex2];
			cylinderPointsBuf[i+2] = points[face[i/3].vertex3];
			cylinderColorsBuf[i] = color4( 0.0, 1.0, 0.0, 1.0 );
			cylinderColorsBuf[i+1] = color4( 0.0, 1.0, 0.0, 1.0 );
			cylinderColorsBuf[i+2] = color4( 0.0, 1.0, 0.0, 1.0 );
		}
	}
	else
	{
		for(int i = 0, j = 0; i < countOfFace[fileIndex]*3; i=i+3)
		{
			spherePointsBuf[i] = points[face[i/3].vertex1];
			spherePointsBuf[i+1] = points[face[i/3].vertex2];
			spherePointsBuf[i+2] = points[face[i/3].vertex3];
			sphereColorsBuf[i] = color4( 0.0, 1.0, 0.0, 1.0 );
			sphereColorsBuf[i+1] = color4( 0.0, 1.0, 0.0, 1.0 );
			sphereColorsBuf[i+2] = color4( 0.0, 1.0, 0.0, 1.0 );
		}
	}
}

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

//generate the drawing buffer for drawing, then draw it (actually, everything related to drawing is drawn here)
void drawFile(int fileIndex)
{
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	if(fileIndex == 0)
	{
		//glBufferData( GL_ARRAY_BUFFER, sizeof(pointsBuf), pointsBuf, GL_STATIC_DRAW );
		glBufferData( GL_ARRAY_BUFFER, sizeof(cylinderPointsBuf) + sizeof(cylinderColorsBuf), NULL, GL_STATIC_DRAW );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(cylinderPointsBuf), cylinderPointsBuf );
		glBufferSubData( GL_ARRAY_BUFFER, sizeof(cylinderPointsBuf), sizeof(cylinderColorsBuf), cylinderColorsBuf );
		GLuint vColor = glGetAttribLocation( program, "vColor" ); 
		glEnableVertexAttribArray( vColor );
		glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cylinderPointsBuf)) );
	}
	else
	{
		glBufferData( GL_ARRAY_BUFFER, sizeof(spherePointsBuf) + sizeof(sphereColorsBuf), NULL, GL_STATIC_DRAW );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(spherePointsBuf), spherePointsBuf );
		glBufferSubData( GL_ARRAY_BUFFER, sizeof(spherePointsBuf), sizeof(sphereColorsBuf), sphereColorsBuf );
		GLuint vColor = glGetAttribLocation( program, "vColor" ); 
		glEnableVertexAttribArray( vColor );
		glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(spherePointsBuf)) );
	}
	glEnable( GL_DEPTH_TEST );
    glDrawArrays( GL_TRIANGLES, 0, countOfFace[fileIndex]*3 );
	glDisable( GL_DEPTH_TEST ); 
	glFlush(); // force output to graphics hardware

	// use this call to double buffer
	 glutSwapBuffers();
}

void do_iteration(int index)
{
	readFile(index);
	linkList cursor;
	for(int i = iteration - 1; i < iteration; i++)
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

int get_count()
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
void generateGeometry( void )
{	
    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
	//
	//

	// Load shaders and use the resulting shader program
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );
    glUseProgram( program );
     // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	/*
    GLuint vColor = glGetAttribLocation( program, "vColor" ); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(spherePointsBuf)) );
	*/
	// sets the default color to clear screen
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); // white background
}

//----------------------------------------------------------------------------
// this is where the drawing should happen
void display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     // clear the window
	
	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)45.0, (GLfloat)width/(GLfloat)height, (GLfloat)0.1, (GLfloat) 1000.0);

	float viewMatrixf[16];
	viewMatrixf[0] = perspectiveMat[0][0];viewMatrixf[4] = perspectiveMat[0][1];
	viewMatrixf[1] = perspectiveMat[1][0];viewMatrixf[5] = perspectiveMat[1][1];
	viewMatrixf[2] = perspectiveMat[2][0];viewMatrixf[6] = perspectiveMat[2][1];
	viewMatrixf[3] = perspectiveMat[3][0];viewMatrixf[7] = perspectiveMat[3][1];

	viewMatrixf[8] = perspectiveMat[0][2];viewMatrixf[12] = perspectiveMat[0][3];
	viewMatrixf[9] = perspectiveMat[1][2];viewMatrixf[13] = perspectiveMat[1][3];
	viewMatrixf[10] = perspectiveMat[2][2];viewMatrixf[14] = perspectiveMat[2][3];
	viewMatrixf[11] = perspectiveMat[3][2];viewMatrixf[15] = perspectiveMat[3][3];
	
	Angel::mat4 modelMat = Angel::identity();
	modelMat = modelMat * Angel::Translate(0, 0, -1) * Angel::RotateZ(0.0f) * Angel::RotateY(0.0f) * Angel::RotateX(0.0f);
	//modelMat = modelMat * Angel::Scale(0.05,0.5,0.05);

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
	GLuint viewMatrix = glGetUniformLocationARB(program, "projection_matrix");
	glUniformMatrix4fv( viewMatrix, 1, GL_FALSE, viewMatrixf);

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	//glBufferData( GL_ARRAY_BUFFER, sizeof(pointsBuf), pointsBuf, GL_STATIC_DRAW );
	glBufferData( GL_ARRAY_BUFFER, sizeof(cylinderPointsBuf) + sizeof(cylinderColorsBuf), NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(cylinderPointsBuf), cylinderPointsBuf );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(cylinderPointsBuf), sizeof(cylinderColorsBuf), cylinderColorsBuf );
	GLuint vColor = glGetAttribLocation( program, "vColor" ); 
	glEnableVertexAttribArray( vColor );
	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cylinderPointsBuf)) );
	
	glEnable( GL_DEPTH_TEST );
    glDrawArrays( GL_TRIANGLES, 0, countOfFace[fileIndex]*3 );
	glDisable( GL_DEPTH_TEST ); 

	myDisplay(1);
	

}
void myDisplay( int fileIndex )
{
	Angel::mat4 modelMat = Angel::identity();

	modelMat = modelMat * Angel::Translate(0, -(yMax[0]-yMin[0])/2, -1) * Angel::RotateY(0.0f) * Angel::RotateX(0.0f);
	//modelMat = modelMat * Angel::Scale((xMax[0]-xMin[0])/(xMax[1]-xMin[1])*0.05,(xMax[0]-xMin[0])/(xMax[1]-xMin[1])*0.05,(xMax[0]-xMin[0])/(xMax[1]-xMin[1])*0.05);

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

	drawFile(fileIndex);

}
void normalize( void )
{
	static float var = 0.05; 
	float ratio = (xMax[0]-xMin[0])/(xMax[1]-xMin[1])*var;
	for(int i = 0; i < countOfFace[0]*3; i++)
	{
		cylinderPointsBuf[i].x *= var;
		cylinderPointsBuf[i].y *= 0.5;
		cylinderPointsBuf[i].z *= var;
		
	}
	xMax[0] *= var;
	xMin[0] *= var;
	yMax[0] *= 0.5;
	yMin[0] *= 0.5;
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
//----------------------------------------------------------------------------

// keyboard handler
void keyboard( unsigned char key, int x, int y )
{
    switch ( key ) 
	{
		case 'q':
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			//readVertexAndFaceFromFile(0);
			myDisplay(0);
			break;
		case 'w':
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			//readVertexAndFaceFromFile(1);
			myDisplay(1);
			break;
		case 'a':
			do_iteration(2);
			printf("\n\n%d\n",get_count());
			break;
		case 033:
			exit( EXIT_SUCCESS );
			break;
    }
}

//----------------------------------------------------------------------------
// entry point
int main( int argc, char **argv )
{
	// init glut
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
	width = 512;
	height = 512;
	ll->value = 'F';
	ll->next = NULL;
    

    glutInitContextVersion( 3, 1 );
    glutInitContextProfile( GLUT_CORE_PROFILE );

    glutCreateWindow( "HW3 -- Hao" );

	// init glew
    glewInit();

    generateGeometry();
	plyFileLoad(0);
	plyFileLoad(1);
	normalize();
	printf("%f\n", xMax[0]-xMin[0]);
	printf("length = %f\n", zMax[0]-zMin[0]);
	printf("%f", xMax[1]-xMin[1]);
	// assign handlers
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutMainLoop();
    return 0;
}
