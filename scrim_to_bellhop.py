"""#
Sara Pierson, Georgia Institute of Technology

This code is meant to be a skeleton code for taking in an input file from scrimmage and passing it to bellhop for assessment of acoustic properties through water, then passing that information (likely impulse response and amplitude assessment) back to scrimmage. 
"""
import numpy as np
from math import pi
from time import sleep
import matplotlib.pyplot as plt
import wave


scrim_file = open("inputfile.txt", "r")
#note that the input file must be in the format necessary for bellhop, or we must extract that information and make a file. 

#line one should contain the name of the file
bell_file = open("bell_file.txt", "a")
bell_file(1) = scrim_file.read(1) #assuming that the first line of the scrimmage file is the name of the bellhop files
bell_file(2) = 5000 #the second line of the bellhop file should be the frequency of transmission. we are going to keep that at 5kHz for the moment being
bell_file(3) = 1
bell_file(4) = "LVW"
#after the fourth line there is a long list of ssp in 6 columns with the other terrain data necessary. Some of the columns are optional, so this depends on what we can get from the terrain data
bell_file() = scrim_file.read([n]) #this function essentially will read the rest of the scrimmage file that is being passed to bellhop and write the ssp/column info into the bellhop file after that we just have to fill in the rest of the bellhop information 
