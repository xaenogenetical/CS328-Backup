#!/bin/bash
echo 'Name of directory to compress:'
read directory
echo 'What to name the compressed file(no extension):'
read name
cd $directory
make clean
cd ..
cp -R $directory $name 
tar cvfz $name.tar.gz $name
rm -r $name
mv $name.tar.gz tarballs/
