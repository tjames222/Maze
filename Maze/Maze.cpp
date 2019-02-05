// Maze.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Author: Tim James
// Course: CST-201
// Date: 1/14/19
// Professor: Bass
// Description: Maze program - Displays a maze to the console based on input from user. Writes a maze to a file based on user input.
// Program was designed by Tim James and inspired by professor Brandon Bass' code.

#include "pch.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <list>
#include <stack>
#include <queue>
#include "Maze.h"

using namespace std;



////////////////////////////////////////////////////////////////    GLOBAL VARIABLES    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

// Declare variables for maze
// Start row and start column
int gStartRow;
int gStartCol;

// Row size and column size
int gRowSize;
int gColSize;

// Wall Row and column
int gWallRow;
int gWallCol;

// End row and column
int gEndRow;
int gEndCol;



////////////////////////////////////////////////////////////////    ERROR TESTING    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

// Checks for input error from file for start, end, and size input
int CheckStartData() {
	if (gStartRow < 0 || gStartRow > gRowSize || gColSize < 0 ||
		gRowSize < 0 || gStartCol < 0 || gStartCol > gColSize ||
		gEndCol < 0 || gEndCol > gColSize || gEndRow < 0 || gEndRow > gRowSize) {
		cout << "Error!" << " File contents invalid." << endl
			<< "*** Check start, end, and size input data. ***" << endl;

		return 1;
	}
}

// Checks for input error from file for wall input
int CheckFileIn() {
	if (gWallCol < 0 || gWallCol > gColSize || gWallRow < 0 || gWallRow > gRowSize) {
		cout << "Error!" << " File contents invalid." << endl
			<< endl << "*** Check wall input data. ***" << endl;

		return 1;
	}
}


////////////////////////////////////////////////////////////////    STRUCT DECLARATIONS    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

struct MiniPath {
	int x1, y1, x2, y2, length;
};


////////////////////////////////////////////////////////////////    CLASS DECLARATIONS    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

class Group {

	// Declaring variables to be used
	protected: char contArray[3][3]; // used for default constructor to be loaded
	
	// Start, end, and blocked booleans 
	protected: bool start;
	protected: bool end;
	protected: bool blocked;
	public: bool visited;

	// Direction for movement
	public: int dir;

	// Current Location
	public: int x_Location; // x location
	public: int y_Location; // y location
			   

	// Creates constructor of blank 3x3 group
	public: Group() {

		// Fills the contArray with the default start character '.'
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				
				contArray[i][j] = '.';
			}
		}
		
		visited = false;
		start = false;
		end = false;
		blocked = false;
				
		//x_Location = 0;
		//y_Location = 0;
		//dir = 0;
	}

	// overloaded constructor that accepts a char to add to Group (index 1,1)
	public:	Group(char brand) {

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				// Checks to see if a wall exists and assigns 'X'
				if (brand == 'X' || brand == 'x') { // Converts to all Caps

					// Fills the contArray with the wall char
					contArray[i][j] = 'X'; 

					// Setting blocked to true
					blocked = true; 
				}
				// if no wall assign '.'
				else { 

					// Fills contArray with default char if not a wall
					contArray[i][j] = '.'; 
				}
			}
		}
		
		// Start position assignment
		if (brand == 'S' || brand == 's') { // Converts to all Caps

			// Sets Start position for contArray coordinates
			contArray[1][1] = 'S'; 

			start = true; 
		}
		// End possition assignment
		else if (brand == 'E' || brand == 'e') { // Converts to all Caps

			// Sets End position for contArray coordinates
			contArray[1][1] = 'E'; 

			end = true; 
		}

		visited = false;
	/*	x_Location = 0;
		y_Location = 0;
		dir = 0;*/
	}

	// Returns values
	char ReturnBrand(int row, int col) {
		return contArray[row][col];
	}

	// Sets the Brand 
	public:	void SetBrand(char brand) {

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				// Checks to see if a wall exists and assigns 'X'
				if (brand == 'X' || brand == 'x') { // Converts to all Caps

					// Fills the contArray with the wall char
					contArray[i][j] = 'X';

					// Setting blocked to true
					blocked = true;
				}
				// if no wall assign '.'
				else { 

					// Fills contArray with default char if not a wall
					contArray[i][j] = '.';
				}
			}
		}
		// Start position assignment
		if (brand == 'S' || brand == 's') { // Converts to all Caps

			// Sets Start position for contArray coordinates
			contArray[1][1] = 'S';

			start = true;
		}
		// End possition assignment
		else if (brand == 'E' || brand == 'e') { // Converts to all Caps

			// Sets End position for contArray coordinates
			contArray[1][1] = 'E';

			end = true;
		}

		visited = false;
		//x_Location = 0;
		//y_Location = 0;
		//dir = 0;
	}
};

