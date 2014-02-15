REXEC = "C:\Program Files\R\R-2.15.0\bin\x64\Rterm.exe" 
ROPTIONS = --vanilla
R = $(REXEC) $(ROPTIONS)

IDIR = ../../../Artiboard/experiments
ODIR = ../artifacts

$(ODIR)/c4-200.tex: c4-200.r $(IDIR)/c4-200.txt
	$(R) -f c4-200.r

$(ODIR)/c4-100.tex: c4-100.r $(IDIR)/c4-100.txt
	$(R) -f c4-100.r
 
$(OT3-100): t3-100.r $(IDIR)/t3-100.txt
	$(R) -f t3-100.r

