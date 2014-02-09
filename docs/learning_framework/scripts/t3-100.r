source("common.r")
inFile <- concat(sourceDir,"t3-100.txt")
outFile <- concat(outDir,"t3-100.tex")

r <- read.table(inFile,header=TRUE,sep=" ")

t <- cast(r,Depth~Method,value="Positions")
attach(t)
t$Ratio = Negamax / NegamaxAB
detach(t)
include.rownames=FALSE
outF <- file(outFile)
print(xtable(t),file=outF,include.rownames=FALSE)
close(outF)