// Used for traversing through the maze
class MazeWalk {
public:
	int col; //X
	int row; //Y
	char dir;
	bool up, down, left, right;
	//list<list<Group>> *maze;

	MazeWalk(int i, int j) {
		col = i;
		row = j;
		dir = 0;
		up, down, left, right = false;
	}
};

// The current position and movements function
class MazeCursor {

public:
	list<list<Group>>::iterator it_R;
	list<Group>::iterator it_C;
	list<list<Group>> *maze;
	int r, c;
	int colSize, rowSize;
	bool wallCollision;

	// Overloaded constructor for MazeCursor. Collision is used to turn on or off wall use
	MazeCursor(list<list<Group>>& mazeBucket, int width, int hieght, bool collision) {
		
		maze = &mazeBucket;
		it_R = maze->begin();
		it_C = it_R->begin();
		r = 0;
		c = 0;
		colSize = width;
		rowSize = hieght;
		wallCollision = collision;
	}

	// Sets the starting position for the iterators
	void SetPos(int x, int y) {
		c = x;
		r = y;
		it_R = maze->begin();
		for (int i = 0; i < r; i++) it_R++;
		it_C = it_R->begin();
		for (int j = 0; j < c; j++) it_C++;
	}

	//MazeWalk GetMazeWalk() {

	//	temp.up = checkPos.MoveUp();
	//	checkPos.SetPos(c, r);
	//	temp.down = checkPos.MoveDown();
	//	checkPos.SetPos(c, r);
	//	temp.left = checkPos.MoveLeft();
	//	checkPos.SetPos(c, r);
	//	temp.right = checkPos.MoveRight();
	//	checkPos.SetPos(c, r);
	//}

	// Move up function
	bool MoveUp() {

		// Checking boundaries of maze
		if (r - 1 < 0) {

			cout << "Stay within bounds" << endl;
			return false;
		}

		// Move up
		it_R--;
		r--;
		it_C = it_R->begin();
		for (int j = 0; j < c; j++) it_C++;
		
		// If wallCollision is on and it is a wall go back down and return false
		if ((it_C->ReturnBrand(1, 1) == 'X' || it_C->visited) && wallCollision) {

			cout << "Up: That's a wall!" << endl;
			it_R++;
			r++;
			it_C = it_R->begin();
			for (int j = 0; j < c; j++) it_C++;
			return false;
		}
		return true;
	}

	bool MoveDown() {

		// Checking boundaries of maze
		if (r + 1 > rowSize - 1) {

			cout << "Stay within bounds" << endl;
			return false;
		}

		// Move down
		it_R++;
		r++;
		it_C = it_R->begin();
		for (int j = 0; j < c; j++) it_C++;
		
		// If wallCollision is on and it is a wall go back up and return false
		if ((it_C->ReturnBrand(1, 1) == 'X' || it_C->visited) && wallCollision) {

			cout << "Down: That's a wall!" << endl;
			it_R--;
			r--;
			it_C = it_R->begin();
			for (int j = 0; j < c; j++) it_C++;
			return false;
		}
		return true;
	}

	bool MoveLeft() {

		// Checking boundaries of maze
		if (c - 1 < 0) {

			cout << "Stay within bounds" << endl;
			return false;
		}

		// Move left
		it_C--;
		c--;

		// If wallCollision is on and it is a wall go right and return false
		if ((it_C->ReturnBrand(1, 1) == 'X' || it_C->visited) && wallCollision) {

			cout << "Left: That's a wall!" << endl;
			it_C++;
			c++;
			return false;
		}
		return true;
	}

	bool MoveRight() {

		if (c + 1 > colSize - 1) {

			cout << "Stay within bounds" << endl;
			return false;
		}

		// Move right
		it_C++;
		c++;

		// If wallCollision is on and it is a wall go left and return false
		if ((it_C->ReturnBrand(1, 1) == 'X' || it_C->visited) && wallCollision) {

			cout << "Right: That's a wall!" << endl;
			it_C--;
			c--;
			return false;
		}
		return true;
	}
};



////////////////////////////////////////////////////////////////    PRINT FUNCTIONS    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

