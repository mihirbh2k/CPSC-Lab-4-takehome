/*
 File:          mazesolver.c
 Purpose:       solves mazes
 Author:		Stephanie Quon and Mihir Bhatia
 Student #s:	95122271 and 76149921
 CS Accounts:	k5d3b and mbhatia01
 Date:			November 5, 2020
 */
 
/******************************************************************
 PLEASE EDIT THIS FILE AND INCLUDE IN YOUR GRADESCOPE SUBMISSION

 Comments that start with // should be replaced with code
 Comments that are surrounded by * are hints
 ******************************************************************/

#define _CRT_SECURE_NO_WARNINGS

/* Preprocessor directives to include libraries */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mazesolver.h"

/*
 Contains the majority of the maze solver logic
 PRE:       The file specified by the MACRO in lines 47 and 60 exists in the project
 POST:      Prints data about maze solutions.
 RETURN:    None
 */
void process()
{
  // Variables 
  int dimension = 0;
  FILE* maze_file = NULL;
  maze_cell** maze = NULL;

  // Holds outputs for the paths
  char outputstring[BUFFER] = "\0";
  char shortoutputstring[BUFFER] = "\0";

  // Declare variables for holding generated path information 
  char** paths = NULL;
  int paths_found = 0;
  int start_row = 0;
  int start_col = 0;

  // Opens and parses the maze file.
  maze_file = fopen(MAZE3877, "r"); 

  // get size of the maze and copy into memory
  if (maze_file) {
	  dimension = get_maze_dimension(maze_file); 
	  maze = parse_maze(maze_file, dimension); 
  }
  // error message if maze file is not parsable
  else { 
    fprintf(stderr, "Unable to parse maze file: %s\n", MAZE3877);
    system("pause");
  }

  // Traverses maze and generates all solutions 
  generate_all_paths(&paths, &paths_found, maze, dimension, start_row, start_col, "");
  printf("Total number of solutions: %d\n", paths_found);

  // Calculates shortest path 
  construct_shortest_path_info(paths, paths_found, shortoutputstring);
  printf("%s\n", shortoutputstring);

  // Calculates the cheapest path
  construct_cheapest_path_info(paths, paths_found, outputstring);
  printf("%s\n", outputstring);

  // free allocated memory from output strings
  free(&shortoutputstring);
  free(&outputstring);

  return;
}

/*
 Acquires and returns the maze size.  Since the maze is always a square, all we
 need to do is find the length of the top row!
 PARM:      maze_file is a pointer to a filestream
 PRE:       maze_file is an initialized pointer to a correctly-formatted maze file
 POST:      maze_file's internal pointer is set to beginning of stream
 RETURN:    length of the first line of text in the maze file EXCLUDING any EOL characters
            ('\n' or '\r') and EXCLUDING the string-terminating null ('\0') character.
 */
int get_maze_dimension( FILE* maze_file )  {
  int  dimension = 0;
  char line_buffer[BUFFER];

  // get dimension of the maze
  dimension = strlen( fgets ( line_buffer, BUFFER, maze_file ) );
  // return pointer to the top
  fseek( maze_file, 0, SEEK_SET );

  // Checks if text file was created in Windows and contains '\r' 
  if ( strchr( line_buffer, '\r' ) != NULL ) 
	  return (dimension - 2); // omit '\r' and '\n' 
  else 
	  return (dimension - 1); // omit '\n'
}

/* 
 Parses and stores maze as a 2D array of maze_cell.  This requires a few steps:
 1) Allocating memory for a 2D array of maze_cell, e.g., maze_cell[rows][columns]
    a) Dynamically allocates memory for 'dimension' pointers to maze_cell, and assign
	   the memory (case as a double pointer to maze_cell) to maze, which is a
	   double pointer to maze_cell (this makes the maze[rows] headers)
	b) For each row of the maze, dynamically allocate memory for 'dimension' maze_cells
	   and assign it (cast as a pointer to maze_cell) to maze[row]
 2) Copying the file to the allocated space
    a) For each row obtained from the file using fgets and temporarily stored in line_buffer
	   i) For each of the 'dimension' columns in that row
	   ii)Assign the character from the file to the struct, and set the struct to unvisited
 3) Returns the double pointer called maze.
 PARAM:  maze_file pointer to an open filestream
 PARAM:  dimension pointer to an int
 PRE:    maze_file is a pointer to a correctly-formatted maze file
 POST:   dimension contains the correct size of the square maze
 POST:   maze contains a dynamically allocated copy of the maze stored in the maze_file
 RETURN: maze, a maze_cell double pointer, which points to 'dimension' single pointers
         to maze_cell, each of which points to 'dimension' maze_cell structs.
 */
