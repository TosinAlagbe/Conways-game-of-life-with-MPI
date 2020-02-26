#ifndef SUB_GRID_H
#define SUB_GRID_H

#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <string>


using namespace std;



struct Sub_grid {

	Sub_grid(int id, int row, int col, int* world, int w_row, int w_col, int* t_world);

	//pointer to current world
	int* w_ptr;
	//row and col size of world
	int w_cols;
	int w_rows;

	//pointers to temp world
	int* t_w_ptr;

	//sub-grid buffers for receiving neighbour borders
	int* top_border;
	int* bottom_border;
	int* right_border;
	int* left_border;
	//initialise to zero in case of non periodic
	int t_left_corner = 0;
	int t_right_corner = 0;
	int b_left_corner = 0;
	int b_right_corner = 0;

	//convenient indexing 
	int& operator()(int i, int j);


	//processor rank/id
	int curr_id;

	int coords[2];

	int cols;
	int rows;

	void play_game(int p_id, int iter);

	void apply_rule(int n_sum, int i, int j);

	void post_to_file(int p_id, std::string filename);

	//pass boundaries to buffer
	void get_top(int* top_buffer) {
		for (int j = 0; j < cols; ++j)
			top_buffer[j] = this->operator()(0, j);
	}
	void get_bottom(int* bottom_buffer) {
		for (int j = 0; j < cols; ++j)
			bottom_buffer[j] = this->operator()(rows - 1, j);
	}
	void get_left(int* left_buffer) {
		for (int i = 0; i < rows; ++i)
			left_buffer[i] = this->operator()(i, 0);
	}
	void get_right(int* right_buffer) {
		for (int i = 0; i < rows; ++i)
			right_buffer[i] = this->operator()(i, cols - 1);
	}
	void get_t_left_corner(int* t_left_corner) {
		*t_left_corner = this->operator()(0, 0);
	}
	void get_t_right_corner(int* t_right_corner) {
		*t_right_corner = this->operator()(0, cols - 1);
	}
	void get_b_left_corner(int* b_left_corner) {
		*b_left_corner = this->operator()(rows - 1, 0);
	}
	void get_b_right_corner(int* b_right_corner) {
		*b_right_corner = this->operator()(rows - 1, cols - 1);
	}

	void random_init();

};

#endif // !SUB_GRID_H
