## Possmodelslib makefile
## author: rconfalonieri
## 15/12/2009

INSTALLATION_PATH=/usr/local/bin

all: clean possmodelslib

possmodelslib :
	cd ../smodels-2.28/ ; make all; cd ..;
	cd SRC ; make libpossmodelslib; make preprocLparselib; cd ..;

possmodels: 
	cd ../smodels-2.28/ ; make all; cd ..;
	cd SRC; make posSmodels; make preprocLparse; cd ..;

#check :
#	cd test; make; cd ..

clean :
	rm -rf dist/
	cd SRC; make distclean; cd ..;
	#rm posSmodels preprocLparse;
	#cd test; make clean; cd ..
	

#install : lparse
#	cp src/lparse $(INSTALLATION_PATH)/
