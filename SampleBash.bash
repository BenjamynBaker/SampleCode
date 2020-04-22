#!/bin/bash

OSTYPE=`uname -s`
VERSION=$1
KEEP_RUNNING="t"
NEED_INFO="f"

#program variables
THREADS="0"
DEADLINE="0"
SIZE="0"

function getProgramInfo {
        read -p  "Enter the Program you would like to run [A1:A4, Exit]: " VERSION
       	read -p "Enter the number of threads you would like to create: " THREADS
        read -p "Enter the deadline for thread completion: " DEADLINE
        read -p "Enter the max size of square value: " SIZE
}

function readFile {
	while IFS= read -r line
        	do
			IFS=' ' read -r -a lineArray <<<$line
                        THREADS=${lineArray[0]}
                        DEADLINE="${lineArray[1]}"
                        SIZE="${lineArray[2]}"
			
			#echo $VERSION
                        #echo $THREADS
                        #echo $DEADLINE
                        #echo $SIZE
			callProgram
	done<"$FILENAME"

}

function callProgram {
	if [ "$VERSION" = "A1" -a "$OSTYPE" = "MINGW64_NT-10.0-17763" ]; then
                        echo "Running A1"
                        if [ "$NEED_INFO" = "t" ]; then
			getProgramInfo
			fi
                        ./partA1.exe $THREADS $DEADLINE $SIZE
                elif [ "$VERSION" = "A2" -a "$OSTYPE" = "Linux" ]; then
                        echo "Running A2"
                        if [ "$NEED_INFO" = "t" ]; then
                        getProgramInfo
                        fi
			./partA2 $THREADS $DEADLINE $SIZE
                elif [ "$VERSION" = "A3" ]; then
                        echo "This would run A3"
                        if [ "$NEED_INFO" = "t" ]; then
                        getProgramInfo
                        fi
			#add functionality to run A3
                        
                elif [ "$VERSION" = "A4" ]; then
                        echo "This would run A4"
			if [ "$NEED_INFO" = "t" ]; then
                        getProgramInfo
			fi
		#add functionality to run A4
                elif [ "$VERSION" = "Exit" ]; then
			exit 0;

                else
                        echo "Invalid input"
			exit 1;

                fi


}



#if no cmd line args are provided
if [ "$1" = "" ]; then
	while [ "$KEEP_RUNNING" = "t" ]; do	
	getProgramInfo	
	callProgram
	done
fi	
if [ -f $2  ]; then
	echo "$2"
	FILENAME="/dev/stdin"
	readFile 
else
#if no file present, verify args
	if [ "$2" = "" -o "$3" = "" -o "$4" = "" ]; then
	 	echo "Not enough arguements provided"
	else
		NEED_INFO="f"
		THREADS="$2"
		DEADLINE="$3"
		SIZE="$4"
		callProgram	
	fi
fi
