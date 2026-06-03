# Compiler
CXX := g++

# Executable name
TARGET := out 

# Source and Object files
SRC := main.cpp MOS6502.cpp C64.cpp 
OBJ := $(SRC:.cpp=.o)

# Compiler flags
# `sdl2-config --cflags` adds the necessary include paths
CXXFLAGS := #`sdl2-config --cflags`

# Linker flags
# `sdl2-config --libs` adds the necessary library links (like -lSDL2)
LDFLAGS := #`sdl2-config --libs`

# Build target
all: $(TARGET)

# Link the executable from object files
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

# Compile source files into object files
# -c tells the compiler to skip linking and just produce the .o file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Utility targets
run: all
	./out kernal.901227-02.bin basic.901226-01.bin

edit:
	nvim Main.cpp *.cpp *.h 

clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean run edit
