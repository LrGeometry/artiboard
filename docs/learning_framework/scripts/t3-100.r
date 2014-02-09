rm()
# concat two strings
concat <- function (a,b) {
	paste(a,b,sep="")
}
library(reshape) 

baseDir <- "..\\..\\Artiboard\\"
sourceDir <- concat(baseDir,"experiments\\")
outDir <- "..\\artifacts\\"

inFile <- concat(sourceDir,"t3-100.txt")
outFile <- concat(outDir,"t3-100.tex")
cat(inFile)
outF <- file(outFile)
writeLines(c("Hello","World"), outF)
close(outF)