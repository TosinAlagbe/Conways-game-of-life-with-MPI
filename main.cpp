#include "Sub_grid.h"
#include "Conway.h"
#include "Globals.h"
#include "Serial.h"
#include <iomanip>
#include <cstdlib>
#include <chrono>
#include <algorithm>
#include <utility>
#include <fstream>
#include <vector>
#include <string>
using std::string;
using std::vector;


//comment out when done with timing tests
#define DO_TIMING

//comment out before running on hpc cluster
//#define SERIAL

using namespace Conway;



int main(int argc, char* argv[])
{
	
	//-------------test case------------------------------------------
	//int grid_row = 6, grid_col = 4, sub_grid_row, sub_grid_col;
	//int grid_area = grid_row * grid_col;
	//int bin[24] = { 1,0,1,0,1,0,1,1,1,0,0,0,1,1,0,1,0,0,1,1,1,0,0,1 };
	////int* temp = new int[grid_area];
	//bool periodic = true;
	//int dims[2] = { 0,0 };
	//int periods[2] = { 0,0 };
	//if (periodic) {
	//	periods[0] = 1;
	//	periods[1] = 1;
	//}
	//int no_of_iter = 100;
	//--------------end of test case------------------------------------

	//-------------randomized test case ----------------------------------
	/*srand(time(NULL));
	int grid_row = 600, grid_col = 600, sub_grid_row, sub_grid_col;
	int grid_area = grid_row * grid_col;
	int* bin = new int[grid_area];
	int* temp = new int[grid_area];
	bool periodic = true;
	int dims[2] = { 0,0 };
	for (int i = 0; i < grid_area; ++i)
		bin[i] = rand() % 2;
	
	int periods[2] = { 0,0 };
	if (periodic) {
		periods[0] = 1;
		periods[1] = 1;
	}
	int no_of_iter = 100;*/
	//-------------end of randomized test case----------------------------------------------------------

	//--------------interesting initial conway pattern from files--------------------------------

	vector<string>lines;
	ifstream ist{ "spacegun.txt" };

	string line;
	while (getline(ist, line)) {

		lines.push_back(line);
	}
	int sub_grid_row, sub_grid_col;
	int grid_row = lines.size();
	int grid_col = lines[0].size();
	int grid_area = grid_row * grid_col;
	int* bin = new int[grid_area];

	int bin_idx =0;
	for (int i = 0; i < grid_row; ++i) {
		for (int j = 0; j < grid_col; ++j) {
			
			//convert char to int 
			bin[bin_idx] = lines[i][j] - '0';
			++bin_idx;
		}
	}
	bool periodic = false;
	int dims[2] = { 0,0 };

	int periods[2] = { 0,0 };
	if (periodic) {
		periods[0] = 1;
		periods[1] = 1;
	}
	int no_of_iter = 100;
	
	//-------------------end of interesting pattern initialisation----------------------------------------

#ifndef SERIAL


	//set up data, initialise mpi, set up grid and decompose grid
	initialise_data(bin, grid_row, grid_col);
	set_up_mpi(argc, argv);
	decompose_grid(dims, grid_row, grid_col, sub_grid_row, sub_grid_col);
	assign_processor_coord(periods, dims);

	
	//start timing from the instantiation of processor subgrid
#ifdef DO_TIMING
	auto start = chrono::high_resolution_clock::now();
#endif
	//create instance of subgrid for each processor
	Sub_grid p_grid{ id,sub_grid_row, sub_grid_col, Conway::data,grid_row, grid_col , Conway::t_data };

	//loop till end of iteration
	for (int iter = 0; iter < no_of_iter; ++iter) {

		//creae type to hold boundary values to pass between processes
		Border_buffers bf = Conway::create_send_and_receive_boundary_buffers(p_grid);

		//pass boundary values between processes
		mpi_pass_borders(p_grid, bf);

		//play conway's game of life on each process sub-grid
		//also contains function to write eachiteration outtput to file
		p_grid.play_game(id, iter);

	}

#ifdef DO_TIMING 
	auto finish = chrono::high_resolution_clock::now();
	double end = MPI_Wtime();
	if (id == 0)
	{
		std::chrono::duration<double> elapsed = finish - start;
		cout << setprecision(5);
		printf("No-of_iterations: %d \n", no_of_iter);
		printf("Grid size: %d rows by %d cols \n", grid_row, grid_col);
		printf("Periodic: %d \n", periodic);
		printf("No of Processes: %d \n", p);
		cout << "The parallel code took " << elapsed.count() << " to run \n";
	}
#endif

	//delete[] bin;
	//delete[] temp;
	//clean up MPI 
	MPI_Finalize();


#else
	auto start = chrono::high_resolution_clock::now();

	for (int iter = 0; iter < no_of_iter; ++iter) {
		serial_play_game(bin, temp, grid_row, grid_col);
	}
	auto finish = chrono::high_resolution_clock::now();
	
	std::chrono::duration<double> elapsed = finish - start;
	cout << setprecision(5);
	printf("The serial code took %g to run", elapsed.count());
	//cout << "The code took " << elapsed.count() << "s to run" << endl;
	

	delete[] bin;
	delete[] temp;

#endif

}






