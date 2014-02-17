source("common.r")
inFile <- concat(sourceDir,"c4-300.txt")
outFile <- concat(outDir,"c4-300")

r <- read.table(inFile,header=TRUE,sep=" ")
attach(r);
# http://stackoverflow.com/questions/1890215/getting-r-plots-into-latex
pdf(concat(outFile,"cc.pdf"));
plot(cutoff,certainty,xaxp=c(0,288,9),xaxs="i",type="p");
sr = split(r,r$fraction);
for (e in sr) lines(e$cutoff,e$certainty,lty=e[[1]][1]-2);
dev.off();

pdf(concat(outFile,"cs.pdf"));
plot(cutoff,size,xaxp=c(0,288,9),xaxs="i",type="p");
sr = split(r,r$fraction);
for (e in sr) lines(e$cutoff,e$size,lty=e[[1]][1]-2);
dev.off();

detach(r);


