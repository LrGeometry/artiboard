# In sublime f7 runs this script
# source("c4-07.r")
rm()
library(reshape) # for melt and cast
# concat two strings
concat <- function (a,b) {
	paste(a,b,sep="")
}

baseDir <- "C:\\development\\github\\artiboard\\Artiboard\\"
sourceDir <- concat(baseDir,"experiments\\")
targetDir <- concat(baseDir,"connect4\\articles\\KBDiscovery\\")

inFile <- concat(sourceDir,"c4-070-2.txt")

results <- read.table(inFile,header=TRUE,sep=" ")


cat("\n*****")
cat("\nDoes pruning 32 produce smaller trees than zero pruning?")
r <- results[results$cutoff<60,c(1,2,3)]
s <- cast(r,fraction~cutoff,value='size')
cat("\nThe data points involved are:\n");
print(s)
cat("\nt-test results");
# http://www.csic.cornell.edu/Elrod/t-test/reporting-t-test.html
# alternative = "less" is the alternative that x (32) has a smaller mean than y(0).
t <- t.test(s$'32',s$'0',alternative="less",pair=TRUE)
print(t)
# the p-value is < 0.05 -- the chance of having a false alarm is very small
cat(concat("We are 95% confident that (size of 0) = (size of 32) ", t$conf.int[2]))
cat("\nSo: 32 does produce much smaller trees");

cat("\nDoes pruning 32 produce more certain trees than zero pruning")
r <- results[results$cutoff<60,c(1,2,4)]
s <- cast(r,fraction~cutoff,value='certainty')
cat("\nThe data points involved are:");
print(s)
cat("t-test results");
t <- t.test(s$'32',s$'0',alternative="greater",pair=TRUE)
print(t)
# the p-value is < 0.05 -- the chance of having a false alarm is very small
cat(concat("We are 95% confident that (certainty of 32) = (certainty of 0) +", t$conf.int[1]))
cat("\nSo: 32 does produce more certain trees");

# H: More pruning produces less certainty 
# first lets eliminate zero pruning
r <- results[results$cutoff>0,]
attach(r)
# The relationship is clearly linear
# certainty = (pruning - 722.5) / 10
#c = certainty - (cutoff + 722.5)/10 + 80
# see it in this plot
#plot(certainty~cutoff)
# a possible equation is 
# certainty = -cutoff/10 + 180
detach(r)

# Which pruning should I use?
# 64 produces the best certainty on average
# and has trees that is smaller than 32
m <- melt(r[r$cutoff<200,],id=1:2)
c <- cast(m,cutoff~variable,mean)
print(c)

# Which training fraction should I use?
m <- melt(r[r$cutoff==64,],id=1:2)
print(cast(m,fraction~variable,mean))
# clearly a higher fraction produce a higher certainty,
# but also a larger tree
# but from 1 2 to 3 there is a decrease in with an increase in size
# So, I think it is best to use 1/3 for training
# This would also speed up training (less examples)

# As summary - I'll use
#   cutoff = 64
#   training denominator = 3
s <- r[r$cutoff==64,]
cat("Baseline values\n")
print(s[1,])




