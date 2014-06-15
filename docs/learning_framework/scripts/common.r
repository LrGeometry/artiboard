rm()
# concat two strings
concat <- function (a,b) {
	paste(a,b,sep="")
}
library(reshape) 
library(xtable)
library(MASS)

eps_file <- function(fileNamePrefix, filename) {
	postscript(file=concat(fileNamePrefix,filename), width = 6.0, height = 4.0, horizontal = FALSE,  paper = "special", family = "ComputerModern", encoding = "TeXtext.enc");
}	
baseDir <- "..\\..\\..\\Artiboard\\"
sourceDir <- concat(baseDir,"experiments\\")
outDir <- "..\\artifacts\\"