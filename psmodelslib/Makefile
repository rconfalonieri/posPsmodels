## psmodelslib makefile
## author: rconfalonieri
## 15/12/2009


# Object Directory
BUILD_DIR=build
DIST_DIR=dist
RM = rm -rf
MKDIR = mkdir
CXX = g++
CXXFLAGS = -g -W -Wall 
LFLAGS = -g 
BUILD_SUBPROJECTS =
SRCS = psmodels.cc smodels.cc stack.cc dcl.cc atomrule.cc read.cc \
       queue.cc timer.cc list.cc improve.cc program.cc api.cc stable.cc \
       tree.cc denant.cc
SRCS_MAIN = main.cc smodels.cc stack.cc dcl.cc atomrule.cc read.cc \
       queue.cc timer.cc list.cc improve.cc program.cc api.cc stable.cc \
       tree.cc denant.cc
OBJS = $(SRCS:.cc=.o)
OBJS_MAIN = $(SRCS_MAIN:.cc=.o)

all: clean libpsmodelslib
#all: clean libpsmodelsstaticlib

libpsmodelslib: $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MacOS X C++ Linker'
	$(MKDIR) -p $(DIST_DIR)
	$(CXX) -dynamiclib -install_name libpsmodelslib.dylib -o $(DIST_DIR)/libpsmodelslib.dylib -fPIC $(OBJS) -lfl
	#$(CXX)  $(CXXFLAGS) -dynamiclib -o "libpsmodelslib.dylib" $(OBJS) 
	@echo 'Finished building target: $@'
	@echo ' '

libpsmodelsstaticlib: $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MacOS X C++ Linker'
	$(MKDIR) -p $(DIST_DIR)
	#$(CXX) -dynamiclib -install_name libpsmodelslib.dylib -o $(DIST_DIR)/libpsmodelslib.dylib -fPIC $(OBJS) 
	ar rc libpsmodelsstaticlib.a $(OBJS) 
	#libtool -o libpsmodelsstaticlib.a -static $(OBJS) -lfl
	@echo 'Finished building target: $@'
	@echo ' '

psmodels: clean $(OBJS_MAIN)
	$(CXX) -o psmodels $(OBJS_MAIN)

clean:
	$(RM) dist/
	$(RM) *.a
	#$(RM) build/
	$(RM) *.o psmodels

%.o: %.cc
	#$(MKDIR) -p $(BUILD_DIR)
	#$(CXX) $(CXXFLAGS) -c -o $@ $<
	$(CXX) -c -Wall -pedantic $<