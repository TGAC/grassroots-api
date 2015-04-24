#!/bin/sh

print_usage()
{
    cat << _USAGE
Usage:  ${_script_name} 
        ${_script_name} -f 
        ${_script_name} -p
        ${_script_name} -h

-f:  For hosts that belong to just one cluster and the /etc/ego.sudoers file already exists: change the cluster, preserve the existing user list, remove all other contents in the file, and save a backup copy of the old file.
-p:  For hosts that belong to multiple clusters and the /etc/ego.sudoers file already exists: add a new cluster to the path, preserve the existing user list, and save a backup copy of the old file.
-h:  Print command usage and exit.

_USAGE
}

_script_name=`basename "$0"`
_cmd_opt_str="$@"
_num_of_opts="$#"
_arg_p=""

if [ "$_num_of_opts" = "0" ] ; then
    :
elif [ "$_num_of_opts" -gt "1" ] ; then
    print_usage
    exit 1
else
    for _opt in $_cmd_opt_str 
    do
        case $_opt in
            -h)
                print_usage
                exit 0
                ;;
            -f)
                :
                ;;
            -p)
                _arg_p="y"
                ;;
            *)
                echo "    Command line option $_opt is not valid."
                print_usage
                exit 1
                ;;
        esac
    done
fi



RUNNER=`id | sed -e 's/[^(]*(\([^)]*\)).*/\1/'`
if [ "$RUNNER" != "root" ]; then
    echo "You should run it as root"
    exit 1
fi

if [ "$EGO_BINDIR" = "" ] ; then
    echo "Cannot find environment variable EGO_BINDIR. Exiting..."
    exit 1
fi

if [ "$EGO_SERVERDIR" = "" ] ; then
    echo "Cannot find environment variable EGO_SERVERDIR. Exiting..."
    exit 1
fi

_sudo_file=/etc/ego.sudoers
_sudo_command=$EGO_BINDIR/egosh

if  [ ! -f $_sudo_command ] ; then
    echo "Cannot find command egosh. Please check your environment. Exiting..."
    exit 1
fi

_clusteradmin=`ls -l $EGO_SERVERDIR/lim | awk ' { print $3 } ' `
if [ "$_clusteradmin" = "" ] ; then
    echo "Cannot find cluster administrator. Please check your environment. Exiting..."
    exit 1 
fi

if [ -f $_sudo_file ]; then
    if [ "$_cmd_opt_str" != "-f" -a "$_cmd_opt_str" != "-p" ]; then
    echo "$_sudo_file already exists. "
    echo "Use egosetsudoers.sh -f or egosetsudoers.sh -p to update the existing $_sudo_file file."
    echo "Exiting..."
    exit 1
    fi
fi

chown root $_sudo_command
if [ "$?" != "0" ] ; then
    echo "Can not change the owner of $_sudo_command to root. Please check your environment. Exiting..."
    exit 1
fi

chmod 4755 $_sudo_command
if [ "$?" != "0" ] ; then
    echo "Can not set uid for $_sudo_command. Please check your environment. Exiting..."
    exit 1
fi

if [ ! -f $_sudo_file -a "$_cmd_opt_str" = "-p" ] ; then
    echo "The file $_sudo_file does not exist. To create it, run $_script_name without the -p option. Exiting..."
    exit 1
fi

if [ -f $_sudo_file ] ; then
    _old_user_list=`cat $_sudo_file | grep "EGO_STARTUP_USERS" | sed -e "s/^.*=//g" | sed -e "s/\"//g"`
    _old_alte_patch=`cat $_sudo_file | grep "^[ 	]*EGO_STARTUP_ALTERNATE_PATHS" | sed -e "s/^.*=//g" | sed -e "s/\"//g"`
    _old_path=`cat $_sudo_file | grep "^[ 	]*EGO_STARTUP_PATH" | sed -e "s/^.*=//g" | sed -e "s/\"//g"`
    if [ "$_old_path" = "$EGO_SERVERDIR" ]; then
        chmod 600 $_sudo_file  
        echo "egosetsudoers succeeds" 
        exit 0
    fi
    if [ "$_old_alte_patch" != "" -a "$_cmd_opt_str" = "-p" ] ; then
        _alte_patch=`cat $_sudo_file | grep "EGO_STARTUP_ALTERNATE_PATHS=" | cut -d'=' -f2`
        _new_alte_patch=":$_alte_patch:"
        _path_exists=`echo $_new_alte_patch | grep ":$EGO_SERVERDIR:"`
        if [ "$_path_exists" != "" ]; then
            chmod 600 $_sudo_file  
            echo "egosetsudoers succeeds" 
            exit 0
        fi
    fi
    if [ ! -f $_sudo_file.old ] ; then
        echo "Saving the existing file $_sudo_file to $_sudo_file.old."
        cp -f $_sudo_file $_sudo_file.old 
    else 
        echo "Saving the existing file $_sudo_file to $_sudo_file.old.$$."
        cp -f $_sudo_file $_sudo_file.old.$$
    fi

    if [ "$_old_user_list" != "" ] ; then
        _exist="n"
        for _temp_user in $_old_user_list
        do
            if [ "$_temp_user" = "$_clusteradmin" ] ; then
                _exist="y"
                break
            fi
        done

        if [ "$_exist" = "n" ] ; then
            _clusteradmin="$_clusteradmin $_old_user_list"
        else 
            _clusteradmin="$_old_user_list"
        fi
    fi
fi

if [ "$_cmd_opt_str" != "-p" ] ; then
cat << EOF > $_sudo_file  
EGO_STARTUP_PATH=$EGO_SERVERDIR
EGO_STARTUP_USERS="$_clusteradmin"
EOF
else
    if [ "$_old_alte_patch" != "" -a "$_path_exists" = "" ]; then
        cat $_sudo_file | sed '/EGO_STARTUP_ALTERNATE_PATHS=/d' > $_sudo_file.$$.temp
	mv $_sudo_file.$$.temp $_sudo_file
    cat << EOF >> $_sudo_file
EGO_STARTUP_ALTERNATE_PATHS=${_old_alte_patch}:${EGO_SERVERDIR}
EOF
    elif [ "$_old_alte_patch" = "" ]; then
    cat << EOF >> $_sudo_file
EGO_STARTUP_ALTERNATE_PATHS=$EGO_SERVERDIR
EOF
    fi
fi

if [ -f $_sudo_file  ]; then
    chmod 600 $_sudo_file  
    echo "egosetsudoers succeeds" 
fi

