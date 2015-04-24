#!/bin/sh
#--------------------------------------------------------------------------
# This code gets the binary type when setting LSF user environment.
# It is based on found_binary_type() in scripts/funcs.sh.
#
# (c) Copyright International Business Machines Corp 1992-2012.
# US Government Users Restricted Rights - Use, duplication or disclosure 
# restricted by GSA ADP Schedule Contract with IBM Corp.
#---------------------------------------------------------------------------

#------------------------------------------------------------------
# Name: strstr
# Synopsis: strstr $SRC_STR $SUB_STR
# Description:
#       check whether $SUB_STR is in $SRC_STR or not
#       grep -w does not work on SGI or HP.
# Return Value:
#       0 yes 1 no
#------------------------------------------------------------------
strstr ()
{
_src_string="$1"
_sub_string="$2"

if [ "$_src_string" = "" -o "$_sub_string" = "" ]; then
    return 1
fi

for _item in $_src_string
do
    if [ "$_item" = "$_sub_string" ]; then
        return 0
    fi
done

return 1
} # strstr

#-----------------------------------
# Name: is_rms_host
# Synopsis: is_rms_host
# Environment Variables:
# Description:
#       It checks whether or not the current host is RMS host
# Return Value:
#       0 RMS host; 1 non-RMS host
#-----------------------------------
is_rms_host ()
{
    RMSQUERY="rmsquery"
    which "$RMSQUERY" >$LSNULFILE 2>&1
    if [ "$?" = "0" ]; then
        HOST_NAME=`hostname | cut -d. -f 1`
        RMS_HOST_LIST=`$RMSQUERY 'select name from nodes' 2>$LSNULFILE`
        if strstr "$RMS_HOST_LIST" "$HOST_NAME" ; then
            field1=`echo $uname_val |$AWK '{print $1}'`
            if [ "$field1" = "Linux" ]; then
                if [ -z "$RMS_VERSION" ]; then
                    RMS_VERSION=`rpm -q qsrmslibs | sed -e "s/qsrmslibs\-//g" | sed -e "s/\-.*//g"` 
                    #
                    # This is safe, in /usr/bin/rmscheckenv, it greps "not installed" as well
                    # if qsrmslibs is not installed, override the binary type set before.
                    # It's a non-rms host
                    #
                    NOT_INSTALLED=`echo $RMS_VERSION | grep "not installed" 2>$LSNULFILE`
                    if [ -n "$NOT_INSTALLED" ]; then
                        RMS_VERSION=""
                        return 1
                    fi
                fi
                if [ "$RMS_VERSION" != "" -a "$RMS_VERSION" != "2.80" -a "$RMS_VERSION" != "2.81" ]; then
                    RMS_VERSION="2.82"
                fi
                export RMS_VERSION
            fi
            return 0
        fi
    fi
    return 1
} #is_rms_host

#-----------------------------------
# Name: is_bproc_host
# Synopsis: is_bproc_host
# Environment Variables:
# Description:
#       It checks whether or not the current host is bproc host
# Return Value:
#       0 bproc host; 1 non-bproc host
#-----------------------------------
is_bproc_host ()
{   
    if [ -f /usr/lib/libbproc.so ]; then
        return 0
    else
        return 1
    fi
} # is_bproc_host 

#-----------------------------------
# Name: is_slurm_host
# Synopsis: is_slurm_host
# Environment Variables:
# Description:
#       It checks whether or not the current host is a slurm host
#       check HP-hptc/normal linux with slurm installed.
#       Currently we only check for HP-hptc: /var/lsf/lsfslurm
#       in the output
# Return Value:
#       0 slurm host; 1 non-slurm host
#-----------------------------------
is_slurm_host ()
{
    _slurm_signature_file="/var/lsf/lsfslurm"

    if [ -f  "$_slurm_signature_file" ]; then
        # HP hptc release
        return 0
    fi
    return 1
} # is_slurm_host

#-----------------------------------
# Name: is_cpuset_host
# Synopsis: is_cpuset_host
# Environment Variables:
# Description:
#       It checkes whether or not the current host is a cpuset host
#       Check if libcpuset.so exist or not
#       /usr/lib/libcpuset.so 
#       /lib32/libcpuset.so 
#       /lib64/libcpuset.so 
# Return Value:
#       0 cpuset host; 1 non-cpuset host
#-----------------------------------
is_cpuset_host ()
{
    if [ -f  "/usr/lib/libcpuset.so" -o -f "/usr/lib64/libcpuset.so" -o -f "/lib32/libcpuset.so" -o -f "/lib64/libcpuset.so" ]; then
	# cpuset host
        return 0
    fi
    # Non-cpuset hosts
    return 1
} # is_cpuset_host

