# SMALLBETS
This is a repository for holding all files associated with the integration of SCRIMMAGE, 3d plume simulations, and BELLHOP. 
this text is a test for making an initial commit, ignore this line



#### Directions to compile Acoustic Toolbox in Ubuntu 18.04 

### Download Acoustic Toolbox Source Code

The Acoustics Toolbox (AT) is distributed under the GNU Public License.  Follow directions below to download and extract AT, note: this is static link and if source code/address is updated you may need to grab latest link from ( http://oalib.hlsresearch.com/AcousticsToolbox/ ):

    $ mkdir ~/smallbets
    $ cd ~/smallbets
    $ wget http://oalib.hlsresearch.com/AcousticsToolbox/at_2020_11_4.zip
    $ unzip at_2020_11_4.zip

### Install Fortran Compiler

You can install the GNU Fortran compiler using::

    $ sudo apt-get install gfortran

### Compile Acoustic Toolbox for Ubuntu

Once compiler and code are setup, you can compile the toolbox.  And if desired move executables into proper bin directory.  Note: executables are create within the script to a *.exe extension (though not standard for linux, the files are compile to now run natively in Linux)

    $ cd ~/smallbets/at_2020_11_4
    $ make