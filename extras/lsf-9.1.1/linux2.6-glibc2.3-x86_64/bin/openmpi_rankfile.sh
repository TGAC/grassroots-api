#!/bin/sh
#****************************************************************************
#
# Name: openmpi_rankfile.sh
#
# $RCSfile: openmpi_rankfile.sh,v $Revision: 1.5 $Date: 2013/01/10 05:52:10 $
#
# (c) Copyright International Business Machines Corp 2013.
# US Government Users Restricted Rights - Use, duplication or disclosure 
# restricted by GSA ADP Schedule Contract with IBM Corp.
#
#****************************************************************************

if [ "$1" = "setup" ]; then

    if [ -f "$LSB_AFFINITY_HOSTFILE" ]; then

        AWK=""
        for tmp in nawk /usr/toolbox/nawk gawk awk  
        do
            if ($tmp 'BEGIN{ } { }') < /dev/null > /dev/null 2>&1  
            then    
                AWK=$tmp
                break
            fi   
        done

        if [ -n "$AWK" ]; then

            LSB_RANK_HOSTFILE=`echo "$LSB_AFFINITY_HOSTFILE" | $AWK '{sub(/[a-zA-Z]*$/,"hostRankFile"); print $0}'`

            $AWK '{printf("rank %d=%s slot=%s\n", NR-1, $1, $2)}' "$LSB_AFFINITY_HOSTFILE" > "$LSB_RANK_HOSTFILE"

            chmod 400 "$LSB_RANK_HOSTFILE"

            echo djob_setenv "LSB_RANK_HOSTFILE=$LSB_RANK_HOSTFILE"
        fi
    fi 
else
    if [ -f "$LSB_RANK_HOSTFILE" ]; then 
        rm -f "$LSB_RANK_HOSTFILE" 
    fi
fi
 