#-----------------------------------
# Name: is_bluegene_host
# Synopsis: is_bluegene_host
# Environment Variables:
# Description:
#       It checkes whether or not the current host is a bluegene service node
#       Check if /usr/lib64/libdb2.so can be found
# Return Value:
#       0 bluegene SN; 1 non-bluegene SN
#-----------------------------------
is_bluegene_host ()
{
    if [ -f "/usr/lib64/libdb2.so" ]; then
        # bluegene SN
        return 0;
    fi;
    # Non-bluegene SN
    return 1
} # is_bluegene_host

#-----------------------------------
# Name: is_craylinux_host
# Synopsis: is_craylinux_host
# Environment Variables:
# Description:
#       It checkes whether or not the current host is a crayxt/xe login/service node 
# Return Value:
#       0 crayxt host; 1 non-crayxt host
#-----------------------------------
is_craylinux_host ()
{
    if [ -f "/opt/cray/rca/default/bin/rca-helper" -a -f "/etc/opt/cray/sdb/node_classes" ]; then
        nid=`/opt/cray/rca/default/bin/rca-helper -i`
        role=`grep "^$nid:" /etc/opt/cray/sdb/node_classes | cut -f2 -d':' 2>/dev/null`
    elif [ -f /etc/node_classes ]; then
        nid=`xtuname`
        nid=`expr ${nid}`
        role=`grep "^$nid:" /etc/node_classes | cut -f2 -d':' 2>/dev/null`
    else
        role=unknown
    fi

    if [ "$role" = "login" -o "$role" = "service"  ]; then
        return 0; # this is a login/service node on cray xt/xe system
    fi
  
    return 1
}

_CUR_PATH_ENV="$PATH"
PATH=/usr/bin:/bin:/usr/local/bin:/local/bin:/sbin:/usr/sbin:/usr/ucb:/usr/sbin:/usr/bsd:${PATH}

# handle difference between system V and BSD echo
# To echo "foo" with no newline, do
# echo $enf "foo" $enl

if [ "`echo -n`" = "-n" ] ; then
    enf=
    enl="\c"
else
    enf=-n
    enl=
fi
export enf enl

 LSNULFILE=/dev/null

# Find a version of awk we can trust
AWK=""
for tmp in nawk  /usr/toolbox/nawk gawk awk 
do
#   This is the real test, for functions & tolower
#    if foo=`(echo FOO | $tmp 'function tl(str) { return tolower(str) } { print tl($1) }') 2>$LSNULFILE` \
#       && test "$foo" = "foo"
#   A simpler test, just for executability
    if ($tmp 'BEGIN{ } { }') < $LSNULFILE > $LSNULFILE 2>&1  
    then
	AWK=$tmp
        break
    fi
done

if test "$AWK" = ""
then
    echo "Cannot find a correct version of awk."
    echo "Exiting ... "
    exit 1
