Given an original and edited text file, produces a "revision" text file containing a series of instructions to turn the original into the edited version. Can also take the original and revision files and create the edited file. 
Useful when edits aren't large and maintain most of the original file's text (such as describing day to day inventory), especially if data transfer is expensive or slow. In testing, the algorithm most often produced revision files of sizes ~10% of the original/edited files.

To use, set the value of createrevision to true or false depending on whether you want to create the revision file or the edited file. Then replace the names of the text files in main and run the program to produce the desired result.
