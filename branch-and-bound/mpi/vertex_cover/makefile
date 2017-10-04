CC = mpicc
RM = rm -rf
FLAG = -fopenmp
OBJECTS = mpi_template.o vertex_cover.o
EXE = vertex_cover

.PHONY: mpivc
all: $(OBJECTS)
	$(CC) $(FLAG) $(OBJECTS) -o $(EXE)

openmp_template.o: mpi_template.h vertex_cover.h
vertex_cover.o: mpi_template.h

.PHONY: clean
clean:
	$(RM) $(OBJECTS) mpivc
