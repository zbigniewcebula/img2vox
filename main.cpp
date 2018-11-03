#if 0
#!/bin/bash
g++ main.cpp --std=c++11 -L/usr/X11R6/lib -lm -lpthread -lX11 -o _main && _main
exit
#endif

#pragma comment(lib, "gdi32.lib")
//cls && g++ main.cpp --std=c++11 -lgdi32 -o main.exe && main.exe

#include <iostream>
#include <map>
#include <cmath>
#ifdef _WIN32
	#include <windows.h>
#endif

#include "CImg.h"
#include "MagicaVoxel.h"
#include "MagicaVoxelSave.h"

using namespace std;
using namespace cimg_library;

int main(int argc, char** argv) {
	if (argc < 3 || argc > 4) {
		cerr	<< "ERROR! Usage: img2vox IMG.* OUT.vox [Z canvas height]" << endl; 
		return 1;
	}

	cout	<< "Image loading...";
	CImg<unsigned char> image(argv[1]);
	cout	<< " DONE!" << endl;

	map<uint, int>	pal;
	map<uint, int>	palPos;
	int				palCount	= 0;

	cout	<< "Palette loading and mapping";
	for(int y = 0; y < image.height(); ++y) {
		for(int x = 0; x < image.width(); ++x) {
			v4	pos(x, image.height() - 1 - y, 0, 0);
			v4	pix(
				image(x, y, 0, 0),
				image(x, y, 0, 1),
				image(x, y, 0, 2),
				0xFF
			);

			auto it	= pal.find(pix.val);
			if (it == pal.end()) {
				pal[pix.val]	= palCount;
				it				= pal.find(pix.val);
				++palCount;
				cout << ".";
			}

			if (pix.a > 0) {
				palPos[pos.val]	= it->second;
			} else {
				palPos[pos.val]	= 255;
			}
		}
	}
	cout << endl;
	if (palCount > 255) {
		cerr	<< "ERROR! Too many colors in image (limited palette to 256)!" << endl;
		return 1;
	}

	int	zHeight	= 1;
	if (argc == 4) {
		zHeight	= max(atoi(argv[3]), 1);
	}


	cout	<< "VOX generation..." << endl;
	VOX		model;
	model.setSize(image.width(), image.height(), zHeight);

	for(int z = 0; z < model.getSizeZ(); ++z) {
		for(int y = 0; y < model.getSizeY(); ++y) {
			for(int x = 0; x < model.getSizeX(); ++x) {
				v4	pos(x, model.getSizeY() - 1 - y, z, 0);
				model.setVoxel(
					pos.x, pos.y, pos.z,
					pos.z == 0? palPos[pos.val]: 0
				);
			}
		}
	}

	cout	<< "VOX Palette remaping..." << endl;
	for(auto it = pal.begin(); it != pal.end(); ++it) {
		model.palette[it->second - 1].val	= it->first;
	}

	cout	<< "VOX File save...";
	model.save(argv[2]);
	cout	<< " DONE!" << endl;
	return 0;
}

/*
	MV_Model	model;
	if (model.LoadModel("./untitled.vox")) {
		cout << "===Yay" << endl;
	} else {
		cout << "=== :(" << endl;
	}
	*/