maze_cell** parse_maze( FILE* maze_file, int dimension )
{
  /* Variables */
  char line_buffer[BUFFER];
  int row = 0;
  int column = 0;
  maze_cell** maze = NULL; 

  /* Allocates memory for correctly-sized maze */
  maze = (maze_cell**) calloc(dimension, sizeof(maze_cell)*dimension);

  /* Allocates memory for an array of rows */
  for ( row = 0; row < dimension; ++row ) {
     maze[row] = (maze_cell*) calloc(dimension, sizeof(maze_cell)*dimension);
  }

  /* Copies maze file to memory */
  row = 0;
  while ( fgets ( line_buffer, BUFFER, maze_file ) ) {
    for ( column = 0; column < dimension; ++column ) {
		maze[row][column].character = line_buffer[column];
		maze[row][column].visited = UNVISITED;
	  }
    row++;
  }

  /* return maze double pointer */
  return maze;
}

/**
 Generates all paths through a maze recursively.
 PARAM:     pointer to a 2D array of all generated paths
            (pass the 2D array by reference - actual parameter to be modified by function)
 PARAM:     pointer to number of paths found
            (pass the integer by reference - actual parameter to be modified by function)
 PARAM:     pointer to a 2D array of maze_cell
 PARAM:     dimension of the square maze
 PARAM:     starting position row
 PARAM:     starting position column
 PARAM:     pointer to a string of chars which contains the 'current' path
 PRE:       maze contains a representation of the maze
 PRE:       dimension contains the correct dimension of the maze
 PRE:       row contains a valid row coordinate
 PRE:       column contains a valid column coordinate
 PRE:       path contains a sequence of characters (the first time you invoke this
            function, the passed parameter should be an empty string, e.g., "" (not NULL)
 POST:      IF current coordinate is not out of maze bounds &&
               current coordinate is not a wall
            THEN path contains current coordinate
 POST:      IF current coordinate is at maze finish line (right boundary)
            THEN paths contains the path from start to finish.
 POST:      dereferenced pathset contains all paths found
 POST:      dereferences numpaths contains the number of paths found
 */
void generate_all_paths(char*** pathsetref, int* numpathsref, maze_cell** maze, int dimension, int row, int column, char* path)
{
	/* Variables */
	int path_length = 0;
	char* new_point = NULL;
	char* new_path = NULL;

	/* Checks for base cases */
	if (row < 0 || column < 0 || row > (dimension - 1)  || column > (dimension - 1) || maze[row][column].character == MAZE_WALL || maze[row][column].visited == VISITED)
		 return;

  /* Otherwise deals with the recursive case.  Pushes the current coordinate onto the path
	  and checks to see if the right boundary of the maze has been reached
	  IF   right boundary reached
	  THEN path is added to the list as a successful path and function returns
	  ELSE the current location is marked as used and the search continues
	      in each cardinal direction using a recursive call using these steps:
		1. get length of path
		2. allocate space for a larger new path
		3. allocate space for a new point to add to the path
		4. assign the value in the maze cell to the new point
		5. concatenate old path to new path
		6. concatenate new point to new path */	
	else {
	  path_length = strlen( path );
	  new_path = ( char * ) calloc( path_length + 2, sizeof( char ) );
	  new_point = ( char * ) calloc( 2, sizeof( char ) );
	  new_point[0] = maze[row][column].character;

	  if ( path_length )
		  new_path = strcat( new_path, path );

	  new_path = strcat( new_path, new_point );
      free(new_point);

	  if ( column == ( dimension - 1 ) ) {
	    /* 1. Reallocate memory in global paths array to make room
			    for a new solution string
			 2. Copy the solution path to the location of new string
			 3. Increment paths counter */
	    *pathsetref = ( char** ) realloc ( *pathsetref, ( (*numpathsref) + 1 ) * sizeof( char* ) );
        (*pathsetref)[*numpathsref] = ( char* ) calloc( strlen( new_path ) + 1, sizeof( char ));
	    strcpy( (*pathsetref)[*numpathsref], new_path );
	    (*numpathsref)++;
      return;
	  } 

	  else {

      // Mark point as visited
      maze[row][column].visited = VISITED;

	  //  Recursively search in each direction using the new_path, and the same pathsetref and numpathsref parameters
	  generate_all_paths(pathsetref, numpathsref, maze, dimension, row + 1, column, new_path);
	  generate_all_paths(pathsetref, numpathsref, maze, dimension, row - 1, column, new_path);
	  generate_all_paths(pathsetref, numpathsref, maze, dimension, row, column + 1, new_path);
	  generate_all_paths(pathsetref, numpathsref, maze, dimension, row, column - 1, new_path);

	  // Mark point as unvisited when bouncing back
	  maze[row][column].visited = UNVISITED;

	   return;

       }

     }
}

