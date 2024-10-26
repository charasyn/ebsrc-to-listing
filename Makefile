# ebsrc-to-listing makefile
# charasyn 2024
# Public domain; see UNLICENSE.txt for more info

# C++ compiler to use.
cppc := g++

###############################################################################

target := ebsrc-to-listing
cpp_sources := $(wildcard src/*.cpp)
cpp_headers := $(wildcard src/*.hpp)

cppflags := -g -O2 -std=c++20 -Wall -Wextra -Werror -pedantic -DNOT_VSCODE=1
linkflags := 

###############################################################################

ifeq ($(OS),Windows_NT)
	target := $(target).exe
endif

all: $(target)

objs := $(patsubst src/%.cpp,obj/%.o,$(cpp_sources))
$(target): $(objs)
	$(cppc) $(cppflags) $(linkflags) -o $@ $^

obj/%.o: src/%.cpp $(cpp_headers) | dir_obj
	$(cppc) $(cppflags) -c -o $@ $<

.PHONY: dir_obj clean

dir_obj:
	mkdir -p obj

clean:
	rm -rf obj
	rm -f $(target)
