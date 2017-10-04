CC = gcc
RM = rm -rf
FLAG = -fopenmp
OBJECTS = openmp_template.o assign.o
EXE = assign

.PHONY: ompvc
all: $(OBJECTS)
	$(CC) $(FLAG) $(OBJECTS) -o $(EXE)

openmp_template.o: openmp_template.h assign.h
vertex_cover.o: openmp_template.h

.PHONY: clean
clean:
	$(RM) $(OBJECTS) ompvc
