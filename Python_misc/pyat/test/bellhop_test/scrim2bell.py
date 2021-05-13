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
import csv
#import ~/path/to/OceanParams.csv

def scrim2bell():
#when I figure out where the OceanParams.csv file gets printed to when we run scrimmage, then we have to change the below filename (and add a different file path)
data = pd.read_csv("~/OceanParam_example.csv", sep='\t')
#data = data.to_numpy()
#^^I tried this above but converting immediately to numpy invalidates the parsing by header scheme I am using below. In an effort to not waste a bunch of time bc this has been taking me forever I am just hard coding the conversion to numpy into each input data line *that contains it
"""Basic source and receiver paramters"""
sd = data['z1'][2]*-1
rd = data['z2'][2]*-1

"""now in the following lines we create a grid for simulation"""

Z = data['z']*-1
Z = Z.to_numpy()
X = data['x_slice']
X = X.to_numpy()

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

end_depth = (data['z'].iloc[-1])*-1
depth = [0, end_depth] #layer depths (so the depth of the "surface" layer and the bottom layer
z1 = Z #SSP depths, alphaR is the corresponding SSP values 
#note z1 and alphaR need to be ==length
alphaR = data['c_z']
alphaR = alphaR.to_numpy()
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
#note that I dont have to change the above 2 variables to .to_numpy() because their type is already numpy.float64 ;) 
cInt = cInt(low, high)
RMax = max(X)
freq = 3000

# Beam params
run_type = 'A' #this is what get the .arr output
	#Note that Karim asked to use Eigen rays but this can not be done because option 'A' is the only option that will output the .arr file (with the amplitudes and travel times)
nbeams = 100 #originally at 100
alpha = np.linspace(-20,20, 100)# min and max launch angle -20 degrees to 20 degrees
box = Box( 1000, 2) #bound the region you let the beams go, depth in meters and range in km
deltas=0# length step of ray trace, 0 means automatically choose
beam = Beam(RunType=run_type, Nbeams=nbeams, alpha=alpha,box=box,deltas=deltas)# package

print(type(pos))
print(pos.r.depth) 
write_env('scrim2bell_env.env', 'BELLHOP', 'Pekeris profile', freq, ssp, bdy, pos, beam, cInt, RMax)
 

system("wine64 ~/Downloads/AcousticToolbox/bin/bellhop.exe scrim2bell_env")

#[x,x,x,x,ppos, p] = read_shd("scrim2bell_env.shd")
[Arr, Pos] = read_arrivals_asc("scrim2bell_env.arr") #option 2 here is the number of arrivals allowed (typically 96? what does mbp 9/96 mean in the .m file?)

AmpDel = []
for i in Arr:
	#print(i)
	if len(i) != 0:
		#append a 0 if you want to document the empty first whatever odd arrivals 
        	AmpDel.append([i[0][0], i[0][1]]) #no reflections (echos) considered
		#we can also get (future) angle
AD = pd.DataFrame(AmpDel)
#AD.to_csv(bell2scrim.csv)
pd.DataFrame(AmpDel).to_csv('bell2scrim.csv')
#Here is where we print the amplitude and delay into the csv file to be passed back to scrimmage
#print(AmpDel[1]) 
#^^^this doesnt have to be done this way, which is why I commented it out. note the line below creates a variable that gets imported into the smallbets_mmap.py code

first_impulse_list = AmpDel[1]
#the above is type "list" currently
print(first_impulse_list)
#first_impulse_array = np.array(first_impulse_list)
with open('first_impulse.csv', 'w', newline='') as f:
	writer = csv.writer(f)	
	writer.writerow(first_impulse_list) 

#print(p.shape)
#p = abs(p)
#p = 10*np.log10(p/np.max(p))
#print(p)
#levs = np.linspace(-30, 0, 20)
#plt.contourf(np.squeeze(p), levels=levs)
#plt.gca().invert_yaxis()
#plt.show()
