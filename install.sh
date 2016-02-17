#!/bin/sh

GetPath () {
	path_to_file=$(locate -r $1 -l 1)

	if [ $path_to_file -eq "" ]; then
		parent_dir="123"
	else
		parent_dir=$(dirname "$path_to_file")
	fi

	echo "$parent_dir"
}



parent_dir=$(GetPath "^\(.*/\)*curl/curl\.h$")
echo "curl/curl.h: _($parent_dir)_"

parent_dir=$(GetPath "^\(.*/\)*libcurl\.so$")
echo "libcurl: _($parent_dir)_"

parent_dir=$(GetPath "^\(.*/\)*libcurly\.so$")

echo "libcurly: _($parent_dir)_"

exit 0
