YACC = bison --defines=parse.cc.h
DLFLAG = -ldl
CFLAGS=-g -O3
LDFLAGS=-g -O3
OBJFILES=array.o instance.o literal.o priority.o term.o debug.o \
         iterate.o lparse.o rule.o error.o lex.o parse.o sm_smodels.o \
		 extern.o library.o parsetree.o smodels.o graph.o list.o  \
		 predicate.o symbol.o
OBJMAINFILES=array.o instance.o literal.o priority.o term.o debug.o \
         iterate.o main.o rule.o error.o lex.o parse.o sm_smodels.o \
		 extern.o library.o parsetree.o smodels.o graph.o list.o  \
		 predicate.o symbol.o

CC = g++
LEX = flex

lparse :clean $(OBJMAINFILES)
	$(CC)  $(LDFLAGS) -o lparse $(OBJMAINFILES) $(DLFLAG)
	
liblparselib: $(OBJFILES)
	@echo 'Building target: $@'
	@echo 'Invoking: MacOS X C++ Linker'
	mkdir -p ../dist
	$(CXX) -dynamiclib -install_name liblparselib.dylib -o ../dist/liblparselib.dylib -fPIC $(OBJFILES) -lfl
	
	@echo 'Finished building target: $@'
	@echo ' '
	
liblparsestaticlib: $(OBJFILES)
	@echo 'Building target: $@'
	@echo 'Invoking: MacOS X C++ Linker'
	#$(MKDIR) -p $(DIST_DIR)
	#$(CXX) -dynamiclib -install_name libpsmodelslib.dylib -o $(DIST_DIR)/libpsmodelslib.dylib -fPIC $(OBJS)
	#ar rc libpsmodelsstaticlib.a $(OBJS)
	libtool -o liblparsestaticlib.a -static $(OBJFILES) -lfl
	@echo 'Finished building target: $@'
	@echo ' '

.SUFFIXES: .cc 

.ll.cc :


.cc.o:
	$(CC) $(CFLAGS) -c $<

clean :
	
	rm -f *.o *~
	rm -f *.a 

lex.cc : lex.ll parse.yy
	$(LEX) -o lex.cc lex.ll

parse.h : parse.cc

parse.cc : parse.yy parsetree.h debug.h global.h defines.h \
  extern.h term.h literal.h list.h instance.h iterate.h rule.h array.h \
  priority.h symbol.h error.h
	$(YACC) -o parse.cc parse.yy && mv parse.cc.h parse.h


array.o: array.cc config.h global.h defines.h extern.h array.h debug.h \
  error.h term.h predicate.h list.h literal.h instance.h iterate.h rule.h \
  priority.h graph.h smodels.h
debug.o: debug.cc config.h debug.h global.h defines.h extern.h error.h \
  term.h
error.o: error.cc config.h error.h term.h global.h defines.h extern.h \
  debug.h list.h symbol.h predicate.h literal.h instance.h iterate.h \
  rule.h array.h priority.h graph.h smodels.h
extern.o: extern.cc global.h config.h defines.h extern.h instance.h \
  debug.h error.h term.h
graph.o: graph.cc config.h predicate.h global.h defines.h extern.h list.h \
  literal.h term.h instance.h debug.h iterate.h rule.h array.h priority.h \
  graph.h smodels.h error.h symbol.h
instance.o: instance.cc config.h error.h term.h global.h defines.h \
  extern.h instance.h debug.h symbol.h predicate.h list.h literal.h \
  iterate.h rule.h array.h priority.h graph.h smodels.h
iterate.o: iterate.cc config.h global.h defines.h extern.h iterate.h \
  instance.h debug.h error.h term.h
lex.o: lex.cc config.h parsetree.h debug.h global.h defines.h extern.h \
  term.h literal.h list.h instance.h iterate.h rule.h array.h priority.h \
  symbol.h parse.h error.h
library.o: library.cc ../lib/lparse.h config.h library.h defines.h \
  symbol.h debug.h error.h term.h global.h extern.h predicate.h list.h \
  literal.h instance.h iterate.h rule.h array.h priority.h graph.h \
  smodels.h
list.o: list.cc config.h list.h error.h term.h global.h defines.h \
  extern.h debug.h rule.h literal.h instance.h iterate.h array.h \
  priority.h predicate.h graph.h smodels.h parsetree.h library.h
literal.o: literal.cc config.h debug.h literal.h defines.h term.h \
  global.h extern.h list.h instance.h iterate.h parsetree.h rule.h \
  array.h priority.h symbol.h predicate.h graph.h smodels.h error.h
 
##added
lparse.o: lparse.cc config.h global.h defines.h extern.h symbol.h debug.h \
  library.h parsetree.h term.h literal.h list.h instance.h iterate.h \
  rule.h array.h priority.h graph.h predicate.h smodels.h error.h

## original
main.o: main.cc config.h global.h defines.h extern.h symbol.h debug.h \
  library.h parsetree.h term.h literal.h list.h instance.h iterate.h \
  rule.h array.h priority.h graph.h predicate.h smodels.h error.h
parse.o: parse.cc config.h parsetree.h debug.h global.h defines.h \
  extern.h term.h literal.h list.h instance.h iterate.h rule.h array.h \
  priority.h symbol.h error.h
parsetree.o: parsetree.cc config.h parsetree.h debug.h global.h defines.h \
  extern.h term.h literal.h list.h instance.h iterate.h rule.h array.h \
  priority.h predicate.h graph.h smodels.h symbol.h error.h
predicate.o: predicate.cc config.h predicate.h global.h defines.h \
  extern.h list.h literal.h term.h instance.h debug.h iterate.h rule.h \
  array.h priority.h graph.h smodels.h symbol.h error.h
priority.o: priority.cc priority.h list.h config.h defines.h array.h \
  global.h extern.h literal.h term.h instance.h debug.h iterate.h rule.h \
  predicate.h graph.h smodels.h symbol.h error.h
rule.o: rule.cc config.h global.h defines.h extern.h rule.h list.h \
  literal.h term.h instance.h debug.h iterate.h array.h priority.h \
  symbol.h graph.h predicate.h smodels.h error.h
sm_smodels.o: sm_smodels.cc sm_smodels.h
smodels.o: smodels.cc smodels.h literal.h config.h defines.h term.h \
  global.h extern.h list.h instance.h debug.h iterate.h rule.h array.h \
  priority.h predicate.h graph.h sm_smodels.h
symbol.o: symbol.cc config.h library.h defines.h debug.h symbol.h term.h \
  global.h extern.h list.h error.h
term.o: term.cc config.h term.h global.h defines.h extern.h symbol.h \
  debug.h parsetree.h literal.h list.h instance.h iterate.h rule.h \
  array.h priority.h error.h graph.h predicate.h smodels.h
