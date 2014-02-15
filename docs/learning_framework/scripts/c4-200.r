source("common.r")
inFile <- concat(sourceDir,"c4-200.txt")
outFile <- concat(outDir,"c4-200.tex")

r <- read.table(inFile,header=TRUE,sep=" ")

t <- cast(r,Function~Depth,value="Positions")
attach(t)
# t$Ratio = Negamax / NegamaxAB
detach(t)
include.rownames=FALSE
outF <- file(outFile)
print(xtable(t),file=outF,include.rownames=FALSE)
close(outF)