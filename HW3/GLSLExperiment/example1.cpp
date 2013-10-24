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

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

// handle to program
GLuint program;

using namespace std;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

color4 colors[NumVertices];
typedef struct linklist
{
	char value;
	struct linklist *next;
} *linkList,linkNode;


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
typedef struct 			
{
	int vertex1;		//X point index
	int vertex2;		//Y point index
	int vertex3;		//Z point index
} face3;
static int countOfVertex;
static long countOfFace; 
//store points from ply files
point4 points[10000];
point4 pointsBuf[60000];
color4 colorsBuf[60000] = {color4( 0.0, 1.0, 0.0, 1.0 )};
//store mesh information from ply files
face3    face[20000];

//variables dealing with normal vecters
point4    normalOfFace[20000];
point4    normalVecter[40000];
static int fileIndex = 0;

static float xMax, xMin, yMax, yMin, zMax, zMin;
// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
    point4( -0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5, -0.5, -0.5, 1.0 ),
    point4( -0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5, -0.5, -0.5, 1.0 )
};
// RGBA olors
color4 vertex_colors[8] = {
    color4( 0.0, 0.0, 0.0, 1.0 ),  // black
    color4( 1.0, 0.0, 0.0, 1.0 ),  // red
    color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
    color4( 0.0, 1.0, 0.0, 1.0 ),  // green
    color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
    color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
    color4( 1.0, 1.0, 1.0, 1.0 ),  // white
    color4( 0.0, 1.0, 1.0, 1.0 )   // cyan
};
//load info from ply file into arrays
void readVertexAndFaceFromFile(int fileIndex)
{
	char line[256];
	FILE *inStream;
	int lineNum = 0;
	float x,y,z;
	int vertex1, vertex2, vertex3;

	xMax = yMax = zMax = -999999;
	xMin = yMin = zMin = FLT_MAX;

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
			fscanf(inStream, "%d",&countOfVertex);
		}
		else if(strcmp(line, "face") == 0)
		{
			fscanf(inStream, "%ld",&countOfFace);

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
	
	for(int j = 0; j < countOfVertex; j++)
	{	//read each vertex
		
		fscanf(inStream,"%f %f %f", &x, &y, &z);
		if(j == 0)
		{
			xMax = xMin = x;
			yMax = yMin = y;
			zMax = zMin = z;
		}
		points[j] =  point4( x, y, z, 1.0 );
		//printf("%f %f %f\n", x,y,z);
		if(xMax < x) xMax = x;
		if(yMax < y) yMax = y;
		if(zMax < z) zMax = z;
		if(xMin > x) xMin = x;
		if(yMin > y) yMin = y;
		if(zMin > z) zMin = z;
	}

	for(int j = 0; j < countOfFace; j++)
	{	//read each vertex
		fscanf(inStream,"%d %d %d %d ", &lineNum, &vertex1, &vertex2, &vertex3);
		//printf("%d %d %d\n", vertex1,vertex2,vertex3);
		face[j].vertex1 =  vertex1;
		face[j].vertex2 =  vertex2;
		face[j].vertex3 =  vertex3;
	}

	fclose(inStream);
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
void drawFile()
{
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	for(int i = 0, j = 0; i < countOfFace*3; i=i+3)
	{
		pointsBuf[i] = points[face[i/3].vertex1];
		pointsBuf[i+1] = points[face[i/3].vertex2];
		pointsBuf[i+2] = points[face[i/3].vertex3];
		colorsBuf[i] = color4( 0.0, 1.0, 0.0, 1.0 );
		colorsBuf[i+1] = color4( 0.0, 1.0, 0.0, 1.0 );
		colorsBuf[i+2] = color4( 0.0, 1.0, 0.0, 1.0 );
	}
	//glBufferData( GL_ARRAY_BUFFER, sizeof(pointsBuf), pointsBuf, GL_STATIC_DRAW );
	glBufferData( GL_ARRAY_BUFFER, sizeof(pointsBuf) + sizeof(colorsBuf), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(pointsBuf), pointsBuf );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(pointsBuf), sizeof(colorsBuf), colorsBuf );

	glEnable( GL_DEPTH_TEST );
	
    glDrawArrays( GL_TRIANGLES, 0, countOfFace*3 );
	glDisable( GL_DEPTH_TEST ); 
	glFlush(); // force output to graphics hardware

	// use this call to double buffer
	glutSwapBuffers();
}

void do_iteration( )
{

}



// quad generates two triangles for each face and assigns colors
//    to the vertices
int Index = 0;
void quad( int a, int b, int c, int d )
{
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
    colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}

// generate 12 triangles: 36 vertices and 36 colors
void colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

void generateGeometry( void )
{	
    colorcube();

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		  NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );


	// Load shaders and use the resulting shader program
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );
    glUseProgram( program );
     // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( program, "vColor" ); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(sizeof(points)) );

	// sets the default color to clear screen
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

void drawCube(void)
{
	// change to GL_FILL
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	// draw functions should enable then disable the features 
	// that are specifit the themselves
	// the depth is disabled after the draw 
	// in case you need to draw overlays
	glEnable( GL_DEPTH_TEST );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
	glDisable( GL_DEPTH_TEST ); 
}

//----------------------------------------------------------------------------
// this is where the drawing should happen
void display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     // clear the window
	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)45.0, (GLfloat)width/(GLfloat)height, (GLfloat)0.1, (GLfloat) 100.0);

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
	modelMat = modelMat * Angel::Translate(-(xMax+xMin)/2, -(yMax+yMin)/2, - sqrt(pow(xMax-xMin,2)+pow(yMax-yMin,2)+pow(zMax-zMin,2))) * Angel::RotateY(0.0f) * Angel::RotateX(0.0f);
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
	readVertexAndFaceFromFile(1);
	drawFile();
}

//----------------------------------------------------------------------------

// keyboard handler
void keyboard( unsigned char key, int x, int y )
{
    switch ( key ) 
	{
		case 'q':
			readFile(2);
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
    // If you are using freeglut, the next two lines will check if 
    // the code is truly 3.2. Otherwise, comment them out
    
	// should run a test here 
	// with different cases
	// this is a good time to find out information about
	// your graphics hardware before you allocate any memory
    glutInitContextVersion( 3, 1 );
    glutInitContextProfile( GLUT_CORE_PROFILE );

	// create window
	// opengl can be incorperated into other packages like wxwidgets, fltoolkit, etc.
    glutCreateWindow( "HW3 -- Hao" );

	// init glew
    glewInit();

    generateGeometry();

	// assign handlers
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
	// should add menus
	// add mouse handler
	// add resize window functionality (should probably try to preserve aspect ratio)

	// enter the drawing loop
	// frame rate can be controlled with 
    glutMainLoop();
    return 0;
}
