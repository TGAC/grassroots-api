#!/bin/bash

LookUp ()
{
	NUM_MATCHES=`locate -c -r "^\(.*/\)*\$1$"`
	FILENAMES=`locate -r "^\(.*/\)*\$1$"`
	VAR_DIR=""
	
	echo "NUM_MATCHES $NUM_MATCHES"
	echo "= ${#FILENAMES[@]}"




	if [ $NUM_MATCHES -eq 0 ]; then
		echo "No match for $1"
	else 
		if [ $NUM_MATCHES -eq 1 ]; then
			echo "matched ${FILENAMES}"
			VAR_DIR="$(dirname "$FILENAMES")"
		else
			j=1

			for i in $FILENAMES; do
				echo "option [$j]: $i"
				j=$(( j + 1 ))
			done

			echo "Which option do you want to use between 1 and $((j-1))?"
			read ANSWER

			re='^[0-9]+$'
			if ! [[ $ANSWER =~ $re ]] ; then
				 echo "error: Not a number";
			else
				if [ $ANSWER -lt $j ]; then
					if [ $ANSWER -gt 0 ]; then 
						echo "You chose $ANSWER"

						j=1 
						for i in $FILENAMES; do

							if [[ $j -eq $ANSWER ]] ; then
								VAR_DIR="$(dirname "$i" )"
							fi 
							j=$(( j + 1 ))
						done

					else
						echo "$ANSWER must be greater than 0"
					fi
				else
						echo "$ANSWER must be less than $j"
				fi
			fi
		fi
	fi
}

echo ">>> CURL"
FILENAME=libcurl\.so
LookUp $FILENAME
echo "Using $VAR_DIR for $FILENAME"

echo ">>> IRODS INC"
FILENAME=irods_version\.h
LookUp $FILENAME
echo "Using $VAR_DIR for $FILENAME"


#echo "irods include"
#FILENAME=`locate -r "^\(.*/\)*irods_version\.h$"`
#echo "FILE: $FILENAME"
#DIR="$(dirname "$FILENAME")"
#echo "DIR: $DIR"



#echo "irods lib"
#FILENAME=`locate -r "^\(.*/\)*libirods_client\.so$"`
#echo "FILE: $FILENAME"


