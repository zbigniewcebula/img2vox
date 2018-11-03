#pragma once
#include <iostream>
#include <string.h>
#include <fstream>

using namespace std;

typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned int	uint;

class v4 {
	public:
		union {
			struct {
				union {
					uchar	x, r;
				};
				union {
					uchar	y, g;
				};
				union {
					uchar	z, b;
				};
				union {
					uchar	w, a, i;
				};
			};
			uint val;
		};

		v4() {
			val = 0;
		}
		v4(uint newVal) {
			val = newVal;
		}
		v4(uint R, uint G, uint B, uint A) {
			r = R;
			g = G;
			b = B;
			a = A;
		}
};

class VOX {
	private:
		int 	X, Y, Z;
	public:
		int		Vcount		= 0;
		v4		palette[256];
		uchar*	voxels		= NULL;

		VOX() {
			//
		}
		~VOX() {
			delete[]	voxels;
		}

		void setSize(uint x, uint y, uint z) {
			X	= x;
			Y	= y;
			Z	= z;

			delete[]	voxels;
			voxels	= new uchar[getVoxelCount()];
			memset(voxels, 0, getVoxelCount() * sizeof(uchar));
		}
		uint getVoxelCount() {
			return X * Y * Z;
		}
		uint getSizeX() {
			return X;
		}
		uint getSizeY() {
			return Y;
		}
		uint getSizeZ() {
			return Z;
		}

		uchar getVoxel(uint x, uint y, uint z) {
			if (x < X && y < Y && z < Z) {
				return voxels[getVoxelIndex(x, y, z)];
			}
			return 0; 
		}
		uchar getVoxel(uint idx) {
			if (idx < getVoxelCount()) {
				return voxels[idx];
			}
			return 0; 
		}
		int getVoxelIndex(uint x, uint y, uint z) {
			return (z * Y * X) + (y * X) + x;
		}

		void setVoxel(uint x, uint y, uint z, uint i) {
			if (x < X && y < Y && z < Z) {
				voxels[getVoxelIndex(x, y, z)]	= i;
			}
		}

		void save(string name) {
			ofstream	hFile(name.c_str(), ios::trunc | ios::out | ios::binary);

			int	ver			= 150;	
			int	voxelCount	= getVoxelCount();	
			
			//Temp
			int	chunkSize	= 0;

			//Nagłówki
			hFile.write("VOX ", 4);
			hFile.write((char*)&ver, 4);
			hFile.write("MAIN", 4);
			hFile.write("\0\0\0\0", 4);
			chunkSize	= voxelCount * sizeof(v4) + 0x434;	
			hFile.write((char*)&chunkSize, 4);

			hFile.write("SIZE", 4);
			chunkSize	= 12;
			hFile.write((char*)&chunkSize, 4);
			hFile.write("\0\0\0\0", 4);
			hFile.write((char*)&X, 4);
			hFile.write((char*)&Y, 4);
			hFile.write((char*)&Z, 4);

			//Voxele
			hFile.write("XYZI", 4);
			chunkSize	= 4 + voxelCount * sizeof(v4);
			hFile.write((char*)&chunkSize, 4);
			hFile.write("\0\0\0\0", 4);
			hFile.write((char*)&voxelCount, 4);

			for(uchar z = 0; z < Z; ++z) {
				for(uchar y = 0; y < Y; ++y) {
					for(uchar x = 0; x < X; ++x) {
						uchar v	= getVoxel(x, y, z);

						/*
						cout	<< "v[" << dec << getVoxelIndex(x, y, z)
								<< "]: "
								<< dec << int(x) << " "
								<< dec << int(y) << " "
								<< dec << int(z) << " ("
								<< dec << int(v) << ")"
						<< endl;
						*/
						hFile.write((char*)&x, 1);
						hFile.write((char*)&y, 1);
						hFile.write((char*)&z, 1);
						hFile.write((char*)&v, 1);
					}
				}
			}

			//Paleta
			hFile.write("RGBA", 4);
			chunkSize	= 0x400;
			hFile.write((char*)&chunkSize, 4);
			hFile.write("\0\0\0\0", 4);

			for(int i = 0; i < 256; ++i) {
				v4*	v	= &palette[i];
				/*
				cout	<< "p[" << dec << i << "]: "
						<< hex << int(palette[i].r) << " "
						<< hex << int(palette[i].g) << " "
						<< hex << int(palette[i].b) << " "
						<< hex << int(palette[i].a) << " "
				<< endl;
				*/
				hFile.write((char*)&palette[i], 4);
			}

			hFile.flush();
			hFile.close();
		}
};