// Writes the maze to a file
void WriteMaze(list<list<Group>> mazeBucket, int rowSize, int colSize) {

	// Declaring iterators
	list<list<Group>>::iterator it_R = mazeBucket.begin();
	list<Group>::iterator it_C = it_R->begin();

	// Declares new ofstream variable
	ofstream fileOut;

	// Opens a file to be written to 
	fileOut.open("C:\\Users\\mrtim\\Documents\\GCU\\CST-201\\WEEK 1\\Maze project 1\\fileOut.txt");

	// Used for Header Columns
	char header[] = { 'A', 'B', 'C', 'D', 'E', 'F', 
		'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 
		'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 
		'Y', 'Z' };

	cout << "   "; // used for spacing between headers

	// Header Letter spacing
	for (int i = 0; i < colSize; i++) {

			fileOut << "     " << header[i] << "";
	}

	// Formating
	fileOut << endl;

	// Row headers and loads the array with the maze data
	for (int i = 0; i < rowSize; i++) {
		for (int m = 0; m < 3; m++) {

			// Row number added to left header column
			if (m == 1) {
				if (i < 10) { 

					fileOut << i << "  "; // spacing for formating
				}
				else { 

					fileOut << i << " ";  // alternate spacing for formating
				}
			}
			else { 
				
				fileOut << "   "; 
			}

			for (int j = 0; j < colSize; j++) { // determines the amount of columns
				for (int n = 0; n < 3; n++) { // used for the loading of a Group object
					it_R = mazeBucket.begin(); // the iterator to select the row list
					for (int o = 0; o < i; o++) it_R++; // cycles through the row lists
					it_C = it_R->begin(); // sets the column iterator to the begining of the row list 
					for (int p = 0; p < j; p++) it_C++; // cycles through the column list 
					fileOut << it_C->ReturnBrand(m, n) << " "; // loads the group object at it_C location with appropriate brand and prints to file
				}
			}
			fileOut << endl; // adds a new line for next set of data
		}
	}
	fileOut.close(); // closes output file stream
}

// Prints the maze graphically to the console
void showMaze(list<list<Group>> mazeBucket, int rowSize, int colSize) {

	// Declaring iterators
	list<list<Group>>::iterator it_R = mazeBucket.begin();
	list<Group>::iterator it_C = it_R->begin();

	// Used for Header Columns
	char header[] = { 'A', 'B', 'C', 'D', 'E', 'F',
		'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
		'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
		'Y', 'Z' };

	// Formatting
	cout << endl;

	// Header Letter spacing
	for (int i = 0; i < colSize; i++) {

		cout << "     " << header[i];
	}

	cout << endl;

	// Row headers and loads the array with the maze data
	for (int i = 0; i < rowSize; i++) {
		for (int m = 0; m < 3; m++) {
			// Row number added to left header column
			if (m == 1) {
				if (i < 10) {

					cout << i << "  "; // spacing for formating
				}
				else {

					cout << i << " ";  // alternate spacing for formating
				}
			}
			else {

				cout << "   ";
			}
			
			// prints the maze
			for (int j = 0; j < colSize; j++) {	// determines the amount of columns
				for (int n = 0; n < 3; n++) { // used for the loading of a Group object
					it_R = mazeBucket.begin(); // the iterator to select the row list
					for (int o = 0; o < i; o++) it_R++; // cycles through the row lists
					it_C = it_R->begin(); // sets the column iterator to the begining of the row list 
					for (int p = 0; p < j; p++) it_C++; // cycles through the column list 
					cout << it_C->ReturnBrand(m, n) << " "; // loads the group object at it_C location with appropriate brand and prints to console
				}
			}
			cout << endl; // adds a new line for next set of data
		}	
	}
}

// Used for printing out coordinates of maze walking
void PrintDirection(int x, int y, string dir) {
	
	cout << " col: " << (char)('A' + x) << " row: " << y << " " << dir << endl;
}



////////////////////////////////////////////////////////////////    MAIN    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

