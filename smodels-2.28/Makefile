CXX = g++
CXXFLAGS = -O3 -Wall -W
LFLAGS =
LIBSRCS = smodels.cc stack.cc dcl.cc atomrule.cc read.cc \
queue.cc timer.cc list.cc improve.cc program.cc api.cc stable.cc \
tree.cc denant.cc
SRCS = $(LIBSRCS) main.cc
OBJS = $(SRCS:.cc=.o)

smodels: $(OBJS)
	$(CXX) $(LFLAGS) -o smodels $(OBJS)

libpsmodelslib: $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MacOS X C++ Linker'
	$(MKDIR) -p $(DIST_DIR)
	$(CXX) -dynamiclib -install_name libpsmodelslib.dylib -o $(DIST_DIR)/libpsmodelslib.dylib -fPIC $(OBJS)
	#$(CXX)  $(CXXFLAGS) -dynamiclib -o "libpsmodelslib.dylib" $(OBJS) 
	@echo 'Finished building target: $@'
	@echo ' '

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $<

.PHONY: lib libinstall clean all build binary install

LIBPATH = /usr/local/lib
LIBOBJS = $(LIBSRCS:.cc=.lo)

# Make a shared library
lib: $(LIBOBJS)
	libtool $(CXX) $(LFLAGS) -o libsmodels.la $(LIBOBJS) \
                -rpath $(LIBPATH)

%.lo: %.cc
	libtool $(CXX) $(CXXFLAGS) -c $<

libinstall:
	libtool install -c libsmodels.la $(LIBPATH)/libsmodels.la

clean:
	rm -f core $(OBJS) $(LIBOBJS) smodels libsmodels.la

all:
	$(MAKE) clean smodels

build: clean

binary: smodels

install: smodels
	install smodels $(DESTDIR)/usr/bin/
