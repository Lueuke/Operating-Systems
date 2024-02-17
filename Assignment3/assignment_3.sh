#!/bin/bash
#SBATCH --job-name=CS4352_A3
#SBATCH --output=%x.o%j
#SBATCH --error=%x.e%j
#SBATCH --partition=quanah
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=36
#SBATCH --time=00:10:00
#SBATCH --mem-per-cpu=5370MB  #5.3GB per core


#==============================================================================
#title        : assignment_3.sh
#description  : This script will perform the following actions:
#		  1) Purge all modules then load GNU 5.4.0 and Singularity
#		  2) Run make on your C / C++ code.
#		  3) Boot up the API server (Ubuntu container running FLASK)
#		  4) Execute your C/C++ Application to interface with the APIs
#author       : errees (R#123456)
#date         : 03/05/2023
#version      : 1.0
#usage        : sbatch assignment_3.sh
#notes        : This script requires GNU 5.4.0 & access to Singularity.
#bash_version : 4.2.46(2)-release
#==============================================================================


#Clean the Environment
module purge
module load gnu/5.4.0 singularity


#Delete the executable then re-compile the source code.
echo -e "\n\nCS4352 API Test Script.\n"
echo -e "Running the following commands:"
echo -e "   make clean &> make.log\n   rm -f assignment_3 server.log\n   make &>> make.log"
make clean &> make.log
rm -f assignment_3 server.log
make &>> make.log

if test -f "assignment_3"; then
        #The make command finished successfully.
        echo -e "\n\nMake successful! Booting up the Ubuntu Server."

	#Boot the Singularity container running FLASK.
	singularity run /lustre/work/errees/courses/cs4352/assignment3/images/python_api.sif &> server.log &

	#Sleep for 10 seconds to ensure the Ubuntu container is alive.
	sleep 10

        echo -e "Executing assignment_3.\n\n"
	#Execute assignment_3 application.
	./assignment_3 &> assignment_3.out
else
        #The make command did not generate a binary file - likely due to incorrect makefile or errors in code.
        echo -e "\n\nThe compilation process appears to have failed! Printing out make.log:\n"
        cat "make.log"
        echo -e "\n"

fi

