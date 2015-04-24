#!/bin/csh
#--------------------------------------------------------------------------
# This code gets the binary type when setting LSF user environment.
# It is based on found_binary_type() in scripts/funcs.sh.
#
# (c) Copyright International Business Machines Corp 1992-2012.
# US Government Users Restricted Rights - Use, duplication or disclosure 
# restricted by GSA ADP Schedule Contract with IBM Corp.
#---------------------------------------------------------------------------

set _CUR_PATH_ENV = "$PATH"
setenv PATH /usr/bin:/bin:/usr/local/bin:/local/bin:/sbin:/usr/sbin:/usr/ucb:/usr/sbin:/usr/bsd:${PATH}

# handle difference between system V and BSD echo
# To echo "foo" with no newline, do
# echo $enf "foo" $enl

if ( "`echo -n`" == "-n" )  then
    set enf=
    set enl="\c"
else
    set enf=-n
    set enl=
endif

  
set LSNULFILE=/dev/null

# Find a version of awk we can trust

set echo_on = `set | grep ^echo | grep -v ^echo_`
if ( "$echo_on" == "echo" ) then
    unset echo
endif

set AWK=""
foreach tmp (awk  /usr/toolbox/nawk gawk nawk)
#   This is the real test, for functions & tolower
#    if foo=`(echo FOO | $tmp 'function tl(str) { return tolower(str) } { print tl($1) }') 2>$LSNULFILE` \
#       && test "$foo" = "foo"
#   A simpler test, just for executability
    set val_tmp=`$tmp 'BEGIN{ } { }'< $LSNULFILE |& cat`
    if ( "$val_tmp" == "" )  then
	set AWK=$tmp
        break
    endif
end

if ( "$echo_on" == "echo" ) then
    set echo
endif
unset echo_on

if (  "$AWK" == "") then
    echo "Cannot find a correct version of awk."
    echo "Exiting ..."
    exit 1
