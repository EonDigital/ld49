.PHONY: all release run_dd

DIR_BUILD:=build

DIR_OBJS:=$(DIR_BUILD)/objs
DIR_SRC:=src

EXEC_DD:=$(DIR_BUILD)/dd.out
EXECS+=$(EXEC_DD)
LDLIBS_DD:=$(addprefix -l,SDL2)
CXXFLAGS_DD:=-std=c++14
CPPFLAGS_DD:=-MMD

print=$(info $(1) $($(1)))

SRC_DD:=$(foreach d,$(DIR_SRC),$(wildcard $(d)/*.cpp))
DIR_OBJS_DD:=$(DIR_OBJS)/dd
OBJS_DD:=$(patsubst $(DIR_SRC)/%,$(DIR_OBJS_DD)/%.o,$(SRC_DD))
DIR_OBJS:=$(dir $(OBJS_DD))

MKDIR:=mkdir -p

all: $(EXECS)
	echo "Build $@ completed"

$(EXEC_DD) : $(OBJS_DD)
$(EXEC_DD) : LDLIBS:=$(LDLIBS_DD)
$(EXEC_DD) $(OBJS_DD) : CPPFLAGS:=$(CPPFLAGS_DD)
$(EXEC_DD) $(OBJS_DD) : CXXFLAGS:=$(CXXFLAGS_DD)

$(OBJS_DD) : $(DIR_OBJS_DD)/%.cpp.o : $(DIR_SRC)/%.cpp
	echo Making $@
	$(MKDIR) $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $^ -o $@
	echo Made   $@

$(EXECS) :
	echo Making $@
	$(MKDIR) $(@D)
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@
	echo Made $@

run_dd :
	./$(EXEC_DD)
