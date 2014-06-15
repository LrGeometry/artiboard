source("common.r")
inFile <- concat(sourceDir,"c4-026.txt")
outFile <- concat(outDir,"c4-026")

r <- read.table(inFile,header=TRUE,sep=" ")
eps_file(outFile,"acc.eps")
boxplot(Accuracy ~ Cutoff, data = r);
eps_file(outFile,"size.eps")
boxplot(Size ~ Cutoff, data = r);
#t <- aggregate(r$Cutoff,list(Strategy=r$Strategy),mean);
#t$var <- aggregate(r$Cutoff,list(S=r$Strategy),var)$x;
#t$ min <- aggregate(r$Cutoff,list(S=r$Strategy),min)$x;
#t$ max <- aggregate(r$Cutoff,list(S=r$Strategy),max)$x;
#outFileT <- file(concat(outFile,"tab.tex"))
#xt = xtable(do.call(data.frame, c(t, check.names = FALSE)));
#print(xt,file=outFileT);
#close(outFileT);

