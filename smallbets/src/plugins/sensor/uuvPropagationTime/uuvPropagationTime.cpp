/*!
 * @file
 *
 * @section LICENSE
 *
 * Copyright (C) 2017 by the Georgia Tech Research Institute (GTRI)
 *
 * This file is part of SCRIMMAGE.
 *
 *   SCRIMMAGE is free software: you can redistribute it and/or modify it under
 *   the terms of the GNU Lesser General Public License as published by the
 *   Free Software Foundation, either version 3 of the License, or (at your
 *   option) any later version.
 *
 *   SCRIMMAGE is distributed in the hope that it will be useful, but WITHOUT
 *   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 *   License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with SCRIMMAGE.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Kevin DeMarco <kevin.demarco@gtri.gatech.edu>
 * @author Eric Squires <eric.squires@gtri.gatech.edu>
 * @date 31 July 2017
 * @version 0.1.0
 * @brief Brief file description.
 * @section DESCRIPTION
 * A Long description goes here.
 *
 */

#include <smallbets/plugins/sensor/uuvPropagationTime/uuvPropagationTime.h>

#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/entity/Entity.h>
#include <scrimmage/math/State.h>
#include <scrimmage/parse/ParseUtils.h>
#include <scrimmage/parse/MissionParse.h>
#include <scrimmage/pubsub/Message.h>
#include <scrimmage/proto/State.pb.h>
#include <scrimmage/common/Random.h>
#include <scrimmage/math/Quaternion.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <fstream>
#include <stdio.h>
#include <cstdio>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <iostream>
#include <limits>

using std::cout;
using std::endl;

namespace sc = scrimmage;

REGISTER_PLUGIN(scrimmage::Sensor,
                scrimmage::sensor::uuvPropagationTime,
                uuvPropagationTime_plugin)

namespace scrimmage {
namespace sensor {

uuvPropagationTime::uuvPropagationTime() {
}

void uuvPropagationTime::init(std::map<std::string, std::string> &params) {
	 std::string tmp = parent_->mp()->log_dir() + "/" + "mmapFile.txt";
     FILE *fid = fopen(tmp.c_str(), "r");
     // If file does not exist, Create new file
      if (fid == NULL) 
      {
        cout << "Cannot open file, file does not exist. Creating new file..";
        fid = fopen(tmp.c_str(),"w+b");
        fprintf(fid,"0                                                      ");//Initialize memory in file
		fclose(fid);
       } 
      else   
      {    // use existing file
         std::cout<<"success mmapFile.txt found." << std::endl;
         fclose(fid);
    }

}

bool uuvPropagationTime::step() {
    struct stat sb;
	std::string tmp = parent_->mp()->log_dir() + "/" + "mmapFile.txt";
    int fd = open(tmp.c_str(),O_RDWR, S_IRUSR | S_IWUSR);

	if (fstat(fd,&sb) == -1)
	{
	  std::cout << "Cannot open file..." << std::endl;
	}  
    
    char *file_in_memory = static_cast<char*> (mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
	printf("file size is %ld\n",sb.st_size);
	
	
    
    count = 0;
	loop = true;
        while (loop && count < 50)
	{
		
	
		if (file_in_memory[0] == '0'){
			printf("Message Received\n");
			for (int i=1; i < sb.st_size; i++)
			{
				printf("%c", file_in_memory[i]);
			}
		        printf("\n");
			file_in_memory[0] = '1';
			for (uint i=0; i < (strlen(text)); i++)
			{
			  file_in_memory[i + 1] = text[i];
			}
			count = 9999;
		}
		if (file_in_memory[0] == '9') {
			for (int i=1; i < sb.st_size; i++)
			{
		            printf("%c", file_in_memory[i]);
			}
		   count = 9999;
		}
		else {
			printf("Waiting on Response\n");
            count += 1;
			usleep(2500);
		}
	}
    return true;
}
} // namespace sensor
} // namespace scrimmage
