.PHONY: all release clean run_dd
.DEFAULT_GOAL:=all
.EXTRA_PREREQS=Makefile

CXXFLAGS_STD:=-std=c++14
CXXFLAGS_OPT:=-Os -g --coverage
CPPFLAGS_DEPS:=-MMD -MP

DIR_BUILD:=build

DIR_OBJS:=$(DIR_BUILD)/objs
DIR_SRC:=src

EXEC_DD:=$(DIR_BUILD)/dd.out
EXECS+=$(EXEC_DD)
# Library support
LDLIBS_DD:=$(addprefix -l,SDL2 SDL2_image gcov stdc++)
# Standard
CXXFLAGS_DD:=$(CXXFLAGS_STD) $(CXXFLAGS_OPT)
# Make sure we handle dependencies.
# MMD gets .d files, MP adds header handling
# To move the file to a uniform place, add -MF
CPPFLAGS_DD:=$(CPPFLAGS_DEPS)

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
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^ 
	echo Made   $@

$(EXECS) :
	echo Making $@
	$(MKDIR) $(@D)
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@
	echo Made $@

run_dd :
	./$(EXEC_DD)
	
clean :
	rm -r $(DIR_BUILD)
