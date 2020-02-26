#include "Conway.h"
#include "Globals.h"
#include "Sub_grid.h"

//2d input array flattened
int* Conway::data;
int Conway::row_sz;
int Conway::col_sz;

//temporary array for swapping 
int* Conway::t_data;

void Conway::initialise_data(int* input_data, int row_size, int col_size) {

	row_sz = row_size;
	col_sz = col_size;

	data = new int[row_size * col_size];
	t_data = new int[row_size * col_size];

	int cnt = 0;
	for (int i = 0; i < row_sz * col_sz; ++i) {
		data[i] = input_data[i];
		t_data[i] = input_data[i];

	}

}

Border_buffers Conway::create_send_and_receive_boundary_buffers(Sub_grid& p_grid) {

	Border_buffers buff;

	//create send buffers 
	buff.left_buffer = new int[p_grid.rows];
	buff.right_buffer = new int[p_grid.rows];
	buff.top_buffer = new int[p_grid.cols];
	buff.bottom_buffer = new int[p_grid.cols];


	//copy boundaries into respective  send buffers	
	p_grid.get_left(buff.left_buffer);
	p_grid.get_right(buff.right_buffer);
	p_grid.get_top(buff.top_buffer);
	p_grid.get_bottom(buff.bottom_buffer);
	p_grid.get_b_left_corner(&buff.b_left_buffer);
	p_grid.get_b_right_corner(&buff.b_right_buffer);
	p_grid.get_t_left_corner(&buff.t_left_buffer);
	p_grid.get_t_right_corner(&buff.t_right_buffer);

	//create receive buffers 
	buff.rec_left_buffer = new int[p_grid.rows];
	buff.rec_right_buffer = new int[p_grid.rows];
	buff.rec_top_buffer = new int[p_grid.cols];
	buff.rec_bottom_buffer = new int[p_grid.cols];
	buff.rec_t_left_buffer = 0;
	buff.rec_t_right_buffer = 0;
	buff.rec_b_left_buffer = 0;
	buff.rec_b_right_buffer = 0;

	return buff;
}

void Conway::mpi_pass_borders(Sub_grid& p_grid, Border_buffers& bf) {

	int tag = 0;

	MPI_Request* request = new MPI_Request[16];

	//send and recieve top and botttom buffers
	if ((up_p != MPI_PROC_NULL && up_p != id) || (down_p != MPI_PROC_NULL && down_p != id)) {

		MPI_Isend(&bf.top_buffer[0], p_grid.cols, MPI_INT, up_p, tag, comm_2d, &request[0]);
		MPI_Irecv(&bf.rec_top_buffer[0], p_grid.cols, MPI_INT, up_p, tag, comm_2d, &request[1]);

		MPI_Isend(&bf.bottom_buffer[0], p_grid.cols, MPI_INT, down_p, tag, comm_2d, &request[2]);
		MPI_Irecv(&bf.rec_bottom_buffer[0], p_grid.cols, MPI_INT, down_p, tag, comm_2d, &request[3]);

	}
	//send and receive left and right process buffer
	if ((left_p != MPI_PROC_NULL && left_p != id) || (right_p != MPI_PROC_NULL && right_p != id)) {

		MPI_Isend(&bf.left_buffer[0], p_grid.rows, MPI_INT, left_p, tag, comm_2d, &request[4]);
		MPI_Irecv(&bf.rec_left_buffer[0], p_grid.rows, MPI_INT, left_p, tag, comm_2d, &request[5]);

		MPI_Isend(&bf.right_buffer[0], p_grid.rows, MPI_INT, right_p, tag, comm_2d, &request[6]);
		MPI_Irecv(&bf.rec_right_buffer[0], p_grid.rows, MPI_INT, right_p, tag, comm_2d, &request[7]);

	}
	//set and receive corner process buffers
	if ((t_left_p != MPI_PROC_NULL && t_left_p != id) || (b_right_p != MPI_PROC_NULL && b_right_p != id)
		|| (t_right_p != MPI_PROC_NULL && t_right_p != id) || (b_left_p != MPI_PROC_NULL && b_left_p != id)) {


		MPI_Isend(&bf.t_left_buffer, 1, MPI_INT, t_left_p, tag, comm_2d, &request[8]);
		MPI_Irecv(&bf.rec_t_left_buffer, 1, MPI_INT, t_left_p, tag, comm_2d, &request[9]);

		MPI_Isend(&bf.b_right_buffer, 1, MPI_INT, b_right_p, tag, comm_2d, &request[10]);
		MPI_Irecv(&bf.rec_b_right_buffer, 1, MPI_INT, b_right_p, tag, comm_2d, &request[11]);

		MPI_Isend(&bf.t_right_buffer, 1, MPI_INT, t_right_p, tag, comm_2d, &request[12]);
		MPI_Irecv(&bf.rec_t_right_buffer, 1, MPI_INT, t_right_p, tag, comm_2d, &request[13]);

		MPI_Isend(&bf.b_left_buffer, 1, MPI_INT, b_left_p, tag, comm_2d, &request[14]);
		MPI_Irecv(&bf.rec_b_left_buffer, 1, MPI_INT, b_left_p, tag, comm_2d, &request[15]);

	}

	MPI_Waitall(16, request, MPI_STATUS_IGNORE);

	MPI_Barrier(comm_2d);

	//transfer data in receive buffers to sub_grid buffers
	if (up_p != MPI_PROC_NULL && up_p != id) {
		if (up_p == down_p)
			p_grid.bottom_border = bf.rec_top_buffer;
		else
			p_grid.top_border = bf.rec_top_buffer;
	}
	else if (up_p != MPI_PROC_NULL && up_p == id) {
		for (int i = 0; i < p_grid.cols; ++i) {
			p_grid.top_border[i] = p_grid(p_grid.rows - 1, i);
		}
	}

	if (down_p != MPI_PROC_NULL && down_p != id) {
		if (down_p == up_p)
			p_grid.top_border = bf.rec_bottom_buffer;
		else
			p_grid.bottom_border = bf.rec_bottom_buffer;
	}
	else if (down_p != MPI_PROC_NULL && down_p == id) {
		for (int i = 0; i < p_grid.cols; ++i) {
			p_grid.bottom_border[i] = p_grid(0, i);
		}
	}

	if (left_p != MPI_PROC_NULL && left_p != id) {
		if (left_p == right_p)
			p_grid.right_border = bf.rec_left_buffer;
		else
			p_grid.left_border = bf.rec_left_buffer;
	}
	else if (left_p != MPI_PROC_NULL && left_p == id) {
		for (int i = 0; i < p_grid.cols; ++i) {
			p_grid.left_border[i] = p_grid(i, p_grid.cols - 1);
		}
	}

	if (right_p != MPI_PROC_NULL && right_p != id) {
		if (right_p == left_p)
			p_grid.left_border = bf.rec_right_buffer;
		else
			p_grid.right_border = bf.rec_right_buffer;
	}
	else if (right_p != MPI_PROC_NULL && right_p == id) {
		for (int i = 0; i < p_grid.cols; ++i) {
			p_grid.right_border[i] = p_grid(i, 0);
		}
	}

	if (t_left_p != MPI_PROC_NULL && t_left_p != id) {
		if (t_left_p == b_right_p)
			p_grid.b_right_corner = bf.rec_t_left_buffer;
		else
			p_grid.t_left_corner = bf.rec_t_left_buffer;
	}
	else if (t_left_p != MPI_PROC_NULL && t_left_p == id) {
		p_grid.t_left_corner = p_grid.b_right_corner;
	}

	if (t_right_p != MPI_PROC_NULL && t_right_p != id) {
		if (t_right_p == b_left_p)
			p_grid.b_left_corner = bf.rec_t_right_buffer;
		else
			p_grid.t_right_corner = bf.rec_t_right_buffer;
	}
	else if (t_right_p != MPI_PROC_NULL && t_right_p == id) {
		p_grid.t_right_corner = p_grid.b_left_corner;
	}

	if (b_left_p != MPI_PROC_NULL && b_left_p != id) {
		if (b_left_p == t_right_p)
			p_grid.t_right_corner = bf.rec_b_left_buffer;
		else
			p_grid.b_left_corner = bf.rec_b_left_buffer;
	}
	else if (b_left_p != MPI_PROC_NULL && b_left_p == id) {
		p_grid.b_left_corner = p_grid.t_right_corner;
	}

	if (b_right_p != MPI_PROC_NULL && b_right_p != id) {
		if (b_right_p == t_left_p)
			p_grid.t_left_corner = bf.rec_b_right_buffer;
		else
			p_grid.b_right_corner = bf.rec_b_right_buffer;
	}
	else if (b_right_p != MPI_PROC_NULL && b_right_p == id) {
		p_grid.b_right_corner = p_grid.t_left_corner;

	}
}

