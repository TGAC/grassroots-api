#!/bin/sh
#$RCSfile: pjllib.sh,v $Revision: 1.4 $Date: 2013/01/10 05:55:54 $

#
# pjllib.sh
# shared functions called by mpirun.lsf as well as 
# any PJL wrapper function.
# Should be stored in $LSF_BINDIR after hpcinstall
# 

# 
# construct_task_geo_str
# step 1: get rid of all white spaces and curly brackets {} and (.
# step 2: replace ) with space so that we can use for to get tasks group by group
# step 3: in each group, replace , with white space so that we can get individual task
#
construct_task_geom_str ()
{
task_geom_expr="$1"
str=`echo $task_geom_expr | sed -e "s/[       {}(]//g" | sed -e "s/)/ /g"`
echo $str
}

#
# It moves the group that contains task 0 to
# the beginning of the list
#
top_first_task ()
{
orig_geom="$1"
tmp_geom=`construct_task_geom_str "$orig_geom"`
sortedgeom=""
for item in $tmp_geom 
do
    case $item in
    *",0,"* | "0,"* | *",0" | "0" ) 
        sortedgeom="$item $sortedgeom"
        ;;
    *)
        sortedgeom="$sortedgeom $item"
        ;;
    esac
done
echo $sortedgeom
}

#
# construct_task_geom_file
#
# This function will create a tmp file
# /tmp/taskgeometry.$$
# which contains 3 columns sorted by task id 
# task_id node_name node_index
# node_index starts from 0 to n-1
# To parse out the node_index column,
# use 
# nodelist=`sort -n $result | awk '{ print $3 }'`  
#
construct_task_geom_file ()
{
tmpgeom=$1
tmpHostfile="$2"

NEW_LSB_MCPU_HOSTS=""
if [ -z "$tmpHostfile" -o "$tmpHostfile" = "/dev/null" ]; then
    tmpHostfile=/dev/null
else
    rm -f $tmpHostfile
    touch $tmpHostfile 
fi

i=0
for group in $tmpgeom
do
    tmpgroup=`echo $group | sed -e "s/,/ /g"`
    ntask=`echo $tmpgroup | wc -w`
    ntask=`expr $ntask`
    let host_index=$i*2+1
    let ntask_index=$host_index+1
    host=`echo $LSB_MCPU_HOSTS | cut -d ' ' -f ${host_index}`
    if [ -n "$host" ]; then
        ntask_orig=`echo $LSB_MCPU_HOSTS | cut -d ' ' -f ${ntask_index}`
        ntask_orig=`expr $ntask_orig`
    else
        ntask_orig=0
    fi

    if [ -z "$host" -o "$ntask" -gt "$ntask_orig" ]; then
        if [ -z "$host" ]; then
            echo "LSB_PJL_TASK_GEOMETRY <$LSB_PJL_TASK_GEOMETRY> requests more hosts than LSB_MCPU_HOSTS <$LSB_MCPU_HOSTS> can offer." 
        else 
            echo "Cannot allocate more than ${ntask_orig} tasks on $host"
        fi
        echo "Please "
        echo " 1. Re-define LSB_PJL_TASK_GEOMETRY $LSB_PJL_TASK_GEOMETRY, or" 
        echo " 2. Modify LSF host selection criteria, or "
        echo " 3. Change job execution host order."
        return 1
    fi
    for task in $tmpgroup
    do
        echo $task " " $host " " $i >> $tmpHostfile
    done
    let i=$i+1
    NEW_LSB_MCPU_HOSTS="$NEW_LSB_MCPU_HOSTS $host $ntask"
    NEW_LSB_MCPU_HOSTS=`echo $NEW_LSB_MCPU_HOSTS`
done
export NEW_LSB_MCPU_HOSTS
return 0
}

#
# rm_1st_keyword_line
#
# grep the line that contains the first occurance of keyword.
# and remove that line from the file.
# 
rm_1st_keyword_line ()
{
key_word="$1"
from_file="$2"
mv -f $from_file $from_file.$$

if [ ! -f "$from_file.$$" ]; then
    echo "Cannot mv -f $from_file $from_file.$$".
    return 1
fi

touch $from_file
line=`cat $from_file.$$ | awk '
BEGIN { found=0; matched_ln=""; }
{
if ( found == 0 ) {
    split($0, arr, " ");
    for ( i in arr )
    {
        if ( arr[i] == key_word )
        {
            found = 1;
            break;
        }
    }
    if ( found == 0 ) {
        print $0 >> new_file ;
    } else {
        matched_ln=sprintf("%s", $0);
    }
} else {
    print $0 >> new_file ;
}
}
END { print matched_ln }' key_word=$key_word new_file=$from_file `

rm -f $from_file.$$
echo "$line"
}

