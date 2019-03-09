BIN := office-space-tetris
SRC := $(wildcard src/*.cpp)
PREFIX ?= /usr/local

LDFLAGS = -lSDL -lSDL_gfx -lpthread
CFLAGS = -O0 -ggdb
CXXFLAGS = -std=c++14

BUILDDIR := build
OBJ := $(SRC:%.cpp=$(BUILDDIR)/%.o)
DEP := $(OBJ:.o=.d)
BUILDDIRS := $(sort $(dir $(OBJ)))
BUILDDIRS_T := $(addsuffix .touched,$(BUILDDIRS))

all: $(BIN)

$(BIN): $(BUILDDIR)/$(BIN)
	ln -fs $(BUILDDIR)/$(BIN) $(BIN)

$(BUILDDIR)/$(BIN): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(BUILDDIR)/%.o: %.cpp $(BUILDDIRS_T) Makefile
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(CFLAGS) -c -o $@ $<

-include $(DEP)
$(BUILDDIR)/%.d: %.cpp $(BUILDDIRS_T)
	@$(CPP) $(CPPFLAGS) $(CXXFLAGS) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@


$(BUILDDIRS_T):
	mkdir -p $(dir $@)
	touch  $@

# Without this, gnu make will assume .touched files are intermediary
# files and they would have been removed by gnu make on exit.
#.PRECIOUS: $(BUILDDIR)/%.touched

.PHONY: clean
clean:
	rm -rf $(BUILDDIR) $(BIN)
