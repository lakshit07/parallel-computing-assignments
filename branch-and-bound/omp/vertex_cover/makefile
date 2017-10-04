CC = gcc
RM = rm -rf
FLAG = -fopenmp
OBJECTS = openmp_template.o vertex_cover.o
EXE = vertex_cover

.PHONY: ompvc
all: $(OBJECTS)
	$(CC) $(FLAG) $(OBJECTS) -o $(EXE)

openmp_template.o: openmp_template.h vertex_cover.h
vertex_cover.o: openmp_template.h

.PHONY: clean
clean:
	$(RM) $(OBJECTS) ompvc
