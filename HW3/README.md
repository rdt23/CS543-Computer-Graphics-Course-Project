1. How my program works:
	1). When you start it, it will display the first tree in the screen[a small bug: the tree will be displayed, then clear, then be displayed again.]
	2). Keyboard events "a", "b", "c", "d", "e" displays the 5 trees respectively. 
	3). Keyboard events "r" displays a random tree. 
	4). Keyboard events "f" displays my park with two cars(one for adult, and one for child). 
	5). ATTENTION: I draw 13 trees in my park, so it will cost about 5 seconds, please be patient. Thanks! :-) 
	6). If you have any question, you can contact me: hzhou@wpi.edu

2. Description for each function in my source code:
	//default function
	void generateGeometry( void );

	//default display function for the whole program
	void display( void );

	//deal with keyboard issue
	void keyboard( unsigned char, int, int );

	//load ply file into array based on file index
	void plyFileLoad(int);

	//load L-systems file into memory
	void readFile(int);

	//extend the formula from L-systems file, and store it into a link list 
	void do_iteration(int);

	//return the length of the above link list (test use only)
	int get_count( void );

	// normalize cylinder and sphere, so that they have the same diameter, and the length of cylinder will be 1 (also change the start point of cylinder to the buttom).
	void normalize( void );

	// draw different trees based on tree index
	void drawTree( int );

	// a function used to generate random float number from pre-determined range
	float RandomNumber(float , float );

	// draw sphere
	void drawSphere( void );

	// draw cylinder
	void drawCylinder( void );

	// flush, so that the tree will displayed into screen
	void flush( void );

	// draw forest
	void drawForest( void );

	// draw ground plane
	void drawGround( void );

	// load point into array to draw ground
	void loadGround( void );

	// draw a car for adult
	void drawCar( void );

	// draw a cow, but it doesn't work in my program(not used)
	void drawCow( void );

	// draw a small car for child
	void drawSmallCar( void );

