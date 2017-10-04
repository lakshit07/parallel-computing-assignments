CC = mpicc
RM = rm -rf
FLAGS = -fopenmp
OBJECTS = mpi_template.o assign.o
EXE = assign

.PHONY: mpiassign
all: $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) -o $(EXE)

mpi_template.o: mpi_template.h assign.h
assign.o: mpi_template.h

.PHONY: clean
clean:
	$(RM) $(OBJECTS) mpiassign
