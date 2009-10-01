EXE = EF

OBJS = \
	EquilibriumFlow.o HornerPolynomial.o BPRFunction.o \
	Bush.o GraphEdge.o Origin.o SecantSolver.o BushNode.o ABGraph.o\
	AlgorithmBSolver.o GraphImporter.o

OBJDIR = ./objs/

INCL = -I./include

VPATH = src:src/TAPFramework:objs

# C++ Compiler command
CXX = g++

CXXFLAGS = \
	-g -pipe -pedantic-errors -Wimplicit -Wparentheses -Wreturn-type\
	-Wcast-qual -Wall -Wpointer-arith -Wwrite-strings -Wconversion -O3\
	-march=native

# additional C++ Compiler options for linking

all: $(EXE)

.SUFFIXES: .cpp .o

$(EXE): $(OBJS)
	bla=;\
	for file in $(OBJS); do bla=$(OBJDIR)"$$file $$bla"; done; \
	$(CXX) $(CXXFLAGS) -o $@ $$bla

clean:
	bla=;\
	for file in $(OBJS); do bla=$(OBJDIR)"$$file $$bla"; done; \
	rm -rf $(EXE) $$bla

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCL) -c -o $(OBJDIR)$@ $<
