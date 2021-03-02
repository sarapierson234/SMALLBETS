"""
Sara Pierson, Georgia Institute of Technology, SMALLBETS
Spring 2021
Python script for running bellhop test on a Munk profile with bathymetry data included. 
We are essentially creating the test.py file (can be found in the pyat/test/bellhop_test directory) with our own input variables. Then this test_smallbetsv1.py will be reduced by bellhop.exe
The code will take in the necessary bellhop information and format it for processing 
Note that this file is modelled directly after test.py by hunterakins
"""
import numpy as np
import sys
sys.path.append("../")
from os import system
from matplotlib import pyplot as plt
from pyat.pyat.env import *
from pyat.pyat.readwrite import *
import pandas as pd
import pandas.read_csv

with open('OceanParam_example.csv') as scrimfile:
	data = csv.reader(scrimfile, delimiter='   ')

"""Basic source and receiver paramters"""
source_depth =  %(extract row 1 column 3)
receiver_depth = (extract row 1 column 6)

"""now in the following lines we create a grid for simulation"""

Z = (extract the entirety of column 10)
X = (extract the entirety of column 7)

cw		=	1500
pw		=	1
aw		=	0
cb		=	1600
pb		=	1.5
ab		=	0.5
"""initialize pyat objs with parameters configured above"""
s = Source(source_depth)
r = Dom(X, Z)  #domain
pos = Pos(s,r)
pos.s.depth	= [sd] 

pos.r.depth	 = Z 
pos.r.range		=	X
pos.Nsd = 1 # number of source ranges
pos.Nrd = len(X) # number of receiver ranges

"""
Set up environment
"""
cw		=	1500 # default sound speed
pw		=	1 # density (kg/m^3
aw		=	0 # atten in water

depth = [first number in Z (column10), last number in column10]

z1 = [extract column 10 as a vector]
alphaR = [extract column 11 as vector]

betaR	=	0.0*np.array([1]*len(z1))		
rho		=	pw*np.array([1]*len(z1))		
alphaI	=	aw*np.array([1]*len(z1))		
betaI	=	0.0*np.array([1]*len(z1))

ssp1 = SSPraw(z1, alphaR, betaR, rho, alphaI, betaI)

#	Sound-speed layer specifications
raw = [ssp1]
NMedia		=	1
Opt			=	'CVW'	
N			=	[0, 0]	
sigma		=	[0, 0]	
raw[0]
ssp = SSP(raw, depth, NMedia, Opt, N, sigma)

# Layer 2
alphaR = 1600 # p wave speed in sediment
betaR = 0 # no shear wave
alphaI = .5 # p wave atten
betaI =0 # s wave atten
rhob = 1600

hs = HS(alphaR, betaR, rhob, alphaI, betaI)
Opt = 'A'
bottom = BotBndry(Opt, hs)
top = TopBndry('CVM')
bdy = Bndry(top, bottom)
low = 1400
high = 1e9
cInt = cInt(low, high)
RMax = max(X)
freq = 3000

# Beam params
run_type = 'A'
nbeams = 100
alpha = np.linspace(-20,20, 100)
box = Box( 5500, 100)
deltas=0
beam = Beam(RunType=run_type, Nbeams=nbeams, alpha=alpha,box=box,deltas=deltas)

write_env('py_env_smallbetsv1.env', 'BELLHOP', 'Pekeris profile', freq, ssp, bdy, pos, beam, cInt, RMax)
  

system("wine64 ~/Downloads/AcousticToolbox/bin/bellhop.exe py_env_smallbetsv1")

[x,x,x,x,ppos, p] = read_shd("py_env_smallbetsv1.shd")

print(p.shape)
p = abs(p)
p = 10*np.log10(p/np.max(p))
print(p)
levs = np.linspace(-30, 0, 20)
plt.contourf(np.squeeze(p), levels=levs)
plt.gca().invert_yaxis()
plt.show()
