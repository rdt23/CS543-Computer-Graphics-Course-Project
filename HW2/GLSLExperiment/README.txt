1. Description for each function in my source code:
	//read and store point & face count from ply file, then load points and faces information into two arrays
	void readVertexAndFaceFromFile(int);
	//default function
	void generateGeometry( void );
	//not used(replace it with my own function named displayFileInScreen) 
	void display( void );
	//deal with keyboard issue
	void keyboard( unsigned char key, int x, int y );
	//generate and store the points need to draw the object, then draw the object into screen
	void drawFile();
	//display control function, everything need to be displayed is controlled here
	void displayFileInScreen( void );
	//glutIdleFunc(move), deal with "X/x", "Y/y", "Z/z", "R", "B" event, to generate animation effect
	void move( void );
	//control function for move(), contrl when to stop, when to start
	void moveCtrl( void );
	//reset the flag variables for sepecial event, so that the current event can work well
	void variableReset( void );
	//translate points to do Y rotation, and revise normal vectors for each face, so that they will display well when doing rotation
	void yRotationPointTrans( void );
	//draw normal vectors
	void drawNormalVectors( void );
	//draw bounding box
	void drawBoundingBox( void );

2. How my program works:
	1). When you start it, it will display the first ply file in the screen
	2). "N" display next ply file, "P" display previous one. [the previous file 0f "0" is "42", and the next one of "42" is "0"] 
	3). Anytime when you press "W", it will display the first ply file (reset function)
	4). When "X/x", "Y/y", "Z/z" events are working, you can stop current moving by pressing any key as "X/x", "Y/y", "Z/z" 
	5). Also, you can stop "B", "R" by pressing the same key twice (avoiding pressing "B" when "R" is working, even it works in my program, but it does not look so beautiful, vice versa)
	6). "e", and "m" can work any time you press them. no matter what is the current event 
	7). When "R" is current event, "X/x", "Y/y", "Z/z" won't work
	8). [Please do not interrupt the idle event with other one before it stops. Even it can work, the shape of its display may make you not so comfortable =)]
	9). If you have any question, you can contact me: hzhou@wpi.edu