DBUSCC_DEBUG = 1

CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++03
LDFLAGS = -L.
CPPFLAGS = -Iinclude

ifeq ($(DBUSCC_DEBUG),1)
CXXFLAGS += -g -O0
endif

DEPS = dbus-1 cppunit
DEPS += libtscb

CXXFLAGS += $(shell pkg-config --cflags $(DEPS)) -fPIC
LIBS = -Wl,--as-needed $(shell pkg-config --libs $(DEPS))

TARGET = libdbuscc.so
TESTS = test_dbuscc

SRC = $(wildcard src/*.cc)
OBJ = $(SRC:%.cc=%.o)

TEST_SRC = $(wildcard tests/*.cc)
TEST_OBJ = $(TEST_SRC:%.cc=%.o)

EXAMPLE_SRC = $(wildcard examples/*.cc)
EXAMPLE_OBJ = $(EXAMPLE_SRC:%.cc=%.o)
EXAMPLE = tscb-integration

CONFIG = include/dbuscc/config.h

all: $(TARGET) $(TESTS) $(EXAMPLE)

$(OBJ): $(CONFIG)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LDFLAGS) -shared $(LIBS)

$(TESTS): $(TARGET) $(TEST_OBJ)
	$(CXX) -o $@ $(TEST_OBJ) $(LDFLAGS) $(LIBS) -ldbuscc

$(EXAMPLE): $(TARGET) $(EXAMPLE_OBJ)
	$(CXX) -o $@ $(EXAMPLE_OBJ) $(LDFLAGS) $(LIBS) -ldbuscc

run_tests: $(TESTS)
	LD_LIBRARY_PATH=. ./$(TESTS)

run_valgrind: $(TESTS)
	LD_LIBRARY_PATH=. valgrind --leak-check=full ./$(TESTS)

run_gdb: $(TESTS)
	LD_LIBRARY_PATH=. gdb ./$(TESTS)

run_example: $(EXAMPLE)
	LD_LIBRARY_PATH=. $(EXAMPLE)

clean:
	rm -rf $(OBJ) $(TARGET) $(TEST_OBJ) $(TESTS) $(EXAMPLE_OBJ) $(EXAMPLE)

.PHONY: all clean

