.DEFAULT_GOAL = all

# Directories.
BIN_DIR = bin
BUILD_DIR = build
SRC_DIR = src

# Compiler.
CXX = g++
CXXFLAGS = -Werror -Wall -O2

# Executable files.
EXEC_NAMES = fractalGen fractalGenAdaptive timehistory
EXEC_FILES = $(addprefix $(BIN_DIR)/, $(EXEC_NAMES))
# Source files, grouped by function.
CPP_DOUBLEPEND = $(wildcard $(SRC_DIR)/DoublePendulum/*.cpp)
CPP_ADAPTCALC = $(wildcard $(SRC_DIR)/AdaptiveCalculation/*.cpp)
CPP_ALL = $(CPP_DOUBLEPEND) $(CPP_ADAPTCALC)
# Object files.
OBJ_DOUBLEPEND = $(CPP_DOUBLEPEND:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
OBJ_ADAPTCALC = $(CPP_ADAPTCALC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
OBJ_EXEC = $(EXEC_NAMES:%=$(BUILD_DIR)/%.o)
OBJ_ALL = $(OBJ_DOUBLEPEND) $(OBJ_ADAPTCALC) $(OBJ_EXEC)
# Prevent make from removing object files as intermediate files.
.PRECIOUS: $(OBJ_ALL)
# Dependency files.
DEP_DOUBLEPEND = $(OBJ_DOUBLEPEND:%.o=%.d)
DEP_ADAPTCALC = $(OBJ_ADAPTCALC:%.o=%.d)
DEP_ALL = $(DEP_DOUBLEPEND) $(DEP_ADAPTCALC)

.PHONY: all
all: $(EXEC_FILES)

# Main targets with specific dependencies.
$(BIN_DIR)/timehistory : $(BIN_DIR)/% : $(BUILD_DIR)/%.o $(OBJ_DOUBLEPEND)
# Ensure directory strucutre is preserved.
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN_DIR)/fractalGen : $(BIN_DIR)/% : $(BUILD_DIR)/%.o $(OBJ_DOUBLEPEND)
# Ensure directory strucutre is preserved.
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN_DIR)/fractalGenAdaptive : $(BIN_DIR)/%: $(BUILD_DIR)/%.o $(OBJ_DOUBLEPEND) $(OBJ_ADAPTCALC)
# Ensure directory strucutre is preserved.
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Include all dependency (.d) files.
-include $(DEP_ALL)

# Build target for every single object file.
# The potential dependency on header files is covered by calling
# `-include $(DEP_ALL)`.
$(BUILD_DIR)/%.o : $(SRC_DIR)/%.cpp
# Ensure directory strucutre is preserved.
	@mkdir -p $(@D)
# The -MMD flags additionaly creates a .d file with the same name as the .o
# file in the same directory.
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)/*
	rm -rf $(BIN_DIR)/*
