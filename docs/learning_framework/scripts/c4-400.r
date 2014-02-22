source("common.r")
# https://www.harding.edu/fmccown/r/
inFile <- concat(sourceDir,"c4-400.txt")
outFile <- concat(outDir,"c4-400")
r <- read.table(inFile,header=TRUE,sep=" ")
attach(r)
postscript(file=concat(outFile,"size.eps"), width = 6.0, height = 4.0, horizontal = FALSE,  paper = "special", family = "ComputerModern", encoding = "TeXtext.enc");
barplot(size,names.arg=region,ylab="Size",ylim=c(2500,3900),xpd=FALSE);
dev.off()
postscript(file=concat(outFile,"cert.eps"), width = 6.0, height = 4.0, horizontal = FALSE,  paper = "special", family = "ComputerModern", encoding = "TeXtext.enc");
barplot(certainty,names.arg=region,ylab="Certainty",ylim=c(60,80),xpd=FALSE);
dev.off()
detach(r)