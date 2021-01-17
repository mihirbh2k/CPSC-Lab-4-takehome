/*
 File:          mazesolver.h
 Purpose:       Contains constants, prototypes, globals
 Author:		Stephanie Quon and Mihir Bhatia
 Student #s:	95122271 and 76149921
 CS Accounts:	k5d3b and mbhatia01
 Date:			November 5, 2020
 */

/******************************************************************
 PLEASE EDIT THIS FILE AND INCLUDE IN YOUR GRADESCOPE SUBMISSION

 If you add any constants define them in the list of #define
 statements.

 If you add any functions to the mazesolver.c file, add their prototypes
 to the list of prototypes.
 ******************************************************************/

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Preprocessor directives to define macros */
#define MAZE1     "maze1.txt"
#define MAZE2     "maze2.txt"
#define MAZE3     "maze3.txt"
#define MAZE119   "maze119.txt"
#define MAZE3877  "maze3877.txt"
#define MAZE_WALL '*'
#define VISITED   'Y'
#define UNVISITED 'N'
#define BUFFER    128

/* Structure used as the cell for the maze representation */
typedef struct maze_cell {
  char character;
  char visited;
} maze_cell; /* We're using a little trick here.  Instead of writing struct maze_cell everywhere
                in our code, we're writing maze_cell.  We can do this because at the end of our
                struct maze_cell definition, we've tacked on the short form of the struct name.
				When we do this, the C standard lets us use maze_cell instead of struct maze_cell.  */

/* Function prototypes */
// void         print_generated_paths ( char** pathset, int numpaths ); /* not implemented, can be used for debugging if needed */
maze_cell**  parse_maze            ( FILE* maze_file, int dimension );
int          get_maze_dimension    ( FILE* maze_file );
void         generate_all_paths    ( char*** pathsetref, int* numpathsref, maze_cell** maze, int dimension, int row, int column, char* path );
int          path_cost             ( char* path_string );
void	     construct_shortest_path_info ( char** pathset, int numpaths, char* outputbuffer );
void		 construct_cheapest_path_info ( char** pathset, int numpaths, char* outputbuffer );
void         process               ( );