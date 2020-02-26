MAKE = make
TARGET = my_code
SOURCES = main.cpp Globals.cpp Conway.cpp Sub_grid.cpp Serial.cpp 

default:
	mpicxx -std=c++11 -o $(TARGET) $(SOURCES)