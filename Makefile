# ebsrc-to-listing makefile
# charasyn 2024

# C++ compiler to use.
cppc := g++

###############################################################################

target := ebsrc-to-listing
cpp_sources := $(wildcard src/*.cpp)

cppflags := -O2 -std=c++17 -Wall -Wextra -Werror -pedantic
linkflags := 

###############################################################################

ifeq ($(OS),Windows_NT)
	target := $(target).exe
endif

all: $(target)

objs := $(patsubst src/%.cpp,obj/%.o,$(cpp_sources))
$(target): $(objs)
	$(cppc) $(cppflags) $(linkflags) -o $@ $^

obj/%.o: src/%.cpp | dir_obj
	$(cppc) $(cppflags) -c -o $@ $<

.PHONY: dir_obj clean

dir_obj:
	mkdir -p obj

clean:
	rm -rf obj
	rm -f $(target)
