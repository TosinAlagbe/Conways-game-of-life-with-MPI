#include "Sub_grid.h"
#include "Globals.h"

Sub_grid::Sub_grid(int id, int row, int col, int* world, int w_row, int w_col, int* t_world)
	:curr_id{ id }, rows{ row }, cols{ col }, w_rows{ w_row }, w_cols{ w_col },
	w_ptr{ world }, t_w_ptr{ t_world }
{

	MPI_Cart_coords(comm_2d, id, 2, coords);
	top_border = new int[cols];
	bottom_border = new int[cols];
	left_border = new int[rows];
	right_border = new int[rows];

	//initialise to zero in case of non-periodic boundary
	for (int i = 0; i < cols; ++i) {
		top_border[i] = 0;
		bottom_border[i] = 0;
	}
	for (int i = 0; i < rows; ++i) {
		left_border[i] = 0;
		right_border[i] = 0;
	}

}

void Sub_grid::play_game(int p_id, int iter) {

	//arrays to specify direction of neighbor cells
	int row_dir[] = { 1,1,0,-1,-1,-1,0,1 };
	int col_dir[] = { 0,1,1,1,0,-1,-1,-1 };

	//iterate thtorugh every sub-grid cell
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {

			//sum of 8 neighbours
			int n_sum = 0;
			//each cell has 8 neighbours
			for (int d = 0; d < 8; ++d) {
				n_sum += this->operator()(i + row_dir[d], j + col_dir[d]);
			}

			//applies conwaw's game of life rule but writes to temp sub_grid
			apply_rule(n_sum, i, j);

		}
	}

	//write to file in binary
	post_to_file(p_id, "iter" + std::to_string(iter));
}

void Sub_grid::apply_rule(int n_sum, int i, int j) {

	//if living
	if (this->operator()(i, j) == 1) {

		if (n_sum <= 1 || n_sum >= 4) {
			t_w_ptr[(coords[0] * rows + i) * w_cols + (coords[1] * cols + j)] = 0;
		}
	}
	//if not living
	else {
		if (n_sum == 3) {
			t_w_ptr[(coords[0] * rows + i) * w_cols + (coords[1] * cols + j)] = 1;
		}
	}
}


void Sub_grid::post_to_file(int p_id, std::string f_name) {

	//sub-grid array to post to file
	int* out_arr = new int[rows * cols];

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			out_arr[i * cols + j]
				= t_w_ptr[(coords[0] * rows + i) * w_cols + (coords[1] * cols + j)];
		}
	}

	MPI_File file_handle;

	MPI_File_open(MPI_COMM_WORLD, f_name.c_str(), MPI_MODE_CREATE | MPI_MODE_RDWR,
		MPI_INFO_NULL, &file_handle);

	MPI_Datatype view_type;
	MPI_Offset disp = 0;
	int sizes[2] = { w_rows,w_cols }, subsizes[2] = { rows,cols };
	int starts[2] = { coords[0] * rows, coords[1] * cols };

	//create subarray view for each processor subarray
	MPI_Type_create_subarray(2, sizes, subsizes, starts, MPI_ORDER_C, MPI_INT, &view_type);
	MPI_Type_commit(&view_type);
	MPI_File_set_view(file_handle, disp, MPI_BYTE, view_type, "native", MPI_INFO_NULL);

	//write each processor subarray to file in binary with the correct postion on data array
	MPI_File_write(file_handle, out_arr, rows * cols, MPI_INT, MPI_STATUS_IGNORE);

	//set pointer to start of file  for reading
	MPI_File_seek(file_handle, 0, MPI_SEEK_SET);

	//receive buffer to be read into
	int* recv_buff = new int[rows * cols];

	MPI_Status status;

	//read binary file to swap with data for next iteration
	MPI_File_read_all(file_handle, &recv_buff[0], rows * cols, MPI_INT, &status);
	MPI_Barrier(comm_2d);

	//copy new grid to data
	int buff_indx = 0;
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {

			w_ptr[(coords[0] * rows + i) * w_cols + (coords[1] * cols + j)]
				= recv_buff[buff_indx];

			//update temporary data as well
			t_w_ptr[(coords[0] * rows + i) * w_cols + (coords[1] * cols + j)]
				= recv_buff[buff_indx];

			++buff_indx;
		}
	}

	MPI_File_close(&file_handle);

	delete[] out_arr;
	delete[] recv_buff;

}


int& Sub_grid::operator()(int i, int j) {

	//top border 
	if (i == -1) {
		if (j == -1)
			return t_left_corner;
		if (j == cols)
			return t_right_corner;
		return top_border[j];
	}

	//bottom border
	if (i == rows) {
		if (j == -1)
			return b_left_corner;
		if (j == cols)
			return b_right_corner;

		return bottom_border[j];
	}

	//note: corners have been taking care of in above code 

	//left border
	if (j == -1)
		return left_border[i];

	//right border
	if (j == cols)
		return right_border[i];

	return w_ptr[(coords[0] * rows + i) * w_cols + (coords[1] * cols + j)];
}

