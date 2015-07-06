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


To compile, use "make". Then run:
./maze maze.in maze.out

View your "maze.out" to see the shortest answer to the maze.

Some mazes may not have an answer due to no possible path or due to no start and/or finish spot.
