CC    ?= gcc
AR    ?= ar
PREFIX = /usr/local
FLAGS  = -O3 -std=c99 -Wall -g -pedantic
SRC    = shp2osm.c
OBJ    = $(SRC:.c=.o)

quadtree_dir = quadtree
shapelib_dir = shapelib

INCLUDE = -I$(quadtree_dir)/src -I$(shapelib_dir)

LDFLAGS = -L$(quadtree_dir)/build -lquadtree -L$(shapelib_dir) -lshp

all: shp2osm

clean:
	@rm -f *.o shp2osm

%.o: %.c
	@$(CC) $< $(FLAGS) $(INCLUDE) -c -o $@

shp2osm: $(OBJ)
	@$(CC) $< -o $@ $(LDFLAGS)

.PHONY: shp2osm clean
