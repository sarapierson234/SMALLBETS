import sys
sys.path.append("/home/buzz/Python_misc/pyat/test/bellhop_test/")
import scrim2bell
import mmap
import os
import shutil
import time
import csv
#from .buzz.Python_misc.test.bellhop_test.scrim2bell import scrim2bell

def main():
    # Create new empty file to back memory map on disk
    #fd = open('/home/buzz/.scrimmage/logs/latest/mmapFile.txt', 'w+')
    fd = os.open(os.path.expanduser("~")+'/.scrimmage/logs/latest/mmapFile.txt', os.O_RDWR)
    #fd = os.open('/home/buzz/.scrimmage/logs/latest/mmapFile.txt', os.O_RDWR)
    loop = 1
    counter = 0
    while loop:
        if counter > 30:
            loop = 0
        
        #print(type(fd))
        #print(type(mmap.MAP_SHARED))
        #print(type(mmap.ACCESS_WRITE))
        with mmap.mmap(fd, 0, mmap.MAP_SHARED, mmap.ACCESS_WRITE) as mmap_buf:
            tmp = mmap_buf.read()
            tmp2 = tmp.decode()
            
            if tmp2[0] == '1':
                # Write Pong and 0
                # Call Bellhop python code here and then write infomation to mmap file
                
                #impulse_list = pd.read_csv(r'~/Python_misc/pyat/test/bellhop_test/first_impulse.csv')
                #print(impulse_list)
                
		## Call scrim2bell.py  (this takes the env file from scrimmage, runs bellhop and outputs first impulse)
		## either skip writing to csv or write to csv and import
                scrim2bell.scrim2bell() #you have 
                #from scrim2bell import first_impulse_list
                #print(first_impulse_list)
                #import first_impulse.csv
#this in invalid, it tries to import a library, this is a heading, not a command.                 
                with open ('first_impulse.csv', newline='') as csvfile:
                    a_n_d_data = csv.reader(csvfile, delimiter=',')
                    a_n_d_data = list(a_n_d_data)
                    print(type(a_n_d_data))
                amp = str(a_n_d_data[0][0])
                delay = str(a_n_d_data[0][1])
                #print(amp)
                #print(delay) 
                amplitude_n_delay = amp + delay
                print(len(amplitude_n_delay))
                #print(tmp2)
                #print(type(amplitude_n_delay))
                #print(amplitude_n_delay{1}) #if usingjust square brackets, use these indicies [4:16] & [26:38]
                tmp2 = amplitude_n_delay + tmp2[6:26]
                #print(tmp2) #--> tmp2 is a csv.reader object
		#Write first bit to pass control to scrimmage plus new message
                #tmp2 = '0' + ' Pong' + tmp2[6:] #THIS IS THE OLD MESSAGE LEFT HERE FOR HISTORICAL REASONS, it is 55 char in length
                tmp = tmp2.encode()
                #print(len(tmp2[6:]))
                #print(len(tmp))
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
