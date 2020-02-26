#ifndef GLOBALS_H
#define GLOBALS_H

#include <mpi.h>

//---------Global variables----------------------------------

//Global variables to be passed between different functions
//Not so good practice but suitable for this problem

//processor id(id) and number of processes(p)
extern int id, p;
//neighbor processor id for 8 neighbors
extern int up_p, down_p, left_p, right_p;
extern int t_left_p, b_left_p, t_right_p, b_right_p;
//Mpi communicator for the decomposed grid
extern MPI_Comm comm_2d;

//-------End of Global variables--------------------------


#endif // ! GLOBALS_H