#include <iostream>
#include "maze_io.h"
using namespace std;

/*****************************************************************
This program uses breadth-first search to find not only a path from start to finish in a maze, but the SHORTEST path. To do so, we keep track of a bfsq and where we've been.

All the mazes are in the "mazes" directory in the present working directory. There are 7 mazes:

maze1.in
maze2.in
maze3.in
...
maze7.in

You can open up the mazes in a text editor (like gedit). Here's the legend:

The first row of two numbers are parsed by the code in "maze_io.cpp". The first number refers to the number of rows and the second number refers to the number of columns.

S	The starting point of the maze
F	The finish point of the maze
#	Walls/impassable areas
.	passable areas

Usage: ./maze maze.in maze.out

View your "maze.out" to see the shortest answer to the maze.

Some mazes may not have an answer due to no possible path or due to no start and/or finish spot.
*****************************************************************/



// Prototype for maze_search
int maze_search(char **, int, int);

int main(int argc, char *argv[])
{
  int rows, cols, result;

  if(argc < 3){
    cerr << "Usage: ./maze in_filename out_filename" << endl;
    return 1;
  }

  char** mymaze = read_maze(argv[1], &rows, &cols);

  // For checkpoint 1, just leave maze_search() unedited
  //  and the program should read in the maze, print it
  //  and write it back to the output file
  result = maze_search(mymaze, rows, cols);

  if( result == 1 ){
    cout << "Path successfully found!" << endl;
    print_maze(mymaze,rows,cols);
    write_maze(argv[2],mymaze,rows,cols);
  }
  else if (result == 0) {
    cout << "No path could be found!" << endl;
  }
  else {
    cout << "Error occurred during search!" << endl;
  }

  for (int i = 0; i < rows; i++) //deallocate memory to mymaze.
  {
	delete [] mymaze[i];
  }
  delete [] mymaze;

  return 0;
}

/**************************************************
 * Attempt to find shortest path and return:
 *  1 if successful
 *  0 if no path exists
 * -1 if an error occurred (no start of finish
 *     cell present in the maze)
 *
 * If path is found fill it in with '*' characters
 *  but don't overwrite the 'S' and 'F' cells
 *************************************************/
int maze_search(char **maze, int rows, int cols)
{

//INITIALIZING VARIABLES AND DATA STRUCTURES
	int max_queue_size = rows * cols;  

	int** bfsq = new int*[max_queue_size]; //dynamically allocate bfsq array
	for (int i = 0; i < max_queue_size; i++)
	{
		bfsq[i] = new int[2];
	}

    bool visited[rows][cols]; //initialize visited 2D array to all falses
	for (int row = 0; row < rows; row++)
	{
		for (int col = 0; col < cols; col++)
		{
			visited[row][col] = false;
		}
	}

    bool start_found = false; //variable to indicate if start exists
    bool finish_found = false; //variable to indicate if finish exists

    int start_row, start_col, finish_row, finish_col;

    int backtrace; //to be used later when finish is inserted into the bfsq

    int* pred = new int[max_queue_size]; //predecessor array

    int head = 0; //initialize head and tail to 0.
    int tail = 0;

//SEARCH FOR START AND FINISH
    for (int row = 0; row < rows; row++)
    {
      for (int col = 0; col < cols; col++)
      {
         if (maze[row][col] == 'S') //if we find start
         {
	    start_row = row;
	    start_col = col;
            bfsq[0][0] = row; 
	    bfsq[0][1] = col;
            visited[row][col] = true;
            start_found = true;
            pred[0] = -1;
            tail++; //increment tail since we have start at index 0.
         }
         if (maze[row][col] == 'F') //if we find finish
         {
            finish_row = row;
			finish_col = col;
            finish_found = true;
         }
      }
   }

//CHECK IF START AND FINISH EXIST, IF NOT RETURN ERROR
   if (start_found == false || finish_found == false)
   {
		for (int i = 0; i < max_queue_size; i++) //deallocate memory
		{
			delete [] bfsq[i];
		}

	   delete [] bfsq;
	   delete [] pred;

      return -1;
   }

//BEGIN BREADTH-FIRST-SEARCH

	//FIND NEIGHBORS
    while (tail > head && visited[finish_row][finish_col] != true)
	//if our bfsq isn't empty and we haven't found the finish as a neighbor
    {
		int neighbors[4][2]; //array for neighbors of current bfsq[head]
		int modifier[8] = {-1, 0, 0, -1, 1, 0, 0, 1}; //for convenience
		for (int i = 0; i < 4; i++)
		{
			for (int rowcol = 0; rowcol < 2; rowcol++)
			{
			neighbors[i][rowcol] = bfsq[head][rowcol] + modifier[2*i + rowcol];
			//using bfsq[head] and modifier to easily create the
			//neighbors to bfsq[head] using a for loop
			//every even entry of modifier is a modifier for row of bfsq[head]
			//every odd entry of modifier is a modifier for col of bfsq[head]
			}
		}

	//ADD NEIGHBORS TO BFSQ
	for (int i = 0; i < 4; i++)
	{
		int row = neighbors[i][0];
		int col = neighbors[i][1];		
		if (row < rows && row >= 0 && col < cols && col >= 0)
		{
    		    if (maze[row][col] == '.' && visited[row][col] != true)
				//if neighbor is valid path and not visited
      		    {
			  visited[row][col] = true; //we visited it
         		  bfsq[tail][0] = row; //add row to bfsq
			  bfsq[tail][1] = col; //add col to bfsq
         		  pred[tail] = head; //add predecessor of neighbor
         		  tail++;
    		    }
      		    if (maze[row][col] == 'F')
		    //if the neighbor is the finish
      		    {
       		    visited[row][col] = true; //we visited the finish
		    bfsq[tail][0] = row; //add it to bfsq
		    bfsq[tail][1] = col; 
		    pred[tail] = head;
        	    backtrace = tail; //backtrace is the bfsq index of the finish
		    tail++;
       		   }
		 }
      }
      head++; //look at next item in bfsq, and go find the neighbors
   }


//BACKTRACE FROM FINISH
   if (visited[finish_row][finish_col] == true)
   {   
	while (bfsq[backtrace][0] != start_row || bfsq[backtrace][1] != start_col)
		//if the backtraced item isn't the start position
        {
          int row = bfsq[pred[backtrace]][0];
          int col = bfsq[pred[backtrace]][1];
		  if (maze[row][col] != 'S')
		  {
         		maze[row][col] = '*'; //place a '*' at the location
		  }
         		backtrace = pred[backtrace]; 
			//our new index is the predecessor of the
			//current backtraced item in the bfsq
		          
		}

		//once we arrive at the start

		for (int i = 0; i < max_queue_size; i++) //deallocate dynamic memory
		{
			delete [] bfsq[i];
		}

	   delete [] bfsq;
	   delete [] pred;

       return 1;
   }

//IF FINISH WASN'T A NEIGHBOR TO A POSSIBLE PATH
   for (int i = 0; i < max_queue_size; i++) //deallocate dynamic memory
		{
			delete [] bfsq[i];
		}

	
   delete [] bfsq;
   delete [] pred;

return 0;
}