endif
  
   set uname_val=`uname -a`
   set BINARY_TYPE="fail"
   set BINARY_TYPE_HPC=""
   set field1=`echo $uname_val |$AWK '{print $1}'`

   switch ($field1) 

    case UNIX_System_V:
	 set BINARY_TYPE="uxp"
	 breaksw

    case OSF1:
         set version=`echo $uname_val | $AWK '{split($3, a, "."); print a[1]}'`

         if ( $version == "V5" ) then
             set BINARY_TYPE="alpha5"
         else if ( $version == "T5" ) then
                set BINARY_TYPE="alpha5"
              else
                 set BINARY_TYPE="alpha" 
              endif
         endif

	 if ( $BINARY_TYPE == "alpha5" ) then
	    #Kite#31479
            set tmp=`/usr/sbin/rcmgr get SC_MS`
            if ( "X$tmp" != "X" ) then
                set BINARY_TYPE="alpha5-rms"
            endif
         endif # binary type is alpha5
         breaksw
     case HI-UX: 
	 set BINARY_TYPE="hppa"
         breaksw
     case HP-UX: 
        # spp need to run uname -C to find the version
        uname -C >& $LSNULFILE
        if ( $status == "1" ) then 
            set version=`echo $uname_val | $AWK '{split($3, a, "."); print a[2]}'`
	    if ( $version == "09" ) then
	       set BINARY_TYPE="hppa9"
            else if ( $version == "10" ) then
               set subver=`echo $uname_val | $AWK '{split($3, a, "."); print a[3]}'`
               if ( $subver == "10" ) then
                   set BINARY_TYPE="hppa10"
               else if ( $subver == "20" ) then
                   set BINARY_TYPE="hppa10.20"
               endif
            else if ( $version == "11" ) then
                set machine=`echo $uname_val | $AWK '{print $5}'`       
                set subver=`echo $uname_val | $AWK '{split($3, a, "."); print a[3]}'`
                set kernel=`/usr/bin/getconf KERNEL_BITS`

                if ("$machine" == "ia64" ) then
                    set BINARY_TYPE_HPC="hpuxia64 hppa11i-64 hppa11-64"
                    set BINARY_TYPE="hppa11-64"
                else
                    if ( "$subver" == "00" ) then
                        if ( "$kernel" == "32" ) then
                            set BINARY_TYPE="hppa11-32"
                        else
                            set BINARY_TYPE="hppa11-64"
                        endif
                    else
                        if ( "$kernel" == "32" ) then
                            set BINARY_TYPE="hppa11-32"
                            set BINARY_TYPE_HPC="hppa11i-32 hppa11-32"
                            # reset to standard LSF binary type 
                            foreach ARCH (hppa11i-32 hppa11-32)
                                if ( -f $LSF_TOP/$LSF_VERSION/$ARCH/etc/mbatchd ) then 
                                    set BINARY_TYPE="$ARCH" 
                                    break;
                                endif
                            end
                        else
                            set BINARY_TYPE="hppa11-64"
                            set BINARY_TYPE_HPC="hppa11i-64 hppa11-64"
                            # reset to standard LSF binary type 
                            foreach ARCH (hppa11i-64 hppa11-64)
                                if ( -f $LSF_TOP/$LSF_VERSION/$ARCH/etc/mbatchd ) then 
                                    set BINARY_TYPE="$ARCH" 
                                    break;
                                fi
                            end
                        endif
                     endif
                endif
            else 
                echo "Cannot figure out the HP version."
		exit 1
            endif 
         else
	   set version=`uname -C -a |  $AWK '{split($3, a, "."); print a[1]}'` 
	    if ( $version == "5" ) then
	     set  BINARY_TYPE="spp"
            else if  ( $version == "4" ) then
	     set  BINARY_TYPE="spp4"
             endif
         endif
         breaksw
    case Linux:
        set version=`echo $uname_val | $AWK '{split($3, a, "."); print a[1]}'`
        set _machine=`uname -m`
        #get glibc version
        set LIBC_SO=`ldconfig -p | grep libc.so.6 | sed 's/.*=>//'`
        foreach glibc ( $LIBC_SO )
            if ( -x "$glibc" ) then
                $glibc >& /dev/null
                if ( "$status" == "0" ) then
                    set _libcver=`$glibc | grep "GNU C Library" | $AWK '{print substr($0,match($0,/version/))}' | $AWK '{print $2}' | $AWK -F. '{print $2}'| sed 's/,//'`
                    if ( "$_libcver" != "" ) then
                        break
                    endif
                endif
            endif
        end

        if ( "$_libcver" == "" ) then
            echo "Cannot figure out the GLibc version."
            exit 1
        endif

        if ( $version == "1" ) then
	   set BINARY_TYPE="linux"
	endif
        if (  $version >= "2" ) then
            set subver=`echo $uname_val | $AWK '{split($3, a, "."); print a[1]"."a[2]}'`

            if ( $subver == "2.0" ) then
                set BINARY_TYPE="linux2"
                if ( "$_machine" == "alpha" ) then
                    set BINARY_TYPE="linux2-alpha"
                else
                    set BINARY_TYPE="linux2-intel"
                endif
            endif

            if ( $subver == "2.2" ) then
                set BINARY_TYPE="linux2.2"
    
                if ( "$_machine" == "alpha" ) then
		    set BINARY_TYPE="linux2.2-glibc2.1-alpha"
                else
                    if ( "$_machine" == "sparc" || "$_machine" == "sparc64" ) then
		        set BINARY_TYPE="linux2.2-glibc2.1-sparc"
		    else if ( "$_machine" == "ppc" ) then
		        set BINARY_TYPE="linux2.2-glibc2.1-powerpc"
                    else
                        set BINARY_TYPE="linux2.2-glibc2.1-x86"
	            endif		 
                endif
            endif

            if ( $subver == "2.4" ) then
               set BINARY_TYPE="linux2.4"

