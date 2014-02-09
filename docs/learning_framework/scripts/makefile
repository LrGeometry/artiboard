REXEC = "C:\Program Files\R\R-2.15.0\bin\x64\Rterm.exe" 
ROPTIONS = --vanilla
R = $(REXEC) $(ROPTIONS)

IDIR = ../../../Artiboard/experiments
ODIR = ../artifacts

OT3-100 = $(ODIR)/t3-100.tex
 
$(OT3-100): t3-100.r $(IDIR)/t3-100.txt
	$(R) -f t3-100.r

