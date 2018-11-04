#!/bin/bash
echo "Removing old data..."
rm -rf ./obj_out
rm -rf ./vox_out
mkdir vox_out

echo "PNG => VOX"
for file in ./png_in/*
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