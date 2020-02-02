EXE = heatgrid

SRC_DIR = src
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CC = mpic++ --std=c++11 -fdiagnostics-color=always -O2
CPPFLAGS += -Iinclude
CFLAGS += -Wall -Wextra -Wpedantic
LDFLAGS += -Llib
#LDLIBS += -l

.PHONY: all clean debug

all: $(EXE)

debug: $(OBJ)
	$(CC) -g $(LDFLAGS) $^ $(LDLIBS) -o $@.out

$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@.out

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -g $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)

rpush:
	@echo -e "\033[1;34m> Syncing remote\033[0m"
	@rsync -r --update . peac:PCA/pn/$(EXE)/

rcompile:
	@echo -e "\033[1;34m> compiling on remote\033[0m"
	@ssh peac_gcc hostname
	@ssh peac_gcc "source ~/.bash_mpi; make -C PCA/pn/$(EXE)/ clean all"

rrun:
	@echo -e "\033[1;34m> Running binary on remote\033[0m"
	@ssh peac "source ~/.bash_mpi; sbatch PCA/pn/$(EXE)/sbatch.sh PCA/pn/$(EXE)/$(EXE).out"

run: rpush rcompile rrun
