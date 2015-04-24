#!/bin/sh
# $RCSfile: lsrun.sh,v $Revision: 1.4 $Date: 2013/01/10 05:55:43 $

#
# lsrun.sh
#
# This script converts call to the `rsh` into the call to `lsgrun`.
# rsh syntax:
#     rsh [-Kdnx] [-l username] host [command]
#
# For all user command, add /bin/sh -c, any environmental variables
# can be passed to remote execution host.
#

# Sanity check for arguments
if  [ $# -eq 0 ] ; then
    echo "Usage: `basename $0` [-Kdnx] [-l username] host [command]"
    exit 1
fi

lsgrun_opts=""
disable_stdin="0"
lsgrun_cmd=""

if [ -z "$LSF_BINDIR" ]; then
    lsgrun_cmd="$LSF_BINDIR/lsgrun "
else
    lsgrun_cmd="lsgrun "
fi

#parse $*:
hostname=""
job_cmdln=""

while [ "$#" != "0" ]; do
    case $1 in
        "-l")
            # Ignore -l username
            shift
            shift
            ;;
        "-"*)
            # -n is an option of echo,
            # add x or use echo -- -n
            optionN=`echo x"$1" | grep n 2>/dev/null`
            if [ -n "$optionN" ]; then
                disable_stdin=1
            fi
            shift
            ;;
        *)
            if [ "$hostname" = "" ]; then
                hostname="$1"
                lsgrun_opts="-m $hostname "
                shift
            else
                break
            fi
            ;;
    esac
done

job_cmdln="$*"

if [ "$disable_stdin" = "0" ]; then
    $lsgrun_cmd $lsgrun_opts /bin/sh -c "$job_cmdln"
else
    $lsgrun_cmd $lsgrun_opts /bin/sh -c "$job_cmdln" </dev/null
fi

#
# Exit with the lsgrun command exit status
# do not print message here as rsh will leave the
# any message from the command to the end user to
# handle
#
exit $?
