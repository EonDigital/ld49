.PHONY: all release

DIR_BUILD:=build

DIR_OBJS:=$(DIR_BUILD)/objs
DIR_SRC:=src

EXEC_DD:=$(DIR_BUILD)/dd.out
EXECS+=$(EXEC_DD)

print=$(info $(1) $($(1)))

SRC_DD:=$(wildcard $(DIR_SRC)/*.cpp)
DIR_OBJS_DD:=$(DIR_OBJS)/dd
OBJS_DD:=$(patsubst $(DIR_SRC)/%,$(DIR_OBJS_DD)/%.o,$(SRC_DD))
DIR_OBJS:=$(dir $(OBJS_DD))

MKDIR:=mkdir -p

all: $(EXECS)
	echo "Build $@ completed"

$(EXEC_DD) : $(OBJS_DD)
$(OBJS_DD) : $(DIR_OBJS_DD)/%.cpp.o : $(DIR_SRC)/%.cpp
	echo Making $@
	$(MKDIR) $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
	echo Made   $@

$(EXECS) :
	echo Making $@
	$(MKDIR) $(@D)
	$(CC) $(LDFLAGS) $< $(LOADLIBES) $(LDLIBS) -o $@
	echo Made $@


