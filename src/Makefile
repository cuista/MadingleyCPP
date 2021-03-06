# Source directories separated by space
# Example ./ src1/ src2/
SRCDIR=./ Data/ Input/ Model/ Output/ Tools/
# Directory where object files will be placed
OBJDIR=../build/
# Include directories separated by space
# Example: include1/ include2/
INCDIR=./ Data/ Input/ Model/ Output/ Tools/ /home/philju/Libraries/zlib-1.2.8/include /home/philju/Libraries/hdf5-1.8.17/include /home/philju/Libraries/netcdf-4.4.1/include /home/philju/Libraries/netcdf-cxx4-4.3/include
# Libraries separated by space
LIBS= $(SUBLIBS) -L/home/philju/Libraries/netcdf-cxx4-4.3/lib -L/home/philju/Libraries/netcdf-4.4.1/lib -L/home/philju/Libraries/hdf5-1.8.17/lib -L/home/philju/Libraries/zlib-1.2.8/lib -L/usr/lib -lnetcdf_c++4 -lnetcdf -lhdf5_hl -lhdf5 -lz -ldl -lpthread 
# Directory where binary file will be placed
BINDIR=../dist/
# Name of the result file
TARGET=madingley
# Compiler
CXX=g++

# Retrive list of the source files
SRC=$(wildcard $(addsuffix *.cpp,$(SRCDIR)))
# Generate list of the object files
OBJ=$(addprefix $(OBJDIR), $(patsubst %.cpp, %.o, $(notdir $(SRC))))

VPATH=$(SRCDIR)

# Compilation flags
CXXFLAGS=-O2 -std=c++11

$(TARGET) : $(OBJ)
	@echo Linking...
	@mkdir -p $(BINDIR)
	@$(CXX) $(CXXFLAGS) -o $(BINDIR)$@ $(OBJ) $(LIBS)
	@cp -R ../input $(BINDIR)
	@echo Madingley compilation complete.

$(OBJDIR)%.o : %.cpp
	@echo Compiling $< in $@...
	@mkdir -p $(OBJDIR)
	@$(CXX) $(CXXFLAGS) $(addprefix -I,$(INCDIR)) -c -o $@ $<

clean :
	@$(RM) -r $(OBJDIR)
	@$(RM) -r $(BINDIR)
