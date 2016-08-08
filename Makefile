# source files.


SRC = main.cpp GFHOG.cpp HogDetect.cpp
OBJ = $(SRC:.cpp=.o)
OUT = GFHOG.ln

INCS_DIR = /usr/local/include/
LIBS_DIR = /usr/local/lib/
SYS_LIBS_DIR  = /usr/lib/x86_64-linux-gnu

# include directories
INCLUDES = -I$(INCS_DIR)

# C++ compiler flags (-g -O2 -Wall)
CCFLAGS = -g -fPIC -fopenmp -Wall -O0

# compiler
CCC = g++

# library paths
LIBS = -L$(LIBS_DIR) -L$(SYS_LIBS_DIR) `pkg-config --cflags --libs opencv`  -lswscale -lswresample -lavformat -lavcodec -lavutil -lz -lbz2 -lpthread -lsuperlu -lblas

# compile flags
LDFLAGS = -g -fPIC -D_DEBUG=1

.SUFFIXES: .cpp

default: $(OUT)

.cpp.o:
	$(CCC) $(INCLUDES) $(CCFLAGS) -D_DEBUG=1 -c $< -o $@

$(OUT): $(OBJ)
	$(CCC) $(OBJ) $(LIBS) $(CCFLAGS) -o $@

clean:
	rm -f $(OBJ) $(OUT) Makefile.bak 
