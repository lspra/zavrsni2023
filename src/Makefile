cpp_files := $(wildcard **/*.cpp) $(wildcard *.cpp)
o_files := $(patsubst %.cpp, %.o, $(cpp_files))
CXXFLAGS := -std=c++23 -Wall -Wextra -Wpedantic

all: $(o_files)
	g++ $(CXXFLAGS) $(o_files) -o lsharp
	
$(o_files) : %.o: %.cpp

clean:
	del $(wildcard *.o)