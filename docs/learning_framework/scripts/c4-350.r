source("common.r")
inFile <- concat(sourceDir,"c4-350.txt")
outFile <- concat(outDir,"c4-350")

r <- read.table(inFile,header=TRUE,sep=" ")
attach(r);

# http://stackoverflow.com/questions/1890215/getting-r-plots-into-latex
postscript(file=concat(outFile,".eps"), width = 6.0, height = 5.0, horizontal = FALSE,  paper = "special", encoding = "TeXtext.enc");
plot(Depth,Performance,ylim=c(30,100),xaxs="i",type="p");
sr = split(r,r$Function);
i = 1
for (e in sr) {
	lines(e$Depth,e$Performance,lty=i,col=i);
	i = i + 1;
}
legend("bottomright", legend = names(sr), text.width = strwidth("WWWWW"), lty = 1:6, xjust = 1, yjust = 1, title = "Depth", col=1:6);
dev.off();


detach(r);


