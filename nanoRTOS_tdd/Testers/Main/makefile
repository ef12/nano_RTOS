# Build Pedo tdd project
TARGET           := pedo_tdd.exe
BUILD_DIR        := build

# The Pedo tdd project location
PROJ             := $(CURDIR)
OPTIMISE         := -O1

# Compilers
GXX              := g++ # C++
GCC              :=     # C

# include all the cpp source files
CPP_SRC_FILES    += Main.cpp

CPP_SRC_DIRS     := $(dir $(CPP_SRC_FILES))
SRCES            := $(notdir $(CPP_SRC_FILES))
OBJECTS          := $(SRCES:=.o)
OBJ_FILES        := $(addprefix $(BUILD_DIR)/,$(OBJECTS))

# Header files folders
INC_FOLDERS      += .
INC_FOLDERS      += ../../../extern/cpputest_build/include

# Libreries
LIBS_LOC         += ../../../extern/cpputest_build/lib
LIBS             += CppUTest CppUTestExt

# Compiler and Linker flags
CFLAGS           += -std=c99 -MP -MD -c -g3 $(OPTIMISE) -Wall
CPPFLAGS         += -MP -MD -c -g3 $(OPTIMISE) -Wall
LNFLAGS          += 

VPATH := $(CPP_SRC_DIRS)

all: $(BUILD_DIR)/$(TARGET)

# Linking
$(BUILD_DIR)/$(TARGET): $(OBJ_FILES)
	@echo 'Building target: $@'
	@echo 'Invoking: Cygwin C++ Linker'
	@$(GXX) $(LNFLAGS) -o $@ $^ $(addprefix -L,$(LIBS_LOC)) $(addprefix -l,$(LIBS))
	@echo 'Finished building target: $@'
	@echo ' '
	
# Compiling the cpp code
$(BUILD_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(BUILD_DIR)
	@echo 'Compiling $<'
	@$(GXX) -o $@ $< $(addprefix -I,$(INC_FOLDERS)) $(CPPFLAGS)
	@echo 'Finished Compiling: $<'
	@echo ' '
	
#
#clean:
#	rm -f build/*.o build/*.d build/*.exe
# EOF
