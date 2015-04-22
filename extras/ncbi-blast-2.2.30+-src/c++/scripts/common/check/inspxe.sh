#!/bin/bash

# =========================================================
# Wrapper to run test under Intel Inspector XE.
# Command line:
#      inspxe <test_name> <args>
# =========================================================

script="$0"
script_dir=`dirname $script`
script_dir=`(cd "$script_dir"; pwd)`
suppress_dir="$(cygpath -w $script_dir)\\inspxe-suppressions"


# Check Intel Inspector XE 2015 presence

if test -z "$INSPECTOR_XE_2015_DIR"; then
   echo "Cannot find Intel Inspector XE 2015."
   exit 1 
fi
inspxe=`echo $(cygpath -u $INSPECTOR_XE_2015_DIR) | sed 's| |\ |g'`/bin32/inspxe-cl


# Result directory name

rd="${1}.i"

n=1
while [ -d "$rd" ]; do
  n="`expr $n + 1`"
  rd="${1}.i_$n"
done


# Add .exe extension to the first argument, name of the program
exe=$1.exe
shift

# Run test
"$inspxe" -collect mi3 -knob detect-resource-leaks=false -result-dir $rd -return-app-exitcode -suppression-file "$suppress_dir" -- $exe "$@"
app_result=$?
"$inspxe" -report problems -report-all -result-dir $rd
insp_result=$?


# Exit with application's exit code if check has finished succesfuly

if test $insp_result -eq 0;  then
   exit $app_result
fi
exit $insp_result
