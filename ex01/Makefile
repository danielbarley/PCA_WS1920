EXE = piapprox

SRC_DIR = src
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CC = g++ --std=c++11 -fdiagnostics-color=always
CPPFLAGS += -Iinclude
CFLAGS += -Wall -Wextra -Wpedantic
LDFLAGS += -Llib
#LDLIBS += -l

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@.out

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)

run:
	@echo -e "\033[1;34m> Syncing remote\033[0m"
	@rsync -r --update . peac:PCA/$(EXE)/
	@echo -e "\033[1;34m> compiling on remote\033[0m"
	@ssh peac_gcc make -C PCA/$(EXE)/ clean all
	@echo -e "\033[1;34m> Running binary on remote with arg 9\033[0m"
	@ssh peac srun time PCA/$(EXE)/$(EXE).out 9