int main() {

	// Variable for new input file stream
	ifstream fileIn;

	// open new file and read data
	fileIn.open("C:\\Users\\mrtim\\Documents\\GCU\\CST-201\\WEEK 1\\Maze project 1\\test.txt");

	//Check if file open is a success
	if (!fileIn) {
		cerr << "*** Unable to open file." << " Check file path. ***" << endl;
		exit(1); // system stop
	}

	// read input
	fileIn >> gColSize >> gRowSize;
	fileIn >> gStartRow >> gStartCol >> gEndRow >> gEndCol;

	// Calling the error function to action for start, end, and size input
	CheckStartData();

	// mazeBucket is a list of lists holding Group objects
	list<list<Group>> mazeBucket;

	// Creates the list of lists holding Group objects
	for (int i = 0; i < gRowSize; i++) {
		list<Group> newRow;
		mazeBucket.push_back(newRow); // creates new row list in the mazeBucket
		for (int j = 0; j < gColSize; j++) {
			Group newCell('.'); // default start character
			mazeBucket.back().push_back(newCell); // adds a Group object to the row list in the mazeBucket
		}
	}

	// instantiating iterators for col and row lists
	list<list<Group>>::iterator it_R = mazeBucket.begin();
	list<Group>::iterator it_C = it_R->begin();

	// brand as walls from input file data
	for (int h = 0; h < 79; h++) {
		fileIn >> gWallCol >> gWallRow;

		// iterates through the list row and col and loads a group object with the correct
		// brand based on input file coordinates
		it_R = mazeBucket.begin();
		for (int i = 0; i < gWallRow; i++) it_R++;
		it_C = it_R->begin();
		for (int j = 0; j < gWallCol; j++) it_C++;

		// Checks for errors with wall input
		CheckFileIn();

		// Loads wall brand
		it_C->SetBrand('X');
	}

	// iterates through the list row and col and loads a group object with the correct
	// brand based on input file coordinates
	it_R = mazeBucket.begin();
	for (int i = 0; i < gStartRow; i++) it_R++;
	it_C = it_R->begin();
	for (int j = 0; j < gStartCol; j++) it_C++;

	// Loads Start brand
	it_C->SetBrand('S');

	// This does what maze[startCol][startRow].SetBrand('S'); does 
	// iterates through the list row and col and loads a group object with the correct
	// brand based on input file coordinates
	it_R = mazeBucket.begin();
	for (int i = 0; i < gEndRow; i++) it_R++;
	it_C = it_R->begin();
	for (int j = 0; j < gEndCol; j++) it_C++;

	// Loads End brand
	it_C->SetBrand('E');

	// Writes the maze to a file
	WriteMaze(mazeBucket, gRowSize, gColSize);

	// Prints the maze to the console
	showMaze(mazeBucket, gRowSize, gColSize);
	cout << endl;

	// Press a key
	cout << endl << "Press <Enter> to continue..." << endl;
	cin.get();

	// Setting variables for use in maze walk
	int fc = gEndCol;
	int fr = gEndRow;

	int c = gStartCol;
	int r = gStartRow;

	// Used to make sure we don't try to go back the same direction we just went
	char lastDir = ' '; // U = Up, D = Down, L = Left, R = Right

	MazeCursor pos(mazeBucket, gColSize, gRowSize, true); // actual pos
	MazeCursor checkPos(mazeBucket, gColSize, gRowSize, false); // checks before it moves

	pos.SetPos(c, r); // sets the starting position for the iterators
	checkPos.SetPos(c, r);

	// Creating stack for backtracking
	//stack<MazeWalk> s;

	// Queue
	queue<MazeWalk> s;

	stack<MazeWalk> s1;

	MazeWalk temp(c, r);

	//temp.up = checkPos.MoveUp();
	//checkPos.SetPos(c, r);
	//temp.down = checkPos.MoveDown();
	//checkPos.SetPos(c, r);
	//temp.left = checkPos.MoveLeft();
	//checkPos.SetPos(c, r);
	//temp.right = checkPos.MoveRight();
	//checkPos.SetPos(c, r);

	s.push(temp);

	// used to set the new direction
	char nextDir = 'L';

	// backtracking
	bool backTracking = false;
	bool moved = false;

	// Used for debugging to eliminate an infinite loop
	int loopCount = 0; 
	int loopLimit = gColSize * gRowSize * 2000000;

	// Traversing through the maze until end is found or infinite loop is detected
	while (!((c == fc && r == fr) || (loopCount >= loopLimit))) {
		
		loopCount++;

		// Checking the Left direction. 
		if (nextDir == 'L' && lastDir != 'R') {

			// Moves left if no wall is encountered and adds temp1 to stack
			if (pos.MoveLeft()) {

				moved = true;

				nextDir = 'D'; // change the direction 
				//lastDir = 'L';
				MazeWalk temp1(--c, r);
				temp1.dir = 'L';
				s.push(temp1);
				s1.push(temp1);

				// Check if visisted is true and pop until new path is found
				//if (pos.it_C->visited) {

				//	backTracking = true;

				//	s1.pop();

				//	MazeWalk *temp = &s1.top();
				//	char whatever = temp->dir;
				//	if (whatever)
				//	nextDir = 'U';
				//	c = temp->col;
				//	r = temp->row;
				//	pos.SetPos(temp->col, temp->row);
				//	//cout << "Backtracking! (" << temp->col << ", " << temp->row << ")";
				//	PrintDirection(temp->col, temp->row, " Backtracking");
				//}
				//else {

				//	//flag visited
				//	pos.it_C->visited = true;
				//	backTracking = false;
					PrintDirection(temp1.col, temp1.row, " Left");
				//}
			}
			else {

				nextDir = 'U'; // change the direction 
			}
		}

		// Checking the Up direction 
		else if (nextDir == 'U' && lastDir != 'D') {

			// Moves Up if no wall is encountered and adds temp1 to stack
			if (pos.MoveUp()) {

				moved = true;

				nextDir = 'L';
				//lastDir = 'U';
				MazeWalk temp1(c, --r);
				temp1.dir = 'U';
				s.push(temp1);
				s1.push(temp1);
				
				// Check if visisted is true and pop until new path is found
				//if (pos.it_C->visited) {

				//	backTracking = true;

				//	s1.pop();
				//	MazeWalk *temp = &s1.top();
				//	pos.SetPos(temp->col, temp->row);
				//	//cout << "Backtracking! (" << temp->col << ", " << temp->row << ")";
				//	PrintDirection(temp->col, temp->row, " Backtracking");
				//}
				//else {

				//	//flag visited
				//	pos.it_C->visited = true;
				//	backTracking = false;
					PrintDirection(temp1.col, temp1.row, " Up");
				//}
				
			}
			else {

				nextDir = 'R'; // change the direction 
			}
		}

		// Checking the Right direction 
		else if (nextDir == 'R' && lastDir != 'L') {

			// Moves Right if no wall is encountered and adds temp1 to stack
			if (pos.MoveRight()) {
				
				moved = true;

				nextDir = 'U';
				//lastDir = 'R';
				MazeWalk temp1(++c, r);
				s.push(temp1);
				s1.push(temp1);
				
				// Check if visisted is true and pop until new path is found
				//if (pos.it_C->visited) {

				//	backTracking = true;

				//	s1.pop();
				//	MazeWalk *temp = &s1.top();
				//	pos.SetPos(temp->col, temp->row);
				//	//cout << "Backtracking! (" << temp->col << ", " << temp->row << ")";
				//	PrintDirection(temp->col, temp->row, " Backtracking");
				//}
				//else {

				//	//flag visited
				//	pos.it_C->visited = true;
				//	backTracking = false;
					PrintDirection(temp1.col, temp1.row, " Right");
				//}
				
			}
			else {
				
				nextDir = 'D'; // change the direction 
			}
		}

		// Checking the Down direction 
		else if (nextDir == 'D' && lastDir != 'U') {
			
			// Moves Down if no wall is encountered and adds temp1 to stack
			if (pos.MoveDown()) {

				moved = true;

				nextDir = 'R';
				//lastDir = 'D';
				MazeWalk temp1(c, ++r);
				s.push(temp1);
				s1.push(temp1);

				// Check if visisted is true and pop until new path is found
				//if (pos.it_C->visited) {

				//	backTracking = true;

				//	s1.top().dir;
				//	nextDir = 'R';

				//	s1.pop();
				//	MazeWalk *temp = &s1.top();
				//	pos.SetPos(temp->col, temp->row);
				//	//cout << "Backtracking! (" << temp->col << ", " << temp->row << ")";
				//	PrintDirection(temp->col, temp->row, " Backtracking");
				//}
				//else {

				//	//flag visited
				//	pos.it_C->visited = true;
				//	backTracking = false;
					PrintDirection(temp1.col, temp1.row, " Down");
				//}
				
			}
			else {

				nextDir = 'L'; // change the direction 
			}
		}
		if (!moved) {
			// Backtrack once and then check all directions again
			backTracking = true;
			s1.pop();
			MazeWalk *temp = &s1.top();
			pos.SetPos(temp->col, temp->row);
			nextDir = 'L';
			lastDir = ' ';

			PrintDirection(temp->col, temp->row, " Backtracking");
		}
		temp = s.front();
		
	}
	// Loop Max Reached!
	if (loopCount >= loopLimit) {

		cout << "I quit! You stink!" << endl;
	}
	// End found check
	if (c == fc && r == fr) {

		cout << endl;
		cout << "MAZE COMPLETED! " << endl;
	}

	cout << endl;
	cout << "PATH TAKEN: " << endl;

	MazeWalk *ptr;

	// Print the coordinates of path taken from stack
	while (!s.empty()) {
		for (int i = 0; i < 5 && !s.empty(); i++) {

			ptr = &s.front();
			cout << "(" << (char)('A' + ptr->col);
			cout << ", ";
			cout << ptr->row << ")" << ",  ";
			s.pop();
		}
		cout << endl;
	}

	// Close File
	fileIn.close();



	return 0;
}
