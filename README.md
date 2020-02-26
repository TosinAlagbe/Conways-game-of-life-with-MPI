# acse-6-individual-assignment-acse-ota19
# Oluwatosin Alagbe
## Conway's game of life parallel implementation using MPI

### How to run already built post processor
This has no arguments to maintain simplicity. It is hardcoded to take in 50 iteration files of 60 by 60 grid each. If you want to make changes to view other grid sizes and extend more iteration files, please see source code ```sfml_post_processing_source ```and you can build it using this tutorial
```
https://www.youtube.com/watch?v=YfMQyOw1zik
```
To use the built post processor (for 50 iterations of 60 by 60 pattern input)
```
run sfml_try.exe 
```
If you're building from source, make sure iteration files are copied to same directory as executable.

### Interesting inputs
Some intersting inputs have been added  such as ```spacegun.txt``` to be used in the mpi parallel program.
Comment out the sections in ```main.cpp``` as needed to change input formats.
Parallel program should be run using an MPI library/implementation.

