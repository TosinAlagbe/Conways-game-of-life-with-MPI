#pragma once

struct Border_buffers {

	//send buffers
	int* left_buffer;
	int* right_buffer;
	int* top_buffer;
	int* bottom_buffer;
	int t_left_buffer;
	int t_right_buffer;
	int b_left_buffer;
	int b_right_buffer;

	//receive buffers 
	int* rec_left_buffer;
	int* rec_right_buffer;
	int* rec_top_buffer;
	int* rec_bottom_buffer;
	int rec_t_left_buffer;
	int rec_t_right_buffer;
	int rec_b_left_buffer;
	int rec_b_right_buffer;

};
