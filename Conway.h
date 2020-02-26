#ifndef CONWAY_H
#define CONWAY_H

#include "Border_buffers.h"
class Sub_grid;


namespace Conway {

	//2d input array flattened
	extern int* data;
	extern int row_sz;
	extern int col_sz;

	//temporary array for swapping 
	extern int* t_data;

	void initialise_data(int* input_data, int row_size, int col_size);

	Border_buffers create_send_and_receive_boundary_buffers(Sub_grid& p_grid);

	void mpi_pass_borders(Sub_grid& p_grid, Border_buffers& bf);

	void set_up_mpi(int argc, char* argv[]);

	//use MPI Cart functionality to assign coordinates to processors
	void assign_processor_coord(int periods[], int dims[]);

	//grid decomposition: grid dimensions should be divisible by dims[]
	void decompose_grid(int dims[], int grid_row, int grid_col, int& sub_grid_row, int& sub_grid_col);

};

#endif // !