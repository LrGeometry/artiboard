rm()
# concat two strings
concat <- function (a,b) {
	paste(a,b,sep="")
}
library(reshape) 
library(xtable)
library(MASS)

baseDir <- "..\\..\\..\\Artiboard\\"
sourceDir <- concat(baseDir,"experiments\\")
outDir <- "..\\artifacts\\"