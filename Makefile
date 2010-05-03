## lparselib makefile
## author: rconfalonieri
## 03/05/2010


all: 
	cd computeLppod ; make preprocLPPODparse; make posPsmodels ; cd computeLppod ;
	@echo '*********************************************'
	@echo 'posPsmodels v0.1'
	@echo 'Usage:'
	@echo './posPsmodels lppod/program_name.lppod'
	@echo ''
	@echo 'To test the solver try one of the available LPPODs programs, e.g. ./posPsmodels lppod/foiks2010.lppod '
	@echo ''
	@echo 'To use the solver, put an LPPOD in the lppod folder and run the solver, e.g. ./posPsmodels lppod/your_program.lppod '
	@echo '*********************************************'
	