/* 
 Calculates the cost for a given path.
 Examples:
 ""    -> 0
 "2"   -> 2
 "871" -> 16
 PARM:   path_string is a pointer to an array of char
 PRE:    path_string is a pointer to a null-terminated array of char (a string)
 RETURN: the 'cost' of the path.
 */
int path_cost ( char* path_string )
{
  // since chars are being added, their integer value is not added, instead their ascii decimal value is being added
  // in the ascii table, 0 starts at 48, so there is an offset of 48
  int ascii_offset = 48;

  // base case: when the char in the string is null, the string is now empty and cost is zero
  if (*path_string == '\0')
	  return 0;

  // recursive: iterate through the string to add all values in the array
  else 
	  return *path_string - ascii_offset + path_cost(path_string + 1);
}

/*
 Writes the shortest path in the paths global variable into the outputbuffer parameter,
 where the shortest path has the fewest cells.
 In the event of a tie, use the first 'shortest' path found.
 Uses this format:
 "Shortest path: XXXXXXXX"
 Don't forget to add a null character at the end.
 PARAM: 2D array containing all paths found
 PARAM: number of paths in pathset
 PARAM: outputbuffer is a character array of sufficient length
 POST:  outputbuffer contains information about the shortest path
 */
void construct_shortest_path_info ( char** pathset, int numpaths, char* outputbuffer )
{
	int i;
	int shortest = 0;

	// if only one path, that path is automatically the shortest
	if (numpaths == 1) {
		// concatenate path to the output buffer
		strcat(outputbuffer, "Shortest path: ");
		strcat(outputbuffer, *pathset);
		strcat(outputbuffer, "\0");
	}

	else {
		// iterate through all paths, comparing lengths to find the shortest one
		for (i = 0; i < numpaths; i++) {
			if (strlen(*(pathset + i)) < strlen(*(pathset + shortest)))
				shortest = i;
		}

		// concatenate shortest path to the output buffer
		strcat(outputbuffer, "Shortest path: ");
		strcat(outputbuffer, *(pathset + shortest));
		strcat(outputbuffer, "\0");
	}

	return;
}

/*
 Writes the cheapest path in the paths global variable into the outputbuffer parameter,
 where the cheapest path has the lowest sum value of its cells.
 In the event of a tie, use the first 'cheapest' path found.
 Uses this format:
 "Cheapest path: XXXXXXXX
  Cheapest path cost: 9999"
 Don't forget to use a newline and to add a null character at the end.
 Use sprintf to put an integer into a buffer which can then be concatenated using strcat.
 PARAM: 2D array containing all paths found
 PARAM: number of paths in pathset
 PARAM: outputbuffer is a character array of sufficient length
 POST:  outputbuffer contains information about the cheapest path and its cost
 */
void construct_cheapest_path_info ( char** pathset, int numpaths, char* outputbuffer )
{
	int i, cost;
	int cheapest = path_cost(*pathset);
	int cheapest_index = 0;
	char* cost_holder = (char*) malloc(sizeof(char));

	// if only one path, that path is automatically the cheapest
	if (numpaths == 1) {
		// concatenate path to the output buffer
		strcat(outputbuffer, "Cheapest path: ");
		strcat(outputbuffer, *pathset);
		strcat(outputbuffer, "\n");

		// calculate path cost
		cost = path_cost(*pathset);
		sprintf(cost_holder, "%d", cost);

		// concatenate the the path cost to the string
		strcat(outputbuffer, "Cheapest path cost: ");
		strcat(outputbuffer, cost_holder);
		strcat(outputbuffer, "\0");

		return;
	}

	else {
		// iterate through all paths, finding the smallest cost
		for (i = 0; i < numpaths; i++) {

			cost = path_cost(*(pathset + i));

			// if the found cost is currently the cheapest, make it the new cheapest value
			if (cost < cheapest) {
				cheapest_index = i;
				cheapest = cost;
			}

		}

		// concatenate cheapest path to the output buffer
		strcat(outputbuffer, "Cheapest path: ");
		strcat(outputbuffer, *(pathset + cheapest_index));
		strcat(outputbuffer, "\n");

		// calculate path cost
		cost = path_cost(*(pathset + cheapest_index));
		sprintf(cost_holder, "%d", cost);

		// concatenate the the path cost to the string
		strcat(outputbuffer, "Cheapest path cost: ");
		strcat(outputbuffer, cost_holder);
		strcat(outputbuffer, "\0");

		return;
	}

}

