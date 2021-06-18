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

data = pd.read_csv("OceanParam_example.csv")

"""Basic source and receiver paramters"""
sd = data['z1'][2]
rd = data['z2'][2]

"""now in the following lines we create a grid for simulation"""

Z = data['z']
X = data['x_slice']

cw		=	1500
pw		=	1
aw		=	0
cb		=	1600
pb		=	1.5
ab		=	0.5
"""initialize pyat objs with parameters configured above"""
s = Source(sd)
r = Dom(X, Z)  #domain
pos = Pos(s,r)
pos.s.depth	= [sd] 

pos.r.depth	 = Z 
pos.r.range		=	X
pos.Nsd = 1 # number of source ranges
pos.Nrd = len(X) # number of receiver ranges


depth = [0, data['z'].iloc[-1]] #layer depths (so the depth of the "surface" layer and the bottom layer
z1 = Z #SSP depths, alphaR is the corresponding SSP values 
#note z1 and alphaR need to be ==length
alphaR = data['c_z']
betaR	=	0.0*np.array([1]*len(z1)) #shear speed at each depth
rho		=	pw*np.array([1]*len(z1)) #rho at each depth-->in future this could also be read-in
alphaI	=	aw*np.array([1]*len(z1)) #atten at each depth	
betaI	=	0.0*np.array([1]*len(z1)) #shear attenuation at each depth


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

#below are the min/max ssp, used only for modal calculations
low = data['c_z'][data['c_z'].idxmin()]
high = data['c_z'][data['c_z'].idxmax()]
cInt = cInt(low, high)
RMax = max(X)
freq = 3000

# Beam params
run_type = 'A' #this is what get the .arr output
nbeams = 100 #originally at 100
alpha = np.linspace(-20,20, 100)# min and max launch angle -20 degrees to 20 degrees
box = Box( 1000, 2) #bound the region you let the beams go, depth in meters and range in km
deltas=0# length step of ray trace, 0 means automatically choose
beam = Beam(RunType=run_type, Nbeams=nbeams, alpha=alpha,box=box,deltas=deltas)# package

write_env('test_smallbetsv1.env', 'BELLHOP', 'Pekeris profile', freq, ssp, bdy, pos, beam, cInt, RMax)
  

system("wine64 ~/Downloads/AcousticToolbox/bin/bellhop.exe py_env")


[x,x,x,x,ppos, p] = read_shd("test_smallbetsv1.shd")
[Arr, Pos] = read_arrivals_asc("test_smallbetsv1.arr") #option 2 here is the number of arrivals allowed (typically 96? what does mbp 9/96 mean in the .m file?)


AmpDel = []
for i in Arr:
	#print(i)
	if len(i) != 0:
		#append a 0 if you want to document the empty first  whatever odd arrivals 
        	AmpDel.append([i[0][0], i[0][1]]) #no reflections (echos) considered
		#we can also get (future) angle
#AD = pd.DataFrame(AmpDel)
#AD.to_csv(bell2scrim.csv)
pd.DataFrame(AmpDel).to_csv('bell2scrim.csv')
#Here is where we print the amplitude and delay into the csv file to be passed back to scrimmage
	
#amplitudes = Arr.A
#delays = Arr.delay
#print(amplitudes)
#print(delays)
print(p.shape)
p = abs(p)
p = 10*np.log10(p/np.max(p))
print(p)
levs = np.linspace(-30, 0, 20)
plt.contourf(np.squeeze(p), levels=levs)
plt.gca().invert_yaxis()
plt.show()


