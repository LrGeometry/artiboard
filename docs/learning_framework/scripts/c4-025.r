source("common.r")
inFile <- concat(sourceDir,"c4-025.txt")
outFile <- concat(outDir,"c4-025")

r <- read.table(inFile,header=TRUE,sep=" ")
postscript(file=concat(outFile,"box.eps"), width = 6.0, height = 4.0, horizontal = FALSE,  paper = "special", family = "ComputerModern", encoding = "TeXtext.enc");
boxplot(Measurement ~ Strategy, data = r);
t <- aggregate(r$Measurement,list(Strategy=r$Strategy),mean);
t$var <- aggregate(r$Measurement,list(S=r$Strategy),var)$x;
t$ min <- aggregate(r$Measurement,list(S=r$Strategy),min)$x;
t$ max <- aggregate(r$Measurement,list(S=r$Strategy),max)$x;
outFileT <- file(concat(outFile,"tab.tex"))
xt = xtable(do.call(data.frame, c(t, check.names = FALSE)));
print(xt,file=outFileT);
close(outFileT);

