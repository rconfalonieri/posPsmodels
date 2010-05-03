## computeLPPOD makefile
## author: rconfalonieri
## 15/12/2009

CXXFLAGS =	-O2 -g -Wall
CXX = g++
OBJS = posPsmodels.o
TARGET = posPsmodels
PSMODELSLIB = libpsmodelslib.dylib
LPARSELIB = liblparselib.dylib
LPARSE2LIB = liblparse2lib.dylib
POSSMODELLIB = libpossmodelslib.dylib
PREPROCPARSELIB = libpreprocLparselib.dylib
#LIBS = -L../lparselib -llparsestaticlib -L../possmodelslib/dist -lpossmodelslib -L../possmodelslib/dist -lpreprocLparselib 

LIBS = -L../lparselib/dist -llparselib -L../lparse2lib/dist -llparse2lib -L../psmodelslib/dist -lpsmodelslib -L../possmodelslib/dist -lpossmodelslib -L../possmodelslib/dist -lpreprocLparselib
#LIBS = -L../psmodelslib/dist -lpsmodelslib -L../possmodelslib/dist -lpreprocLparselib 
SRCS = utils.cc lpodmodel.cc parsing.cc lppodprogram.cc lppodrule.cc lppodreduction.cc main.cc 
OBJS = $(SRCS:.cc=.o)


$(TARGET): clean make-libs copy preprocLPPODparse.o $(OBJS) 
	$(CXX) $(CXXFLAGS) $(OBJS) preprocLPPODparse.o $(LIBS) -o  $(TARGET)    
	
preprocLPPODparse.o: preprocLPPODparse.cc
	$(CXX) $(CXXFLAGS) -c $<
	
	
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< 
	
preprocLPPODparse: preprocLPPODparse.o
	$(CXX) $(CXXFLAGS) $(LFLAGS) -o preprocLPPODparse preprocLPPODparse.o -lfl

preprocLPPODparse.cc: preprocLPPODparse.yy preprocLPPODparse.lex.c
	bison -o preprocLPPODparse.cc preprocLPPODparse.yy

preprocLPPODparse.lex.c: preprocLPPODparse.ll
	flex -o preprocLPPODparse.lex.c preprocLPPODparse.ll



copy: 
	cp ../psmodelslib/dist/$(PSMODELSLIB) .
	#cp ../psmodelslib/libpsmodelsstaticlib.a .
	#cp ../psmodels/psmodels .
	#cp ../lparselib/src/lparse .
	cp ../lparselib/dist/$(LPARSELIB) .
	cp ../lparse2lib/dist/$(LPARSE2LIB) .
	#cp ../lparselib/src/liblparsestaticlib.a .
	cp ../possmodelslib/dist/$(POSSMODELLIB) .
	cp ../possmodelslib/dist/$(PREPROCPARSELIB) .
	
make-libs:
	#cd ../lparselib; make all;
	#cd ../psmodelslib; make all; 
	#cd ../possmodelslib; make all; 
	#cd ../computeLPPOD;

clean:
	rm preprocLPPODparse.o preprocLPPODparse.cc preprocLPPODparse.lex.c
	rm -f $(OBJS) $(TARGET) *.dylib *.a
	#rm -f $(OBJS) $(TARGET)  
