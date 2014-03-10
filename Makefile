CC    ?= gcc
AR    ?= ar
PREFIX = /usr/local
FLAGS  = -O3 -std=c99 -Wall -g -pedantic
SRC    = shp2osm.c
OBJ    = $(SRC:.c=.o)

all: shp2osm

quardtree/build/libquadtree.a: $(OBJ)
	@mkdir -p build
	@$(AR) rcs $@ $^

shapelib/libshp.a: $(OBJ)
	@mkdir -p build
	@$(AR) rcs $@ $^

clean:
	@rm -fr bin build *.o src/*.o

%.o: %.c
	@$(CC) $< $(FLAGS) -c -o $@

shp2osm: *.c $(OBJ)
	@mkdir -p bin
	@$(CC) $^ -o bin/$@
	@bin/$@

.PHONY: shp2osm clean
