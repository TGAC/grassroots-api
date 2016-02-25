#!/bin/bash

LookUp ()
{
	NUM_MATCHES=`locate -c -r "^\(.*/\)*\$1$"`
	FILENAMES=`locate -r "^\(.*/\)*\$1$"`
	VAR_DIR=""
	
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


echo ">>> CURL INC"
FILENAME=curl\.h
LookUp $FILENAME
CURL_INC_DIR=$VAR_DIR
echo "Using $VAR_DIR for $FILENAME"


echo ">>> CURL LIB"
FILENAME=libcurl\.so
LookUp $FILENAME
CURL_LIB_DIR=$VAR_DIR
echo "Using $VAR_DIR for $FILENAME"


echo ">>> IRODS INC"
FILENAME=irods_version\.h
LookUp $FILENAME
IRODS_INC_DIR=$VAR_DIR
echo "Using $VAR_DIR for $FILENAME"


echo ">>> IRODS LIB"IR
FILENAME=libirods_client\.so
LookUp $FILENAME
IRODS_LIB_DIR=$VAR_DIR
echo "Using $VAR_DIR for $FILENAME"


echo ">>> JANSSON INC"
FILENAME=jansson\.h
LookUp $FILENAME
JANSSON_INC_DIR=$VAR_DIR
echo "Using $VAR_DIR for $FILENAME"


echo ">>> JANSSON LIB"
FILENAME=libjansson\.so
LookUp $FILENAME
JANSSON_LIB_DIR=$VAR_DIR
echo "Using $VAR_DIR for $FILENAME"


echo ">>> UUID INC"
FILENAME=uuid\/uuid\.h
LookUp $FILENAME
UUID_INC_DIR="$(dirname "$VAR_DIR" )"
echo "Using $VAR_DIR for $FILENAME"


echo ">>> UUID LIB"
FILENAME=libuuid\.so
LookUp $FILENAME
UUID_LIB_DIR=$VAR_DIR
echo "Using $VAR_DIR for $FILENAME"

echo "CURL_LIB_DIR: $CURL_LIB_DIR"
echo "CURL_INC_DIR: $CURL_INC_DIR"
echo "IRODS_LIB_DIR: $IRODS_LIB_DIR"
echo "IRODS_INC_DIR: $IRODS_INC_DIR"
echo "JANSSON_LIB_DIR: $JANSSON_LIB_DIR"
echo "JANSSON_INC_DIR: $JANSSON_INC_DIR"
echo "UUID_LIB_DIR: $UUID_LIB_DIR"
echo "UUID_INC_DIR: $UUID_INC_DIR"

