## lparselib makefile
## author: rconfalonieri
## 15/12/2009

INSTALLATION_PATH=/usr/local/bin

all: clean lparselib
#all: clean liblparsestaticlib

lparse :
	cd src ; make lparse; cd ..

liblparsestaticlib :
	cd src ; make liblparsestaticlib; cd ..

	
lparselib :
	cd src ; make liblparselib; cd ..


#check :
#	cd test; make; cd ..

clean :
	cd src; make clean; cd ..
	#cd test; make clean; cd ..
	rm -rf dist/

#install : lparse
#	cp src/lparse $(INSTALLATION_PATH)/