fi
export AWK
 
    uname_val=`uname -a`
    BINARY_TYPE="fail"
    BINARY_TYPE_HPC="" # if the binary type is different between standard LSF and HPC
    export BINARY_TYPE BINARY_TYPE_HPC
    field1=`echo $uname_val |$AWK '{print $1}'`
    case "$field1" in 
    UNIX_System_V)
	 BINARY_TYPE="uxp"
	 ;;

    OSF1)
         version=`echo $uname_val | $AWK '{split($3, a, "."); print a[1]}'`
         if [ $version = "V5" ] ; then
             BINARY_TYPE="alpha5"
         else
            if [ $version = "T5" ] ; then
               BINARY_TYPE="alpha5"
             else
                BINARY_TYPE="alpha"
             fi
         fi

         if [ "$BINARY_TYPE" = "alpha5" ]; then
            # check for rms
	    # Kite#31479
	    if [ ! -z "`/usr/sbin/rcmgr get SC_MS`" ]; then
		BINARY_TYPE="alpha5-rms"
	    fi
         fi
	     ;;
    HI-UX) 
	   BINARY_TYPE="hppa"
 	   ;;
   HP-UX) 
        # spp need to run uname -C to find the version
        uname -C 2>$LSNULFILE
        if [ $? = 1 ]; then 
            version=`echo $uname_val | $AWK '{split($3, a, "."); print a[2]}'`
	    if [ $version = "09" ] ; then
	        BINARY_TYPE="hppa9"
            elif [ $version = "10" ] ; then
                subver=`echo $uname_val | $AWK '{split($3, a, "."); print a[3]}'`
                if [ $subver = "10" ]; then
                    BINARY_TYPE="hppa10"
                elif [ $subver = "20" ]; then
                    BINARY_TYPE="hppa10.20"
                fi
            elif [ $version = "11" ] ; then
                machine=`echo $uname_val | $AWK '{print $5}'`
                subver=`echo $uname_val | $AWK '{split($3, a, "."); print a[3]}'`
                if [ "$machine" = "ia64" ]; then
                    BINARY_TYPE="hppa11-64"
                    #
                    # The order in BINARY_TYPE_HPC matters
                    # the order determines how profile.lsf/cshrc.lsf
                    # sets the LSF_BINDIR. The correct order should be
                    # from high version of OS to low version of OS.
                    #
                    BINARY_TYPE_HPC="hpuxia64 hppa11i-64 hppa11-64"
                else
                    kernel=`/usr/bin/getconf KERNEL_BITS`
                    if [ "$subver" = "00" ]; then
                        if [ "$kernel" = "32" ] ; then
                            BINARY_TYPE="hppa11-32"
                        else 
                            BINARY_TYPE="hppa11-64"
                        fi
                   else
                        if [ "$kernel" = "32" ] ; then
                            BINARY_TYPE="hppa11-32"
                            BINARY_TYPE_HPC="hppa11i-32 hppa11-32"
                            # reset to standard LSF binary type 
                            for ARCH in hppa11i-32 hppa11-32
                            do
                                if [ -f $LSF_TOP/$LSF_VERSION/$ARCH/etc/mbatchd ]; then 
                                    BINARY_TYPE="$ARCH" 
                                    break;
                                fi
                            done
                        else 
                            BINARY_TYPE="hppa11-64"
                            BINARY_TYPE_HPC="hppa11i-64 hppa11-64"
                            # reset to standard LSF binary type 
                            for ARCH in hppa11i-64 hppa11-64
			    do
                                if [ -f $LSF_TOP/$LSF_VERSION/$ARCH/etc/mbatchd ]; then 
                                    BINARY_TYPE="$ARCH" 
                                    break;
                                fi
                            done
                        fi
                
                    fi
                     
                fi
            else 
                echo "Cannot figure out the HP version."
		exit 1
            fi 
         else
	    version=`uname -C -a |  $AWK '{split($3, a, "."); print a[1]}'` 
	    if [ $version = "5" ]; then
	        BINARY_TYPE="spp"
            elif  [ $version = "4" ]; then
	        BINARY_TYPE="spp4"
             fi
         fi
        ;;
    Linux)
        version=`echo $uname_val | $AWK '{split($3, a, "."); print a[1]}'`
        _machine=`uname -m`
        #get glibc version
        for glibc in `ldconfig -p | grep libc.so.6 | sed 's/.*=>//'`
        do
            if [ -x "$glibc" ] ; then
                $glibc >/dev/null 2>&1
                if [ "$?" != "0" ] ; then
                    continue
                fi
                _libcver=`$glibc 2>/dev/null | grep "GNU C Library" | $AWK '{print substr($0,match($0,/version/))}' | $AWK '{print $2}' | $AWK -F. '{print $2}'| sed 's/,//'`
            fi
            if [ "$_libcver" != "" ] ; then
                break
            fi
        done
        if [ "$_libcver" = "" ] ; then
            echo "Cannot figure out the GLibc version."
            exit 1
        fi

        if [ $version = "1" ]; then
	    BINARY_TYPE="linux"
        elif [  $version = "2" -o $version -gt 2 ]; then
            subver=`echo $uname_val | $AWK '{split($3, a, "."); print a[1]"."a[2]}'`

            if [ $subver = "2.0" ]; then
	        BINARY_TYPE="linux2"
                if [ "$_machine" = "alpha" ]; then
                    BINARY_TYPE="linux2-alpha"
                else
                    BINARY_TYPE="linux2-intel"
                fi
            elif [  $subver = "2.2" ]; then
                BINARY_TYPE="linux2.2"

                if [ "$_machine" = "alpha" ]; then
                    BINARY_TYPE="linux2.2-glibc2.1-alpha"
                elif [ "$_machine" = "sparc" -o "$_machine" =  "sparc64" ]; then
		    BINARY_TYPE="linux2.2-glibc2.1-sparc"
                elif [ "$_machine" = "ppc" ]; then
		    BINARY_TYPE="linux2.2-glibc2.1-powerpc"
		else
                    BINARY_TYPE="linux2.2-glibc2.1-x86"
                fi

            elif [  $subver = "2.4" ]; then
                  BINARY_TYPE="linux2.4"

