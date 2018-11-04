#!/bin/bash
for file in ./png_in/*
do
	fm=$(basename $file)
	fm=${fm%.*}
	./main_exe $file ./vox_out/$fm 32
done

python3 vox2obj.py

cp ./vox_out/*.png ./obj_out/

echo "Finished!"