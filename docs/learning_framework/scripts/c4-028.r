source("common.r")
inFile <- concat(sourceDir,"c4-028.txt")
outFile <- concat(outDir,"c4-028")

r <- read.table(inFile,header=TRUE,sep=" ")
r$LogSize = log(r$Size)
r$LogAccuracy = log(r$Accuracy)
sr = split(r,r$Encoding)

eps_file(outFile,"acc.eps")
attach(r)
plot(Cutoff,Accuracy,type="n");
detach(r)

lt=0;
for (e in sr) {
	attach(e)
	lt=lt+1
	lines(Cutoff,Accuracy,lty=lt,col=lt,type="o")
	detach(e)
}
legend("topright", legend = names(sr), text.width = strwidth("WWW"), lty = 1:4, xjust = 1, yjust = 1, title = "Encoding", col=1:4);
dev.off()

eps_file(outFile,"size.eps")
attach(r)
plot(Cutoff,LogSize,type="n");
detach(r)

lt=0;
for (e in sr) {
	attach(e)
	lt=lt+1
	lines(Cutoff,LogSize,lty=lt,col=lt,type="o")
	detach(e)
}
legend("topright", legend = names(sr), text.width = strwidth("WWW"), lty = 1:4, xjust = 1, yjust = 1, title = "Encoding", col=1:4);

dev.off()
