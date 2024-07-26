#!/bin/bash
#
# Example command line script to upload files.
#
# Save as ftp_tool.sh and make it executable (chmod +x ftp_tool.sh)
#
# ./ftp_tool.sh HOST USER PWD DIR FILES
#
# HOST : hostname | IP address
# USER : username to logon with
# PASS : password of USER
# DIR  : directory to upload to (. is login folder)
# FILES: a space seperates list of filenames, wildcards allowed
#
# ./ftp_tool.sh 192.168.2.20 user secret .  file1.png file2.txt files* file5
# ./ftp_tool.sh 192.168.2.20 user secret /A file1.png file2.txt files* file5
#
HOST=$1
USER=$2
PASS=$3
DIR=$4
# ${@:5} is an array of file names starting at parameter 5
FILES="${@:5}"

ftp -inv $HOST <<EOF
passive
user $USER $PASS
mkdir $DIR
cd $DIR
mput $FILES
bye
EOF