#
# reorder_file_based_on_task_geom
# 
# Re-order any file based on the order of
# LSB_PJL_TASK_GEOMETRY
#
reorder_file_based_on_task_geom ()
{
orig_file="$1" 
sort_flag="$2"
if [ ! -f "$orig_file" ]; then
    return 1
fi

task_geom_str=`construct_task_geom_str "$LSB_PJL_TASK_GEOMETRY"`
construct_task_geom_file "$task_geom_str" "/tmp/task_geom_file.$$"
if [ "$?" != "0" ]; then
    return 1
fi

if [ "$sort_flag" = "sort" ]; then
    sort -n -o "/tmp/task_geom_file.$$" "/tmp/task_geom_file.$$"
fi

mv -f $orig_file $orig_file.$$
touch $orig_file.$$
node_name_list=`awk '{print $2}' /tmp/task_geom_file.$$`
rm -f /tmp/task_geom_file.$$
for node_name in $node_name_list
do
    rm_1st_keyword_line "$node_name" $orig_file.$$ >> $orig_file
    if [ "$?" != "0" ]; then
        return 1
    fi
done
rm -f $orig_file.$$ 
return 0
}

#
# syntax_check
#
# Syntax check of LSB_PJL_TASK_GEOMETRY.
# 1. illegal characters other than {}(),[0-9]
# 2. Format of {...}
# 3. Format of (id,id)(id,id)...
# 4. task id must be continuous and starting from 0
#

syntax_check()
{
task_geom_env_var="$1"

illegal_chars=`echo "$task_geom_env_var" | sed -e "s/[0-9,{}() 	]//g"`
if [ -n "$illegal_chars" ]; then
    echo "Wrong format - illegal characters in $task_geom_env_var"
    echo "Format of LSB_PJL_TASK_GEOMETRY: {(id,id,...)(id,id,...)...}"
    return 1
fi

open_bracket=`echo "$task_geom_env_var" | grep "^{" `
close_bracket=`echo "$task_geom_env_var" | grep "}$" `
if [ -z "$open_bracket" -o -z "$close_bracket" ]; then
    echo "Wrong format, Task Geometry must be enclosed in {}"
    echo "Format of LSB_PJL_TASK_GEOMETRY: {(id,id,...)(id,id,...)...}"
    return 1
fi

str=`echo "$task_geom_env_var" | sed -e "s/^{//" | sed -e "s/}$//"`

leftover=`echo $str| sed -e "s/([ 	]*[0-9][0-9]*[ 	]*)//g"`
leftover=`echo $leftover | sed -e "s/[ 	]*,[ 	]*[0-9][0-9]*[ 	]*)//g"`
leftover=`echo $leftover | sed -e "s/[ 	]*,[ 	]*[0-9][0-9]*[ 	]*//g"`
leftover=`echo $leftover | sed -e "s/([ 	]*[0-9][0-9]*[ 	]*//g"`
leftover=`echo $leftover`
if [ -n "$leftover" ]; then
    echo "Wrong format of $task_geom_env_var, unmatched brackets or wrong format in task group specification"
    echo "Format of LSB_PJL_TASK_GEOMETRY: {(id,id,...)(id,id,...)...}"
    return 1
fi

str=`echo $str | sed -e "s/[{}(),]/ /g"`
max_task=`echo $str | wc -w`
max_task=`expr $max_task`
if [ "$max_task" -le "0" ]; then
    echo "Wrong format of $task_geom_env_var, LSB_PJL_TASK_GEOMETRY cannot be NULL if defined"
    echo "Format of LSB_PJL_TASK_GEOMETRY: {(id,id,...)(id,id,...)...}"
    return 1
fi

i=0
while [ $i -lt $max_task ]
do
    case " $str " in
        *" $i "*)
            :
            ;;
        *)
            echo "Task Geometry must be continuous integers and must start from 0"
            echo "Format of LSB_PJL_TASK_GEOMETRY: {(id,id,...)(id,id,...)...}"
            return 1
            ;;
    esac
    let i=$i+1
done

return 0
}
