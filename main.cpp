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

typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned int	uint;

#include "CImg.h"
#include "MagicaVoxel.h"
#include "MagicaVoxelSave.h"

using namespace std;
using namespace cimg_library;

int main(int argc, char** argv) {
	if (argc < 3 || argc > 5) {
		cerr	<< "ERROR! Usage: img2vox IMG.* OUT.vox [Z canvas height] [PALETTE.png]" << endl; 
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

	cout	<< "VOX Palette remaping and palette file generation..." << endl;

	CImg<unsigned char> palette(256, 1, 1, 4, 0);

	for(auto it = pal.begin(); it != pal.end(); ++it) {
		uchar idx	= it->second - 1;
		v4	clr(it->first);
		model.palette[idx].val	= clr.val;
		palette(idx, 0, 0, 0)	= clr.r;
		palette(idx, 0, 0, 1)	= clr.g;
		palette(idx, 0, 0, 2)	= clr.b;
		palette(idx, 0, 0, 3)	= clr.a;
	}
	if (argc == 5) {
		palette.save(argv[4]);
	} else {
		palette.save("palette.png");
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
