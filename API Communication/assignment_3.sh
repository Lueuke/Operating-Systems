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

chmod +x assignment_3 

#Delete the executable then re-compile the source code.
echo -e "\n\nCS4352 API Test Script.\n"
echo -e "Running the following commands:"
echo -e "   make clean &> make.log\n   rm -f assignment_3 server.log\n   make &>> make.log"
make clean &> make.log
rm -f assignment_3 server.log assignment_3.out
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


	# Define the integers to verify, in order
	integersToVerify=(6089 24356 5085 30962565)

	# Initialize index to track the current integer to verify
	currentIndex=0

	while read -r line; do
		# Check if the current line contains the current integer to verify
		if [[ "$line" =~ ${integersToVerify[$currentIndex]} ]]; then
			# Move to the next integer
			((currentIndex++))
		fi
		if [ $currentIndex -eq ${#integersToVerify[@]} ]; then
			echo "All integers verified in order."
			break
		fi
	done < assignment_3.out

	# Calculate how many integers were NOT found
	integersNotFound=$((${#integersToVerify[@]} - currentIndex))


	#Detect R#
	rNum=$(grep -o 'R[0-9]\+' assignment_3.out)

	if [ -z "$rNum" ]; then
		echo "R# Detected: None" > assignment_3_grade.out
	else
		echo "R# Detected: $rNum"> assignment_3_grade.out
	fi

	if [ "$integersNotFound" -eq 0 ]; then
		echo "Correctness Score: 100%" >> assignment_3_grade.out
	elif [ "$integersNotFound" -eq 1 ]; then
		echo "Correctness Score: 80%" >> assignment_3_grade.out
	elif [ "$integersNotFound" -eq 2 ]; then
		echo "Correctness Score: 50%" >> assignment_3_grade.out
	elif [ "$integersNotFound" -eq 3 ] || [ "$integersNotFound" -eq 4 ]; then
		echo "Correctness Score: 0%" >> assignment_3_grade.out
	fi


else
        #The make command did not generate a binary file - likely due to incorrect makefile or errors in code.
        echo -e "\n\nThe compilation process appears to have failed! Printing out make.log:\n"
        cat "make.log"
        echo -e "\n"

	echo "R# Detected: None" > assignment_3_grade.out
	echo "Correctness Score: 0%" >> assignment_3_grade.out

fi

