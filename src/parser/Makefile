#==========================================================================
#                         Directories and names 
# ==========================================================================

PARSER_PREFIX = Bookshelf
LIB_PREFIX = bookshelf
DEBUG_PREFIX = $(shell echo $(PARSER_PREFIX) | tr a-z A-Z)# lower case to upper case 
OBJDIR = ./obj
MKDIR = if [ ! -d $(@D) ]; then mkdir -p $(@D); fi

#==========================================================================
#                         Compilation Flags
# ==========================================================================

# default DBG is off
DBG = 0

ifeq ($(DBG), 1)
	CXXFLAGS = -g3 -fno-inline -std=c++11
else
	CXXFLAGS = -O3 -std=c++11
endif

#==========================================================================
#                         Special Library
# ==========================================================================

INCLUDE = -I $(realpath .)

# ==========================================================================
#                         Standard Setting
# ==========================================================================

# parser.cc and scanner.cc are generated 
SRCS = $(wildcard *.cc) $(PARSER_PREFIX)Parser.cc $(PARSER_PREFIX)Scanner.cc
OBJS = $(SRCS:%.cc=$(OBJDIR)/%.o)
DEPS = $(OBJS:%.o=%.d) 	# one dependency file for each source


all: lib$(LIB_PREFIX)parser

# Generate scanner and parser

$(PARSER_PREFIX)Parser.cc: $(PARSER_PREFIX)Parser.yy
	@$(YACC) --version
	$(YACC) -o $(PARSER_PREFIX)Parser.cc --defines=$(PARSER_PREFIX)Parser.h $(PARSER_PREFIX)Parser.yy

$(PARSER_PREFIX)Scanner.cc: $(PARSER_PREFIX)Scanner.ll
	@$(LEX) --version
	$(LEX) -o $(PARSER_PREFIX)Scanner.cc $(PARSER_PREFIX)Scanner.ll

# Compile dependency 

$(OBJDIR)/%.d: %.cc
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) $< -MM -MT $(@:%.d=%.o) >$@ $(INCLUDE)
$(OBJDIR)/$(PARSER_PREFIX)Parser.d: $(PARSER_PREFIX)Parser.cc
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) $< -MM -MT $(@:%.d=%.o) >$@ $(INCLUDE)
$(OBJDIR)/$(PARSER_PREFIX)Scanner.d: $(PARSER_PREFIX)Scanner.cc $(PARSER_PREFIX)Parser.cc
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) $< -MM -MT $(@:%.d=%.o) >$@ $(INCLUDE)

-include $(DEPS)

# Implicit rule to compile c++ files

$(OBJDIR)/%.o: %.cc
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INCLUDE) 

# Link executable

lib$(LIB_PREFIX)parser: $(OBJS)
	$(AR) $(ARFLAGS) lib$(LIB_PREFIX)parser.a $(OBJS)

.PHONY: clean
clean: cleandep
	rm -f $(OBJS) $(OBJS_TEST)

.PHONY: cleandep
cleandep:
	rm -f $(DEPS) $(DEPS_TEST)

.PHONY: extraclean
extraclean: clean
	rm -f lib$(LIB_PREFIX)parser.a
	rm -f $(PARSER_PREFIX)Parser.cc $(PARSER_PREFIX)Parser.h $(PARSER_PREFIX)Scanner.cc 
	rm -f location.hh position.hh stack.hh
