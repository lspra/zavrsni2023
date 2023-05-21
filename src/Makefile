cpp_files := $(wildcard **/*.cpp) $(wildcard *.cpp)
o_files := $(patsubst %.cpp, %.o, $(cpp_files))
CXXFLAGS := -std=c++23 -Wall

all: $(o_files)
	g++ -std=c++23 -Wall $(o_files) -o main
	
$(o_files) : %.o: %.cpp

clean:
	del $(wildcard *.o)