# ia64 is the family of 64-bit CPUs from Intel. We shouldn't need a new
# distribution for each processor

                  if [ "$_machine" = "ia64" ]; then
		      if [ "$_libcver" = "1" ]; then
		          BINARY_TYPE="linux2.4-glibc2.1-ia64"
	              elif [ "$_libcver" = "2" ]; then
                          # check if Dist system
                          if [ -e "/etc/cassatt-release" ]; then
                              BINARY_TYPE="linux2.4-glibc2.2-ia64-dist"
                          else
                              BINARY_TYPE="linux2.4-glibc2.2-ia64"
                          fi
                          is_rms_host
                          if [ "$?" = "0" ]; then
                              BINARY_TYPE_HPC="rms${RMS_VERSION}_linux2.4-glibc2.2-ia64 linux2.4-glibc2.2-ia64"
                          fi
			  is_cpuset_host
                          if [ "$?" = "0" ]; then
			      BINARY_TYPE_HPC="linux2.4-glibc2.2-sn-ipf"
		          fi
                      else
                              if [ "$_libcver" = "1" ]; then
                                  BINARY_TYPE="linux2.4-glibc2.1-ia64"
                              elif [ "$_libcver" = "2" ]; then
                                  BINARY_TYPE="linux2.4-glibc2.2-ia64"
                              else
                                  BINARY_TYPE="linux2.4-glibc2.3-ia64"
                              fi
                          is_rms_host
                          if [ "$?" = "0" ]; then
                              BINARY_TYPE_HPC="rms${RMS_VERSION}_linux2.4-glibc2.2-ia64 linux2.4-glibc2.3-ia64"
                          fi
			  is_cpuset_host
                          if [ "$?" = "0" ]; then
			      BINARY_TYPE_HPC="linux2.4-glibc2.3-sn-ipf"
		          fi
		      fi
                  elif [ "$_machine" = "ppc64" ]; then
                     BINARY_TYPE="linux2.4-glibc2.2-ppc64"
                  elif [ "$_machine" = "s390" ]; then
                     BINARY_TYPE="linux2.4-glibc2.2-s390-32"
                  elif [ "$_machine" = "s390x" ]; then
                     BINARY_TYPE="linux2.4-glibc2.2-s390x-64"
		  elif [ "$_machine" = "armv5l" ]; then
		     BINARY_TYPE="linux2.4-glibc2.2-armv5l"
                  elif [ "$_machine" = "x86_64" ]; then
                       _cputype=`cat /proc/cpuinfo | grep -i vendor | $AWK '{print $3}' | uniq`

		       if [ "$_libcver" = "1" ]; then
			   BINARY_TYPE="linux2.4-glibc2.1-x86_64"
			
		       elif [ "$_libcver" = "2" -o "$_libcver" = "3" ]; then
                           is_craylinux_host
			   if [ "$?" = "0"  ]; then
			       BINARY_TYPE="linux2.4-glibc2.3-x86_64-cray"
			   else
			       BINARY_TYPE="linux2.4-glibc2.2-x86_64"
			   fi
		       fi

                       is_slurm_host
                       if [ "$?" = "0" ]; then
                          BINARY_TYPE="linux2.4-glibc2.3-x86_64-slurm"
                       fi
                  elif [ "$_machine" = "alpha" ]; then
                     BINARY_TYPE="linux2.4-glibc2.2-alpha"
                     # check for rms
                     is_rms_host
                     if [ "$?" = "0" ]; then
                        BINARY_TYPE="rms${RMS_VERSION}_linux2.4-glibc2.2-alpha"
                     fi
                  else
		     echo $uname_val | $AWK '{print $3}' | grep "_Scyld" > $LSNULFILE 2>&1
		     if [ "$?" = "0" ]; then
                         BINARY_TYPE="linux2.4-glibc2.2-x86-scyld"
		     else
                       BINARY_TYPE="fail"
                       if [ "$_libcver" = "1" ]; then
                          BINARY_TYPE="linux2.4-glibc2.1-x86"
                       elif [ "$_libcver" = "2" ]; then
                          # check if Dist system
                          if [ -e "/etc/cassatt-release" ]; then
                              BINARY_TYPE="linux2.4-glibc2.2-x86-dist"
                          else
                              BINARY_TYPE="linux2.4-glibc2.2-x86"
			      if [ -d "/proc/vmware" ]; then
				  BINARY_TYPE="linux2.4-glibc2.2-x86-esx2.5"
			      fi
                          fi
                       elif [ "$_libcver" = "3" ]; then
                          BINARY_TYPE="linux2.4-glibc2.3-x86"
                       fi
                       # check for rms
                       is_rms_host
                       if [ "$?" = "0" ]; then
                           BINARY_TYPE="rms${RMS_VERSION}_${BINARY_TYPE}"
                       fi
                       is_bproc_host
                       if [ "$?" = "0" ]; then
                           BINARY_TYPE="${BINARY_TYPE}-bproc"
                       fi
                       is_slurm_host
                       if [ "$?" = "0" ]; then
                           BINARY_TYPE="${BINARY_TYPE}-slurm"
		       fi
		     fi 
                  fi
            elif [  $subver = "2.6" -o $version -gt 2 ]; then
               BINARY_TYPE="fail"
               case "$_machine" in
                   ppc64)
                     if [ "$_libcver" = "3" ]; then
                          BINARY_TYPE="linux2.6-glibc2.3-ppc64"
                          BINARY_TYPE_HPC="linux2.6-glibc2.3-ppc64-bluegene linux2.6-glibc2.3-ppc64"
                     elif [ "$_libcver" -ge "4" ]; then
                          BINARY_TYPE="linux2.6-glibc2.3-ppc64"
                     fi
                     ;;
                   ia64)
                      if [ "$_libcver" = "1" ]; then
                          BINARY_TYPE="linux2.6-glibc2.1-ia64"
                      elif [ "$_libcver" = "2" ]; then
                          BINARY_TYPE="linux2.6-glibc2.2-ia64"
                      elif [ "$_libcver" = "3" ]; then
                          BINARY_TYPE="linux2.6-glibc2.3-ia64"

			  is_rms_host
                          if [ "$?" = "0" ]; then
                              BINARY_TYPE_HPC="rms${RMS_VERSION}_${BINARY_TYPE}"
                          fi

			  is_cpuset_host
                          if [ "$?" = "0" ]; then
			      BINARY_TYPE_HPC="linux2.6-glibc2.3-sn-ipf"
		          fi

                      elif [ "$_libcver" = "4" ]; then
                          BINARY_TYPE="linux2.6-glibc2.3-ia64"
			  is_cpuset_host
			  if [ "$?" = "0" ]; then
			      BINARY_TYPE_HPC="linux2.6-glibc2.4-sn-ipf"
                          fi
                        elif [ "$_libcver" -ge 5 ]; then
                            BINARY_TYPE="linux2.6-glibc2.3-ia64"
                            is_cpuset_host
            			    if [ "$?" = "0" ]; then
    			                BINARY_TYPE_HPC="linux2.6-glibc2.5-sn-ipf"
                            fi
   
                            is_slurm_host
                            if [ "$?" = "0" ]; then
                                BINARY_TYPE="${BINARY_TYPE}-slurm"
                            fi
                      fi
                      
                      if [ ${_libcver} -ge 3 ]; then
                          is_slurm_host
                          if [ "$?" = "0" ]; then
                              BINARY_TYPE="${BINARY_TYPE}-slurm"
                          fi
                      fi
                      ;;
                   x86_64)
                       if [ "$_libcver" = "1" ]; then
                          BINARY_TYPE="linux2.6-glibc2.1-x86_64"
                       elif [ "$_libcver" = "2" ]; then
                          BINARY_TYPE="linux2.6-glibc2.2-x86_64"
                       elif [ "$_libcver" = "3" ]; then
                          BINARY_TYPE="linux2.6-glibc2.3-x86_64"
                       elif [ "$_libcver" -ge "4" ] ; then
                          BINARY_TYPE="linux2.6-glibc2.3-x86_64"
		       fi
		       #Check Cray XT3
		       #
                       is_craylinux_host
		       if [ "$?" = "0" ]; then
		           BINARY_TYPE="${BINARY_TYPE}-cray"
		       fi
		       #check BProc/BProc4
		       #
		       is_bproc_host
		       if [ "$?" = "0" ]; then
		           BINARY_TYPE="${BINARY_TYPE}-bproc"
			   if [ -f /usr/lib/libbproc.so.4 ]; then
			       BINARY_TYPE="${BINARY_TYPE}4"
                           fi
		       fi 
                       is_slurm_host
                       if [ "$?" = "0" ]; then
                           BINARY_TYPE="${BINARY_TYPE}-slurm"
                       fi
		       # check rms
		       is_rms_host
		       if [ "$?" = "0" ]; then
		           BINARY_TYPE_HPC="rms${RMS_VERSION}_${BINARY_TYPE} ${BINARY_TYPE}"
                       fi
                       ;;
                   i[3456]86)
                       if [ "$_libcver" = "1" ]; then
                           BINARY_TYPE="linux2.6-glibc2.1-x86"
                       elif [ "$_libcver" = "2" ]; then
                           BINARY_TYPE="linux2.6-glibc2.2-x86"
                       elif [ "$_libcver" = "3" ]; then
                           BINARY_TYPE="linux2.6-glibc2.3-x86"
                           # check for rms
                           is_rms_host
                           if [ "$?" = "0" ]; then
                               BINARY_TYPE_HPC="rms${RMS_VERSION}_${BINARY_TYPE} ${BINARY_TYPE}"
                           fi
                       elif [ "$_libcver" = "4" -o "$_libcver" = "5" -o "$_libcver" = "6" ]; then
                           BINARY_TYPE="linux2.6-glibc2.3-x86"
                       elif [ "$_libcver" -ge "7" ] ; then
                           BINARY_TYPE="linux2.6-glibc2.3-x86"
                       fi
                       ;;
               esac
            fi  #### end of if subver = 2.6 ####
        fi
         ;;
    UNIX_SV)
        BINARY_TYPE="mips-nec"
         ;;
    Darwin)
        BINARY_TYPE="macosx"
         ;;
    NEWS-OS)
	BINARY_TYPE="mips-sony"
 	;;
    AIX)
        version=`echo $uname_val | $AWK '{print $4}'`
        release=`echo $uname_val | $AWK '{print $3}'`
        if [ $version = "4" -a $release -ge 1 ]; then
           if [ $release -lt 2 ]; then
                BINARY_TYPE="aix4"
           elif [ $release -ge 2 ]; then
                 BINARY_TYPE="aix4.2"
           fi
        elif [ $version = "5" -a $release -ge 1 ]; then
            #On AIX, only in 64bit mode, you can see shlap64 process
            #check shlap64 process is safer than check kernel_bit
            if [ `ps -e -o "comm" |grep shlap64` ]; then
               BINARY_TYPE="aix-64"
            else
	        BINARY_TYPE="aix-32"
            fi
	elif [ $version = "3" ]; then
            BINARY_TYPE="aix3"
	elif [ $version = "6" ]; then
            if [ `ps -e -o "comm" |grep shlap64` ]; then
               BINARY_TYPE="aix-64"
            else
	        BINARY_TYPE="aix-32"
            fi
  	elif [ $version = "7" ]; then
            if [ `ps -e -o "comm" |grep shlap64` ]; then
               BINARY_TYPE="aix-64"
            else
	        BINARY_TYPE="aix-32"
            fi
        fi
	;;
    IRIX*)
        version=`echo $uname_val | $AWK '{split($3, a, "."); print a[1]}'`
        release=`echo $uname_val | $AWK '{split($3, a, "."); print a[2]}'`
        if [ $version = "6" -a $release -ge "2" ]; then
	    mls=`sysconf MAC`
	    if [ "$mls" = "1" ]
	    then
        	if [ $version = "6" -a $release -eq "5" ]; then
                    modification=`/sbin/uname -R | awk '{split($2, a, "."); print a[3]}' | awk -F'[.a-zA-Z]' '{print $1}'`
                    if [ -z "$modification" ]; then
                        BINARY_TYPE="trix6"
                    else
                        if [ $modification -ge "8" ]; then
                            BINARY_TYPE="trix6.5.24"
                        else
                            BINARY_TYPE="trix6"
                        fi
                    fi 
		else
		    BINARY_TYPE="trix6"
                fi
	    else
        	if [ $version = "6" -a $release -eq "5" ]; then
                    modification=`/sbin/uname -R | awk '{split($2, a, "."); print a[3]}' | awk -F'[.a-zA-Z]' '{print $1}'`
                    if [ -z "$modification" ]; then
                        BINARY_TYPE="sgi6.5"
                    else
                        if [ $modification -ge "8" ]; then
                            BINARY_TYPE="irix6.5.24"
                        else
                            BINARY_TYPE="sgi6.5"
                        fi
                    fi
	        elif [ $version = "6" -a $release -eq "4" ]; then
	            BINARY_TYPE="sgi6"
	        elif [ $version = "6" -a $release -eq "2" ]; then
	            BINARY_TYPE="sgi6"
		fi
	    fi
         else
	    BINARY_TYPE="sgi5"
 	fi
	;;
    SunOS)
        version=`echo $uname_val | $AWK '{split($3, a, "."); print a[1]}'`
        minorver=`echo $uname_val | $AWK '{split($3, a, "."); print a[2]}'`
        machine=`echo $uname_val | $AWK '{print $5}'`
        if [ $version = "4" ]; then
	    BINARY_TYPE="sunos4"
 	else
            if [ "$machine" = "i86pc" ]; then
                BIT64=`/usr/bin/isainfo -vk | grep -c '64.bit' 2> /dev/null`
                if [ "$BIT64" -eq "0" ] ; then
                    if [ "$minorver" = "7" -o "$minorver" = "8" -o "$minorver" = "9" -o "$minorver" = "10" ] ; then
                        if [ "$minorver" = "10" ]; then
                            BINARY_TYPE="x86-sol10"
                        else 
                            BINARY_TYPE="x86-sol7"
                        fi
                    else
                        BINARY_TYPE="x86-sol2"
                    fi
                else
                    if [ "$minorver" = "10" -o "$minorver" -gt "10" ] ; then
                        BINARY_TYPE="x86-64-sol10"
                    fi
                fi
            else
                BINARY_TYPE="sparc-sol2"
                if [ "$minorver" -ge "7" -a "$minorver" -le "9" ]; then
	       	    BIT64=`/usr/bin/isainfo -vk | grep -c '64.bit' 2> /dev/null`
	            if [ "$BIT64" -eq "0" ]; then
		        BINARY_TYPE="sparc-sol7-32"
	            else
		        BINARY_TYPE="sparc-sol7-64"
	            fi
                elif [ "$minorver" -ge "10" ]; then
                    BINARY_TYPE="sparc-sol10-64"
                fi
            fi
 	fi
	;;
    SUPER-UX)
	lastfield=`echo $uname_val | $AWK '{print $NF}'`
	if [ $lastfield = "SX-3" ]; then
	    BINARY_TYPE="sx3"
 	elif [  $lastfield = "SX-4" ]; then
	    BINARY_TYPE="sx4"
        elif [  $lastfield = "SX-5" ]; then
            BINARY_TYPE="sx5"
        elif [  $lastfield = "SX-6" ]; then
            BINARY_TYPE="sx6"
        elif [  $lastfield = "SX-8" ]; then
            BINARY_TYPE="sx8"
        fi
	;;
    ULTRIX)
        BINARY_TYPE="ultrix"
         ;;
    ConvexOS)
        BINARY_TYPE="convex"
         ;;
    *)
        # cray machine
        lastfield=`echo $uname_val | $AWK '{print $NF}'`
        if [ $lastfield = "TS" ]; then
            SUBTITLE=`target -s | cut -d= -f2`
            if [ "$SUBTITLE" = " CRAY-TS-IEEE" ]; then
                BINARY_TYPE="crayt90-ieee"
            else
                BINARY_TYPE="crayt90"
            fi
        elif [ $lastfield = "C90" ]; then
   	    BINARY_TYPE="crayc90"
 	elif  [ $lastfield = "J90" ]; then
	    BINARY_TYPE="crayj90"
	elif  [ $lastfield = "SV1" ]; then
	    BINARY_TYPE="craysv1"
	elif  [ $lastfield = "T3E" ]; then
	    BINARY_TYPE="crayt3e"
        elif  [ $lastfield = "crayx1" ]; then
            BINARY_TYPE="crayx1"
         fi
	;;
    esac
    if [ $BINARY_TYPE = "fail" ]; then
        echo "Cannot get binary type"
    fi
