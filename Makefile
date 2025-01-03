CXX = clang++
SRC = ball.cpp simbox.cpp main.cpp # List only the source files (not headers)
HEADERS = $(wildcard *.h) # List all header files in the current directory
CXX_FLAGS = -std=c++20 -gdwarf-4 -fstandalone-debug -O0 -Wall -Wextra -Werror 

UBSAN_FLAGS = -fsanitize=undefined -fno-sanitize=alignment

exec: main.out
	./main.out

clean:
	rm -f *.out

ubsan: $(SRC)
	$(CXX) $(CXX_FLAGS) $(UBSAN_FLAGS) $^ -o main.out
	./main.out

new: new.out
	./new.out


# This passes only the source files ($(SRC)) to the compiler. 
# You don’t include header files in the compilation command because they are included by the source files as needed.
main.out: $(SRC) $(HEADERS)
	$(CXX) $(CXX_FLAGS) $(SRC) -o $@  

new.out: simbox2.cpp simbox2.h main.cpp
	$(CXX) $(CXX_FLAGS) simbox2.cpp main.cpp -o $@



.PHONY: clean exec ubsan sim
.DEFAULT_GOAL := exec

  
    
# #$<: Represents the first prerequisite (or dependency), xxl/main.cpp, in the rule
# #$@: Represents the target of the rule (xxl1/main.out)

# Comments explaining variables
#$@: Represents the target of the rule (main.out)
#$^: Represents all the prerequisites (in this case, all source files in $(SRC))



