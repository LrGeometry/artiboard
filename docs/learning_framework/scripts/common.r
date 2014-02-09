rm()
# concat two strings
concat <- function (a,b) {
	paste(a,b,sep="")
}
library(reshape) 
library(xtable)

baseDir <- "..\\..\\..\\Artiboard\\"
sourceDir <- concat(baseDir,"experiments\\")
outDir <- "..\\artifacts\\"