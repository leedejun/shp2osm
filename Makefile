CXX    ?= g++
CC     ?= gcc
AR     ?= ar
PREFIX = /opt/soft/shp2osm/bin
CXXFLAGS  = -O3 -std=c++11 -Wall -g -pedantic
CFLAGS    = -O3 -std=c99 -Wall -g -pedantic
SRC_CPP  = shp2osm.cpp osmutil.cpp
OBJ_CPP  = $(SRC_CPP:.cpp=.o)
OBJ_C    = $(SRC_C:.c=.o)

quadtree_dir = quadtree
shapelib_dir = shapelib

INCLUDE = -I$(quadtree_dir)/src -I$(shapelib_dir)

LDFLAGS = -L$(quadtree_dir)/build -lquadtree -L$(shapelib_dir) -lshp -lm

all: shp2osm

clean:
	@rm -f *.o shp2osm

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

%.o: %.c
	@$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

shp2osm: $(OBJ_CPP) $(OBJ_C)
	@$(CXX) $^ -o $@ $(LDFLAGS)

.PHONY: shp2osm clean
