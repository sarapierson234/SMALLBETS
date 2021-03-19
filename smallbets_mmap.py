import mmap
import os
import shutil
import time


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
                # Write Pong and 0
                # Call Bellhop python code here and then write infomation to mmap file

                #Write first bit to pass control to scrimmage plus new message
                tmp2 = '0' + ' Pong' + tmp2[6:]
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
