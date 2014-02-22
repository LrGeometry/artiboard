source("common.r")
inFile <- concat(sourceDir,"c4-300.txt")
outFile <- concat(outDir,"c4-300")

r <- read.table(inFile,header=TRUE,sep=" ")
attach(r);

# http://stackoverflow.com/questions/1890215/getting-r-plots-into-latex
postscript(file=concat(outFile,"cc.eps"), width = 6.0, height = 5.0,
           horizontal = FALSE,  paper = "special",
           family = "ComputerModern", encoding = "TeXtext.enc");
plot(cutoff,certainty,xaxp=c(0,288,9),xaxs="i",type="p");
sr = split(r,r$fraction);
for (e in sr) lines(e$cutoff,e$certainty,lty=e[[1]][1]-2);
dev.off();

postscript(file=concat(outFile,"cs.eps"),width = 6.0, height = 5.0,
           horizontal = FALSE,  paper = "special",
           family = "ComputerModern", encoding = "TeXtext.enc");
plot(cutoff,size,xaxp=c(0,288,9),xaxs="i",type="p");
sr = split(r,r$fraction);
for (e in sr) lines(e$cutoff,e$size,lty=e[[1]][1]-2);
dev.off();

detach(r);


