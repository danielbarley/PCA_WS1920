EXE = piapprox

SRC_DIR = src
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CC = g++ --std=c++11
CPPFLAGS += -Iinclude
CFLAGS += -Wall -Wextra -Wpedantic
LDFLAGS += -Llib
#LDLIBS += -l

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)

run:
	@scp -r ./* peac:PCA/pn/
	@ssh peac_gcc make -C PCA/pn all
	@echo "> Running binary with arg 7"
	@ssh peac PCA/pn/$(EXE) 7