void Conway::set_up_mpi(int argc, char* argv[]) {

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

}

//use MPI Cart functionality to assign coordinates to processors
void Conway::assign_processor_coord(int periods[], int dims[]) {

	int coords[2];

	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &comm_2d);
	MPI_Cart_coords(comm_2d, id, 2, coords);


	//Get neighbor process ids
	MPI_Cart_shift(comm_2d, 0, 1, &up_p, &down_p);
	MPI_Cart_shift(comm_2d, 1, 1, &left_p, &right_p);
	//corner coordinates
	int top_coords[2];
	int t_left_coords[2] = { coords[0] - 1, coords[1] - 1 };
	int t_right_coords[2] = { coords[0] - 1, coords[1] + 1 };
	int b_left_coords[2] = { coords[0] + 1, coords[1] - 1 };
	int b_right_coords[2] = { coords[0] + 1,coords[1] + 1 };
	//check for corner process when non periodic
	if (periods[0] == 0 && periods[1] == 0) {

		if (t_left_coords[0] < 0 || t_left_coords[1] < 0)
			t_left_p = MPI_PROC_NULL;
		else
			MPI_Cart_rank(comm_2d, t_left_coords, &t_left_p);

		if (t_right_coords[0] < 0 || t_right_coords[1] > dims[1] - 1)
			t_right_p = MPI_PROC_NULL;
		else
			MPI_Cart_rank(comm_2d, t_right_coords, &t_right_p);

		if (b_left_coords[0] > dims[0] - 1 || b_left_coords[1] < 0)
			b_left_p = MPI_PROC_NULL;
		else
			MPI_Cart_rank(comm_2d, b_left_coords, &b_left_p);

		if (b_right_coords[0] > dims[0] - 1 || b_right_coords[1] > dims[1] - 1)
			b_right_p = MPI_PROC_NULL;
		else
			MPI_Cart_rank(comm_2d, b_right_coords, &b_right_p);
	}
	else {

		MPI_Cart_rank(comm_2d, t_left_coords, &t_left_p);
		MPI_Cart_rank(comm_2d, t_right_coords, &t_right_p);
		MPI_Cart_rank(comm_2d, b_left_coords, &b_left_p);
		MPI_Cart_rank(comm_2d, b_right_coords, &b_right_p);

	}

}

//grid decomposition: grid dimensions should be divisible by dims[]
void Conway::decompose_grid(int dims[], int grid_row, int grid_col, int& sub_grid_row, int& sub_grid_col) {

	MPI_Dims_create(p, 2, dims);

	sub_grid_row = grid_row / dims[0];
	sub_grid_col = grid_col / dims[1];

}