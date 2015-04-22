#! /bin/sh

# $Id: check_exec_test.sh 430459 2014-03-26 13:54:08Z ivanov $
# Author:  Vladimir Ivanov, NCBI 
#
###########################################################################
#
# Time-guard script to run test program from other scripts (shell).
# 
#
# Usage:
#    check_exec_test.sh [-stdin] <cmd-line>
#
#    -stdin   - option used when application from <cmd-line> attempt
#               to read from std input (cgi applications for example).
#    cmd-line - command line to execute (shell scripts not allowed).
#
# Note:
#    The <cmd-line> should be presented with one application with
#    optional parameters only. Using scripts is not allowed here.
#
#    The CHECK_TIMEOUT environment variable defines a "timeout" seconds
#    to execute specified command line.By default timeout is 200 sec.
#
#    For protect infinity execution <cmd-line>, this script terminate 
#    check process if it still executing after "timeout" seconds.
#    Script return <cmd-line> exit code or value above 0 in case error
#    to parent shell.
#
###########################################################################


# Get parameters
timeout="${CHECK_TIMEOUT:-200}"
script_dir=`dirname $0`
script_dir=`(cd "$script_dir"; pwd)`

is_stdin=false
tmp=./$2.stdin.$$

# Reinforce timeout
ulimit -t `expr $timeout + 5` > /dev/null 2>&1

# Use different kill on Unix and Cygwin
case `uname -s` in
   CYGWIN* ) cygwin=true  ; kill='/bin/kill -f' ;;
   *)        cygwin=false ; kill='kill' ;;
esac

# Run command.
if [ "X$1" = "X-stdin" ]; then
   is_stdin=true
   trap 'rm -f $tmp' 1 2 15
   cat - > $tmp
   shift
fi

echo $1 | grep '\.sh' > /dev/null 2>&1 
if test $? -eq 0;  then
   echo "Error: Using CHECK_EXEC macro to run shell scripts is prohibited!"
   exit 1
fi

if $is_stdin; then
   $NCBI_CHECK_TOOL "$@" < $tmp &
else
   $NCBI_CHECK_TOOL "$@" &
fi
pid=$!
trap "$kill $pid; rm -f $tmp" 1 2 15

# Execute time-guard
$script_dir/check_exec_guard.sh $timeout $pid &

# Wait ending of execution
wait $pid > /dev/null 2>&1
status=$?
rm -f $tmp > /dev/null 2>&1

# Return test exit code
exit $status
