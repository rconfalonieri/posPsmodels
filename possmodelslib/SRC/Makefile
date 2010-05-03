CXX = g++
CXXFLAGS = -O3 -W -Wall -I ../../smodels-2.28/
LFLAGS =
SRCS = MyAtom.cpp AtomSet.cpp PosAtom.cpp PosAtomSet.cpp MyRule.cpp  PosRule.cpp PosRuleVector.cpp Dictionnary.cpp  PosLogProg.cpp ApiExt.cpp LogProgReader.cpp lpio.cpp 
SRCS_MAIN = $(SRCS) Main.cpp 
SRCS_LIB = $(SRCS) possmodels.cpp
OBJS_MAIN = $(SRCS_MAIN:.cpp=.o)
OBJS_LIB = $(SRCS_LIB:.cpp=.o)
DIST_DIR = dist
MKDIR = mkdir
SMODELSSRCS = smodels.cc stack.cc dcl.cc atomrule.cc read.cc \
queue.cc timer.cc list.cc improve.cc program.cc api.cc stable.cc \
tree.cc denant.cc
SMODELSOBJS = $(SMODELSSRCS:%.cc=../../smodels-2.28/%.o)


#all: distclean posSmodels preprocLparse

posSmodels: $(OBJS_MAIN)
	$(CXX) $(LFLAGS) -o "../posSmodels" $(OBJS_MAIN) $(SMODELSOBJS) -lfl
	

libpossmodelslib: distclean $(OBJS_LIB)
	@echo 'Building target: $@'
	@echo 'Invoking: MacOS X C++ Linker'
	$(MKDIR) -p ../$(DIST_DIR)
	$(CXX) -dynamiclib -install_name libpossmodelslib.dylib -o ../$(DIST_DIR)/libpossmodelslib.dylib -fPIC $(OBJS_LIB) $(SMODELSOBJS) -lfl
	#$(CXX)  $(CXXFLAGS) -dynamiclib -o "libpsmodelslib.dylib" $(OBJS) 
	@echo 'Finished building target: $@'
	@echo ' '


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< 

preprocLparselib: preprocLparse.o
	$(CXX) $(LFLAGS) -dynamiclib -install_name libpreprocLparselib.dylib -o ../$(DIST_DIR)/libpreprocLparselib.dylib -fPIC preprocLparse.o -lfl

preprocLparse: preprocLparse.o
	$(CXX) $(LFLAGS) -o "../preprocLparse" preprocLparse.o -lfl

preprocLparse.cpp: preprocLparse.y preprocLparse.lex.c
	bison -o preprocLparse.cpp preprocLparse.y

preprocLparse.lex.c: preprocLparse.l
	flex -o preprocLparse.lex.c preprocLparse.l

clean:
	#rm -f $(OBJS_MAIN) posSmodels.o ../posSmodels preprocLparse.o ../preprocLparse
	rm -f $(OBJS_LIB) preprocLparse.o 

distclean: clean
	rm -f preprocLparse.cpp preprocLparse.lex.c 

