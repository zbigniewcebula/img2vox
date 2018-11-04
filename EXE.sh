#!/bin/bash
echo "Removing old data..."
rm -rf ./obj_out
rm -rf ./vox_out
mkdir vox_out

if [ $# -eq 0 ]
then	
	echo "Cloning/Pulling 1000-blades repository..."
else
	echo "Entering PNG generator..."
fi
if [ -d "./1000-blades" ]
then
	cd ./1000-blades
	git pull
else
	if [ $# -eq 0 ]
	then
		git clone https://github.com/joshuaskelly/1000-blades
	fi
	cd ./1000-blades
fi

echo "PNG Generation..."
python3 generate.py
cd ./out/textures

echo "PNG Split..."
if [ ! -d "./split" ]
then
	mkdir split
fi
convert *.png -crop 32x32 ./split/%05d.png

echo "App check..."
cd ../../../
if [ -f "main_exe" ]
then
	echo "png2vox Exists!"
else
	echo "Compiling png2vox..."
	bash ./main.cpp
fi

echo "PNG => VOX"
for file in ./1000-blades/out/textures/split/*
do
	fm=$(basename $file)
	fm=${fm%.*}
	./main_exe $file ./vox_out/$fm 32
done

echo "VOX => OBJ"
python3 vox2obj.py

echo "PNG =+> OBJ"
cp ./vox_out/*.png ./obj_out/

echo "Finished!"