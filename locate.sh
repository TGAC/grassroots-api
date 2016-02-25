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


echo ">>> MONGODB INC"
FILENAME=mongoc\.h
LookUp $FILENAME
MONGO_C_INC_DIR="$(dirname "$VAR_DIR" )"
echo "Using $VAR_DIR for $FILENAME"


echo ">>> MONGODB LIB"
FILENAME=libmongoc-1\.0\.so
LookUp $FILENAME
MONGO_C_LIB_DIR=$VAR_DIR
echo "Using $VAR_DIR for $FILENAME"


echo ">>> BSON INC"
FILENAME=bson\.h
LookUp $FILENAME
BSON_INC_DIR="$(dirname "$VAR_DIR" )"
echo "Using $VAR_DIR for $FILENAME"


echo ">>> BSON LIB"
FILENAME=libbson-1\.0\.so
LookUp $FILENAME
BSON_LIB_DIR=$VAR_DIR
echo "Using $VAR_DIR for $FILENAME"


echo ">>> DROPBOX INC"
FILENAME=dropbox\.h
LookUp $FILENAME
DROPBOX_INC_DIR="$(dirname "$VAR_DIR" )"
echo "Using $VAR_DIR for $FILENAME"


echo ">>> DROPBOX LIB"
FILENAME=libdropbox\.so
LookUp $FILENAME
DROPBOX_LIB_DIR=$VAR_DIR
echo "Using $VAR_DIR for $FILENAME"


echo ">>> SAMTOOLS INC"
FILENAME=hts\.h
LookUp $FILENAME
SAMTOOLS_INC_DIR="$(dirname "$VAR_DIR" )"
echo "Using $VAR_DIR for $FILENAME"


echo ">>> SAMTOOLS LIB"
FILENAME=libhts\.so
LookUp $FILENAME
SAMTOOLS_LIB_DIR=$VAR_DIR
echo "Using $VAR_DIR for $FILENAME"


echo ">>> HTMLCXX INC"
FILENAME=ParserDom\.h
LookUp $FILENAME
# need to go up 3 levels to get required include folder
VAR_DIR="$(dirname "$VAR_DIR" )"
VAR_DIR="$(dirname "$VAR_DIR" )"
HTMLCXX_INC_DIR="$(dirname "$VAR_DIR" )"
echo "Using $VAR_DIR for $FILENAME"


echo ">>> HTMLCXX LIB"
FILENAME=libhtmlcxx\.so
LookUp $FILENAME
HTMLCXX_LIB_DIR=$VAR_DIR
echo "Using $VAR_DIR for $FILENAME"


echo ">>> HCXSELECTC INC"
FILENAME=hcxselect\.h
LookUp $FILENAME
HCXSELECT_INC_DIR=$VAR_DIR
echo "Using $VAR_DIR for $FILENAME"


echo ">>> HCXSELECTC LIB"
FILENAME=libhcxselect\.so
LookUp $FILENAME
HCXSELECTCXX_LIB_DIR=$VAR_DIR
echo "Using $VAR_DIR for $FILENAME"


echo "CURL_LIB_DIR: $CURL_LIB_DIR"
echo "CURL_INC_DIR: $CURL_INC_DIR"
echo "IRODS_LIB_DIR: $IRODS_LIB_DIR"
echo "IRODS_INC_DIR: $IRODS_INC_DIR"
echo "JANSSON_LIB_DIR: $JANSSON_LIB_DIR"
echo "JANSSON_INC_DIR: $JANSSON_INC_DIR"
echo "UUID_LIB_DIR: $UUID_LIB_DIR"
echo "UUID_INC_DIR: $UUID_INC_DIR"
echo "MONGO_C_LIB_DIR: $MONGO_C_LIB_DIR"
echo "MONGO_C_INC_DIR: $MONGO_C_INC_DIR"
echo "BSON_LIB_DIR: $BSON_LIB_DIR"
echo "BSON_INC_DIR: $BSON_INC_DIR"
echo "DROPBOX_LIB_DIR: $DROPBOX_LIB_DIR"
echo "DROPBOX_INC_DIR: $DROPBOX_INC_DIR"
echo "SAMTOOLS_LIB_DIR: $SAMTOOLS_LIB_DIR"
echo "SAMTOOLS_INC_DIR: $SAMTOOLS_INC_DIR"
echo "HTMLCXX_LIB_DIR: $HTMLCXX_LIB_DIR"
echo "HTMLCXX_INC_DIR: $HTMLCXX_INC_DIR"
echo "HCXSELECTCXX_LIB_DIR: $HCXSELECTCXX_LIB_DIR"
echo "HCXSELECTCXX_INC_DIR: $HCXSELECTCXX_INC_DIR"

