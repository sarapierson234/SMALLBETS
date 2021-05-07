import mmap
import os
import shutil
import time
import pandas as pd
import csv
#import scrim2bell  #test needs to change to scrim2bell when it gets fixed fully
#import first_impulse from scrim2bell #here as well
## note that I left the two lines above even though they do not work. While you might be able to tell what I was trying to do--import the scrim2bell & its results into this file, those two lines simply dont accomplish that. 
#from Python_misc.pyat.test.bellhop_test import scrim2bell
#source home.buzz.Python_misc.pyat.test.bellhop_test.scrim2bell import first_impulse
#none of the above work in any configuration that I could look up online or come up with myself

with open ('/home/buzz/Python_misc/pyat/test/bellhop_test/first_impulse.csv', newline='') as csvfile:
	amplitude_n_delay = csv.reader('csvfile', delimiter=',')
amplitude_n_delay = str(amplitude_n_delay)
print(amplitude_n_delay)

def main():
    # Create new empty file to back memory map on disk
    fd = os.open(os.path.expanduser("~")+'/.scrimmage/logs/latest/mmapFile.txt', os.O_RDWR)
    loop = 1
    counter = 0
    while loop:
        if counter > 30:
            loop = 0
        
        with mmap.mmap(fd, 0, mmap.MAP_SHARED, mmap.ACCESS_WRITE) as mmap_buf:
            tmp = mmap_buf.read()
            tmp2 = tmp.decode()
            
            if tmp2[0] == '1':
                #impulse_list = pd.read_csv(r'~/Python_misc/pyat/test/bellhop_test/first_impulse.csv')
                #print(impulse_list)
                
		## Call scrim2bell.py  (this takes the env file from scrimmage, runs bellhop and outputs first impulse)
		## either skip writing to csv or write to csv and import
                tmp2 = '0' + 'amplitude_n_delay' + tmp2[6:]
                tmp = tmp2.encode()
                mmap_buf[:] = tmp #push message to mmap file
                mmap_buf.flush() #ensure buffer is flushed and file is writen
                counter = 0 #reset timout counter
                print("Message sent to scrimmage\n")
            elif tmp2[0] == 9:
                loop = 0
            else:
                print("Waiting for Scrimmage\n")
                time.sleep(1)
                counter += 1
                mmap_buf.flush()
        
if __name__ == '__main__':
    main()