# ia64 is the family of 64-bit CPUs from Intel. We shouldn't need a new
# distribution for each processor
	       # *** test if this is an RMS host *** 
	       # we don't want to see error message when "which rmsquery" fails
               # so use >& to redirect stdout and stderr to the null file
               # and check the exit code of the command
 	       set is_rms_host="1"
               which rmsquery >& $LSNULFILE
               if ( $status == "0" ) then
                   set RMSQUERY=`which rmsquery`
                   if ( -x "$RMSQUERY" ) then
                       # In csh, you can redirect stdout with ">",
                       # or stdout and stderr together with ">&" but
                       # there is no direct way to redirect stderr only.
                       $RMSQUERY 'select name from nodes' >& $LSNULFILE
                       if ( $status == "0" ) then
                           set HOST_NAME=`hostname | cut -d. -f 1`
                           set RMS_HOST_LIST=`$RMSQUERY 'select name from nodes'` 
		           foreach RMS_HOST ( $RMS_HOST_LIST )
			      if ( "$RMS_HOST" == "$HOST_NAME" ) then 	   
                                 set is_rms_host="0"
                                 set RMS_VERSION=`printenv RMS_VERSION`
                                 if ( "$RMS_VERSION" == "" ) then
                                     set RMS_VERSION=`rpm -q qsrmslibs | sed -e "s/qsrmslibs\-//g" | sed -e "s/\-.*//g"`
                                     #
                                     # This is safe, see /usr/bin/rmscheckenv, it greps "not installed"
                                     #
                                     set NOT_INSTALLED=`echo $RMS_VERSION | grep "not installed"`
                                     if ( "$NOT_INSTALLED" != "" ) then
                                         set is_rms_host="1"
                                         set RMS_VERSION=""
                                     endif
                                     if ( "$RMS_VERSION" != "" && "$RMS_VERSION" != "2.80" && "$RMS_VERSION" != "2.81" ) then
                                         set RMS_VERSION="2.82"
                                     endif
                                 endif
				 break
                              endif
			   end
                        endif
                    endif
               endif
	       # Use libbproc.so to check whether this host is bproc host
	       set is_bproc_host="0"
	       if ( -f /usr/lib/libbproc.so ) then
                    set is_bproc_host="1"
               endif

               # check if this is a slurm host
               set is_slurm_host="1"
               if ( -f /var/lsf/lsfslurm ) then
                   set is_slurm_host="0"
               endif
               if ( "$_machine" == "ia64" ) then
		   if ( "$_libcver" == "1" ) then
		       set BINARY_TYPE="linux2.4-glibc2.1-ia64"
		   else if ( "$_libcver" == "2" ) then
                       # check if Dist system
                       ls -l /etc/cassatt-release >& $LSNULFILE
                       if ( "$status" == "0" ) then
                           set BINARY_TYPE="linux2.4-glibc2.2-ia64-dist"
                       else
	                   set BINARY_TYPE="linux2.4-glibc2.2-ia64"
                       endif

		       if ( "$is_rms_host" == "0" ) then
		           set BINARY_TYPE_HPC="rms${RMS_VERSION}_linux2.4-glibc2.2-ia64 linux2.4-glibc2.2-ia64"
                       endif
                       # check libcpuset.so file and see whether we 
		       # are running on cpuset system
		       if ( -f /usr/lib/libcpuset.so || -f /lib32/libcpuset.so || -f /lib64/libcpuset.so ) then
                           set BINARY_TYPE_HPC="linux2.4-glibc2.2-sn-ipf"
                       endif
		   else
                           if ( "$_libcver" == "1" ) then
                               set BINARY_TYPE="linux2.4-glibc2.1-ia64"
                           else if ( "$_libcver" == "2" ) then
                                   set BINARY_TYPE="linux2.4-glibc2.2-ia64"
                           else
                                   set BINARY_TYPE="linux2.4-glibc2.3-ia64"
                           endif
		       if ( "$is_rms_host" == "0" ) then
		           set BINARY_TYPE_HPC="rms${RMS_VERSION}_linux2.4-glibc2.2-ia64 linux2.4-glibc2.3-ia64"
                       endif
                       # check libcpuset.so file and see whether we 
		       # are running on cpuset system
		       if ( -f /usr/lib/libcpuset.so || -f /lib32/libcpuset.so || -f /lib64/libcpuset.so ) then
                           set BINARY_TYPE_HPC="linux2.4-glibc2.3-sn-ipf"
                       endif
		   endif

               else if ( "$_machine" == "alpha" ) then
                    set BINARY_TYPE="linux2.4-glibc2.2-alpha"
		    if ( "$is_rms_host" == "0" ) then
		         set BINARY_TYPE="rms${RMS_VERSION}_linux2.4-glibc2.2-alpha"
                    endif
               else if ( "$_machine" == "ppc64" ) then
                    set BINARY_TYPE="linux2.4-glibc2.2-ppc64"
               else if ( "$_machine" == "s390" ) then
                    set BINARY_TYPE="linux2.4-glibc2.2-s390-32"
               else if ( "$_machine" == "s390x" ) then
                    set BINARY_TYPE="linux2.4-glibc2.2-s390x-64"
               else if ( "$_machine" == "armv5l" ) then
		    set BINARY_TYPE="linux2.4-glibc2.2-armv5l"
               else if ( "$_machine" == "x86_64" ) then
                      set _cputype=`cat /proc/cpuinfo | grep -i vendor | $AWK '{print $3}' | uniq`

		    if ( "$_libcver" == "1" ) then
			set BINARY_TYPE="linux2.4-glibc2.1-x86_64"
		    else if ( "$_libcver" == "2" || "$_libcver" == "3" ) then 
                        if ( -f /opt/cray/rca/default/bin/rca-helper && -f /etc/opt/cray/sdb/node_classes ) then
                            set nid=`/opt/cray/rca/default/bin/rca-helper -i`
                            set nid=`expr $nid`
			    set role=`grep "^${nid}:" /etc/opt/cray/sdb/node_classes | cut -f2 -d':'`
                        else if ( -f /etc/node_classes ) then
                            set nid=`xtuname`
                            set nid=`expr $nid`
			    set role=`grep "^${nid}:" /etc/node_classes | cut -f2 -d':' `
                        else
                            set role=unknown
                        endif
			
			if ( "$role" == "login" || "$role" == "service" ) then
			    set BINARY_TYPE="linux2.4-glibc2.3-x86_64-cray"
			else
			    set BINARY_TYPE="linux2.4-glibc2.2-x86_64"
			endif
		    endif

		    if ( "$is_slurm_host" == "0" ) then
                        # slurm: it needs slurm binaries.
			# no need to fall back to normal LSF binary type
                        set BINARY_TYPE="linux2.4-glibc2.3-x86_64-slurm"
                    endif
               else 
	            echo $uname_val | $AWK '{print $3}' | grep "_Scyld" >& $LSNULFILE 
		    if ( "$status" == "0" ) then
		         set BINARY_TYPE="linux2.4-glibc2.2-x86-scyld"
                    else
                      set BINARY_TYPE="fail"
                      if ( "$_libcver" == "1" ) then
                          set BINARY_TYPE="linux2.4-glibc2.1-x86"
                      else if ( "$_libcver" == "2" ) then
                          # check if Dist system
                          ls -l /etc/cassatt-release >& $LSNULFILE
                          if ( "$status" == "0" ) then
                              set BINARY_TYPE="linux2.4-glibc2.2-x86-dist"
                          else
                              set BINARY_TYPE="linux2.4-glibc2.2-x86"
			      if ( -d "/proc/vmware" ) then
				  set BINARY_TYPE="linux2.4-glibc2.2-x86"
			      endif
			  endif
                      else if ( "$_libcver" == "3" ) then
                          set BINARY_TYPE="linux2.4-glibc2.3-x86"
                      endif 
		      if ( "$is_rms_host" == "0" ) then
		            set BINARY_TYPE="rms${RMS_VERSION}_${BINARY_TYPE}"
                      endif
		      if ( "$is_bproc_host" == "1" ) then
		            set BINARY_TYPE="${BINARY_TYPE}-bproc"
                      endif
                      if ( "$is_slurm_host" == "0" ) then
                        # slurm: it needs slurm binaries.
                        # no need to fall back to normal LSF binary type
                          set BINARY_TYPE="${BINARY_TYPE}-slurm"
                      endif
                    endif 
               endif
            endif
            if ( $subver == "2.6" || $version > "2" ) then
               set BINARY_TYPE="fail"

	       # *** test if this is an RMS host *** 
	       # we don't want to see error message when "which rmsquery" fails
               # so use >& to redirect stdout and stderr to the null file
               # and check the exit code of the command
 	       set is_rms_host="1"
               which rmsquery >& $LSNULFILE
               if ( $status == "0" ) then
                   set RMSQUERY=`which rmsquery`
                   if ( -x "$RMSQUERY" ) then
                       # In csh, you can redirect stdout with ">",
                       # or stdout and stderr together with ">&" but
                       # there is no direct way to redirect stderr only.
                       $RMSQUERY 'select name from nodes' >& $LSNULFILE
                       if ( $status == "0" ) then
                           set HOST_NAME=`hostname | cut -d. -f 1`
                           set RMS_HOST_LIST=`$RMSQUERY 'select name from nodes'` 
		           foreach RMS_HOST ( $RMS_HOST_LIST )
			      if ( "$RMS_HOST" == "$HOST_NAME" ) then 	   
                                 set is_rms_host="0"
                                 set RMS_VERSION=`printenv RMS_VERSION`
                                 if ( "$RMS_VERSION" == "" ) then
                                     set RMS_VERSION=`rpm -q qsrmslibs | sed -e "s/qsrmslibs\-//g" | sed -e "s/\-.*//g"`
                                     #
                                     # This is safe, see /usr/bin/rmscheckenv, it greps "not installed"
                                     #
                                     set NOT_INSTALLED=`echo $RMS_VERSION | grep "not installed"`
                                     if ( "$NOT_INSTALLED" != "" ) then
                                         set is_rms_host="1"
                                         set RMS_VERSION=""
                                     endif
                                     if ( "$RMS_VERSION" != "" && "$RMS_VERSION" != "2.80" && "$RMS_VERSION" != "2.81" ) then
                                         set RMS_VERSION="2.82"
                                     endif
                                 endif
				 break
                              endif
			   end
                        endif
                    endif
               endif

               set is_slurm_host="1"
               if ( -f /var/lsf/lsfslurm ) then
                   set is_slurm_host="0"
               endif

               switch ($_machine)
                   case ppc64:
                       if ( "$_libcver" == "3" ) then
                           set BINARY_TYPE="linux2.6-glibc2.3-ppc64"
                           set BINARY_TYPE_HPC="linux2.6-glibc2.3-ppc64-bluegene linux2.6-glibc2.3-ppc64"
                       else if ( "$_libcver" >= "4" ) then
                           set BINARY_TYPE="linux2.6-glibc2.3-ppc64"
                       endif
                   breaksw
                   case ia64:
                       if ( "$_libcver" == "1" ) then
                           set BINARY_TYPE="linux2.6-glibc2.1-ia64"
                       else if ( "$_libcver" == "2" ) then
                           set BINARY_TYPE="linux2.6-glibc2.2-ia64"
                       else if ( "$_libcver" == "3" ) then
                           set BINARY_TYPE="linux2.6-glibc2.3-ia64"
                           # check libcpuset.so file and see whether we 
		           # are running on cpuset system
		           if ( -f /usr/lib/libcpuset.so || -f /lib32/libcpuset.so || -f /lib64/libcpuset.so ) then
                               set BINARY_TYPE_HPC="linux2.6-glibc2.3-sn-ipf"
                           endif
                       else if ( "$_libcver" == "4" ) then
                           set BINARY_TYPE="linux2.6-glibc2.3-ia64"
			   # check libcpuset.so file and see whether we
			   # are running on cpuset system
			   if ( -f /usr/lib/libcpuset.so || -f /lib32/libcpuset.so || -f /lib64/libcpuset.so ) then
			       set BINARY_TYPE="linux2.6-glibc2.3-ia64"
			       set BINARY_TYPE_HPC="linux2.6-glibc2.4-sn-ipf"
                           endif
                       else if ( "$_libcver" >= "5" ) then
                           set BINARY_TYPE="linux2.6-glibc2.3-ia64"
			   # check libcpuset.so file and see whether we
			   # are running on cpuset system
			   if ( -f /usr/lib/libcpuset.so || -f /lib32/libcpuset.so || -f /lib64/libcpuset.so ) then
			       set BINARY_TYPE="linux2.6-glibc2.3-ia64"
			       set BINARY_TYPE_HPC="linux2.6-glibc2.5-sn-ipf"
                           endif
                       endif

		       # Check for RMS host 
		       if ( "$is_rms_host" == "0" ) then
                           set BINARY_TYPE="rms${RMS_VERSION}_${BINARY_TYPE}"
                       endif

		       # Check for SLURM
		       if ( "$is_slurm_host" == "0" ) then
		          # slurm: it needs slurm binaries.
			  # no need to fall back to normal LSF binary type
			  set BINARY_TYPE="${BINARY_TYPE}-slurm"
                       endif
                       breaksw
                   case x86_64:
                       if ( "$_libcver" == "1" ) then
                          set BINARY_TYPE="linux2.6-glibc2.1-x86_64"
                       else if ( "$_libcver" == "2" ) then
                          set BINARY_TYPE="linux2.6-glibc2.2-x86_64"
                       else if ( "$_libcver" == "3" ) then
                          set BINARY_TYPE="linux2.6-glibc2.3-x86_64"
			  #
			  #Check BProc/BProc4
			  #
			  if ( -f /usr/lib/libbproc.so ) then
			      set BINARY_TYPE="${BINARY_TYPE}-bproc"
			      if ( -f /usr/lib/libbproc.so.4 ) then
			          set BINARY_TYPE="${BINARY_TYPE}4"
                              endif
			  endif
			  
			  #
			  # check rms
			  #
                          if ( "$is_rms_host" == "0" ) then
                             set BINARY_TYPE_HPC="rms${RMS_VERSION}_${BINARY_TYPE} ${BINARY_TYPE}"
                          endif
                       else if ( "$_libcver" >= "4" ) then
                          set BINARY_TYPE="linux2.6-glibc2.3-x86_64"
                       endif
                       #Check CRAY XT3:
                       if ( -f /opt/cray/rca/default/bin/rca-helper && -f /etc/opt/cray/sdb/node_classes ) then
                           set nid=`/opt/cray/rca/default/bin/rca-helper -i`
                           set nid=`expr $nid`
		           set role=`grep "^${nid}:" /etc/opt/cray/sdb/node_classes | cut -f2 -d':'`
                       else if ( -f /etc/node_classes ) then
		           set nid=`xtuname`
                           set nid=`expr $nid`
			   set role=`grep "^${nid}:" /etc/node_classes | cut -f2 -d':' `
		       else
			   set role=unknown
		       endif
			
		       if ( "$role" == "login" || "$role" == "service" ) then
			   set BINARY_TYPE="${BINARY_TYPE}-cray"
		       endif

		       # Check SLURM
		       if ( "$is_slurm_host" == "0" ) then
		           # slurm: it needs slurm binaries.
			   # no need to fall back to normal LSF binary type
			   set BINARY_TYPE="${BINARY_TYPE}-slurm"
                       endif
                       breaksw
                   case i[3456]86: 
                       if ( "$_libcver" == "1" ) then
                           set BINARY_TYPE="linux2.6-glibc2.1-x86"
                       else if ( "$_libcver" == "2" ) then
                           set BINARY_TYPE="linux2.6-glibc2.2-x86"
                       else if ( "$_libcver" == "3" ) then
                           set BINARY_TYPE="linux2.6-glibc2.3-x86"
                           if ( "$is_rms_host" == "0" ) then
		               set BINARY_TYPE_HPC="rms${RMS_VERSION}_${BINARY_TYPE} ${BINARY_TYPE}"
                           endif
                       else if ( "$_libcver" == "4" ) then
                           set BINARY_TYPE="linux2.6-glibc2.3-x86"
                       else if ( "$_libcver" == "5" ) then
                           set BINARY_TYPE="linux2.6-glibc2.3-x86"
                       else if ( "$_libcver" == "6" ) then
                           set BINARY_TYPE="linux2.6-glibc2.3-x86"
                       else if ( "$_libcver" >= "7" ) then
                           set BINARY_TYPE="linux2.6-glibc2.3-x86"
                       endif
                       breaksw
               endsw
            endif

        endif
        breaksw
    case UNIX_SV:
       set BINARY_TYPE="mips-nec"
        breaksw
    case Darwin:
       set BINARY_TYPE="macosx"
       breaksw
    case NEWS-OS:
	set BINARY_TYPE="mips-sony"
 	breaksw
    case AIX:
        set version=`echo $uname_val | $AWK '{print $4}'`
        set release=`echo $uname_val | $AWK '{print $3}'`
        if ( $version == "4" && $release >= "1" ) then  
            if ( $release < "2" ) then
                set BINARY_TYPE="aix4"
            else
                set BINARY_TYPE="aix4.2"
            endif
        else if ( $version == "5" && $release >= "1" ) then
            #On AIX, only in 64bit mode, you can see shlap64 process
            #check shlap64 process is safer than check kernel_bit
            set _shlap64=`ps -e -o "comm" |grep -c shlap64`
            if ( "$_shlap64" != "0" )  then
               set BINARY_TYPE="aix-64"
            else
	        set BINARY_TYPE="aix-32"
            endif
         else if ( $version == "3" ) then
             set BINARY_TYPE="aix3"
         else if ( $version == "6" ) then
            set _shlap64=`ps -e -o "comm" |grep -c shlap64`
            if ( "$_shlap64" != "0" )  then
               set BINARY_TYPE="aix-64"
            else
	        set BINARY_TYPE="aix-32"
            endif
  	 else if ( $version == "7" ) then
            set _shlap64=`ps -e -o "comm" |grep -c shlap64`
            if ( "$_shlap64" != "0" )  then
               set BINARY_TYPE="aix-64"
            else
	        set BINARY_TYPE="aix-32"
            endif
         endif
	breaksw
    case IRIX*:
        set version=`echo $uname_val | $AWK '{split($3, a, "."); print a[1]}'`
        set release=`echo $uname_val | $AWK '{split($3, a, "."); print a[2]}'`
        if ( $version == "6" && $release >= "2" ) then
          set mls=`sysconf MAC`
          if ( "$mls" == "1" ) then
	      if ( $version == "6" && $release == "5" ) then
                  set modification=`/sbin/uname -R | awk '{split($2, a, "."); print a[3]}' | awk -F'[.a-zA-Z]' '{print $1}'`
                  if ( "$modification" >= "8" ) then
                      set BINARY_TYPE="trix6.5.24"
                  else
                      set BINARY_TYPE="trix6"
                  endif
              else
                  set BINARY_TYPE="trix6"
              endif
          else
	      if ( $version == "6" && $release == "5" ) then
                  set modification=`/sbin/uname -R | awk '{split($2, a, "."); print a[3]}' | awk -F'[.a-zA-Z]' '{print $1}'`
                  if ( "$modification" >= "8" ) then
                      set BINARY_TYPE="irix6.5.24"
                  else
                      set BINARY_TYPE="sgi6.5"
                  endif
              else if ( $version == "6" && $release == "4" ) then
                  set BINARY_TYPE="sgi6"
              else if ( $version == "6" && $release == "2" ) then
                  set BINARY_TYPE="sgi6"
              endif
          endif
        else
          set BINARY_TYPE="sgi5"
        endif

	breaksw
    case SunOS:
        set version=`echo $uname_val | $AWK '{split($3, a, "."); print a[1]}'`
        set minorver=`echo $uname_val | $AWK '{split($3, a, "."); print a[2]}'`
        set machine=`echo $uname_val | $AWK '{print $5}'`
        if ( $version == "4" ) then
	    set BINARY_TYPE="sunos4"
 	else
            if ( "$machine" == "i86pc" ) then
               set BIT64=`/usr/bin/isainfo -vk | grep -c '64.bit'`

               if ( "$BIT64" == "0" ) then
 
                   if ( "$minorver" == "7" || "$minorver" == "8" || "$minorver" == "9" || "$minorver" == "10" ) then
                       if ( "$minorver" == "10" ) then
                           set BINARY_TYPE="x86-sol10"
                       else
                           set BINARY_TYPE="x86-sol7"
                       endif
                   else
                       set BINARY_TYPE="x86-sol2"
                   endif
               else
                   if ("$minorver" >= "10") then
                       set BINARY_TYPE="x86-64-sol10"
                   endif
               endif
            else
               set BINARY_TYPE="sparc-sol2"
               if ( "$minorver" >= "7" && "$minorver" <= "9" ) then
     		    set BIT64=`/usr/bin/isainfo -vk | grep -c '64.bit'`
		
	       	    if ( "$BIT64" == "0" ) then
		        set BINARY_TYPE="sparc-sol7-32"
		    else
		        set BINARY_TYPE="sparc-sol7-64"
                    endif
               else if ( "$minorver" >= "10" ) then
                   set BINARY_TYPE="sparc-sol10-64"
                endif
            endif
 	endif
	breaksw
    case SUPER-UX:
	set lastfield=`echo $uname_val | $AWK '{print $NF}'`
	if ( $lastfield == "SX-3" ) then
	   set BINARY_TYPE="sx3"
 	else if (  $lastfield == "SX-4" ) then
	    set BINARY_TYPE="sx4"
        else if ( $lastfield == "SX-5") then
            set BINARY_TYPE="sx5"
        else if ( $lastfield == "SX-6") then
            set BINARY_TYPE="sx6"
        else if ( $lastfield == "SX-8") then
            set BINARY_TYPE="sx8"
	endif
	breaksw
    case ULTRIX:
         set BINARY_TYPE="ultrix"
         breaksw
    case ConvexOS:
         set BINARY_TYPE="convex"
         breaksw
    default:
        # cray machine
        set lastfield=`echo $uname_val | $AWK '{print $NF}'`
        if ( $lastfield == "TS" ) then
            set SUBTITLE=`target -s | cut -d= -f2`
            if ( "$SUBTITLE" == " CRAY-TS-IEEE" ) then
                set BINARY_TYPE="crayt90-ieee"
            else
                set BINARY_TYPE="crayt90"
            endif
        else if ( $lastfield == "C90" ) then
   	    set BINARY_TYPE="crayc90"
 	else if  ( $lastfield == "J90" ) then
	    set BINARY_TYPE="crayj90"
	else if ( $lastfield == "SV1" ) then
	    set BINARY_TYPE="craysv1"
	else if ( $lastfield == "T3E" ) then
	    set BINARY_TYPE="crayt3e"
        else if ( $lastfield == "crayx1" ) then
            set BINARY_TYPE="crayx1"
        endif
        breaksw
    endsw
    if ( $BINARY_TYPE == "fail" ) then
        echo "Cannot get binary type."
    else
    endif
