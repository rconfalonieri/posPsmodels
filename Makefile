## lparselib makefile
## author: rconfalonieri
## 03/05/2010


all: 
	cd computeLppod ; make preprocLPPODparse; make posPsmodels ; cd .. ;
	@echo '*********************************************'
	@echo 'posPsmodels v0.1'
	@echo 'Usage:'
	@echo './posPsmodels lppod/program_name.lppod'
	@echo ''
	@echo 'To test the solver try one of the available LPPODs programs, 
	@echo e.g. ./posPsmodels lppod/dessert.lppod '
	@echo ''
	@echo 'To use the solver, put an LPPOD in the lppod folder and run the solver, 
	@echo e.g. ./posPsmodels lppod/your_program.lppod '
	@echo '*********************************************'
	
