EXE = matmul3

SRC_DIR = src
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CC = g++ -g --std=c++11 -fdiagnostics-color=always -fopenmp

ifneq (${OMPS},)
	CC += -DOMP_SCHEDULING=${OMPS}
endif

CPPFLAGS += -Iinclude
CFLAGS += -Wall -Wextra -Wpedantic
LDFLAGS += -Llib
LDLIBS +=

.PHONY: all clean omp_static

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
	@ssh peac_gcc OMPS=static make -C PCA/$(EXE)/ clean all
	@ssh peac mv PCA/$(EXE)/$(EXE).out PCA/$(EXE)/$(EXE)_static.out
	@ssh peac_gcc OMPS=dynamic make -C PCA/$(EXE)/ clean all
	@ssh peac mv PCA/$(EXE)/$(EXE).out PCA/$(EXE)/$(EXE)_dynamic.out
	@ssh peac_gcc OMPS=guided make -C PCA/$(EXE)/ clean all
	@ssh peac mv PCA/$(EXE)/$(EXE).out PCA/$(EXE)/$(EXE)_guided.out
	@echo -e "\033[1;34m> Running binary on remote\033[0m"
	@ssh peac sbatch PCA/$(EXE)/sbatch.sh PCA/$(EXE)/$(EXE).out
