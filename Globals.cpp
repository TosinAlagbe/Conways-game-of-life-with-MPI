#include "Globals.h"

//---------initial definition of global varaibles------------------

//processor id(id) and number of processes(p)
int id, p;
//neighbor processor id for 8 neighbors
int up_p, down_p, left_p, right_p;
int t_left_p, b_left_p, t_right_p, b_right_p;
//Mpi communicator for the decomposed grid
MPI_Comm comm_2d;

//-------------------------------------------------------------
