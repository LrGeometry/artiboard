source("common.r")
inFile <- concat(sourceDir,"c4-027.txt")
outFile <- concat(outDir,"c4-027")

r <- read.table(inFile,header=TRUE,sep=" ")
attach(r)
eps_file(outFile,"acc.eps")
plot(Cutoff,Accuracy,type="o")
eps_file(outFile,"size.eps")
plot(Cutoff,Size,type="o")
detach()


