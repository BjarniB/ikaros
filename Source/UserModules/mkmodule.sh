#!/bin/bash

# usage: sh mkmodule.sh modulename

MODULENAME="$1"
ORGNAME="MyModule"
## make new dir with MODULENAME
mkdir $MODULENAME
## copy everything from MyModule
cp -R $ORGNAME/ $MODULENAME/
cd $MODULENAME
ls
## rename all files
for file in $(find . -name "*$ORGNAME*" -type f -maxdepth 2)
    do
        echo '    change' "$file" 'to' `echo "$file" | sed s/$ORGNAME/$MODULENAME/g`
        mv "$file" `echo "$file" | sed s/$ORGNAME/$MODULENAME/g`
    done
ls
sed -e "s/$ORGNAME/$MODULENAME/; s/~$ORGNAME/~$MODULENAME/; s/\&$ORGNAME/\&$MODULENAME/; s:/$ORGNAME:/$MODULENAME:g;" -i "" *.*
cd Examples/
ls
sed -e "s:/$ORGNAME:/$MODULENAME:g" -i "" *.*

## search and replace in files
