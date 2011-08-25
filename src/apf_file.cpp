#include "apf_file.h"

#include <cstdio>
#include <iostream>
#include <cstring>
#include "libtcod.hpp"
#include "app.h"

/*
 * An .apf file is a RIFF file with custom segments.
 *
 * RIFF resources:
 * - http://en.wikipedia.org/wiki/Resource_Interchange_File_Format
 * - http://www.kk.iij4u.or.jp/~kondo/wave/mpidata.txt
 * - http://msdn.microsoft.com/en-us/library/ms713231
 */

// These should be defined if not already
/*
typedef unsigned long uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
*/

//********* Endianness helpers
static bool hasDetectedBigEndianness = false;
static bool isBigEndian;
void detectBigEndianness(){
	if (!hasDetectedBigEndianness){
		uint32 Value32;
		uint8 *VPtr = (uint8 *)&Value32;
		VPtr[0] = VPtr[1] = VPtr[2] = 0; VPtr[3] = 1;
		if(Value32 == 1) isBigEndian = true;
		else isBigEndian = false;
		hasDetectedBigEndianness = true;
	}
}

uint16 bswap16(uint16 s){
	uint8* ps = (uint8*)&s;
	uint16 res;
	uint8* pres = (uint8*)&pres;
	pres[0] = ps[1];
	pres[1] = ps[0];
}

uint32 bswap32(uint32 s){
	__builtin_bswap32(s); // gcc only?
}

uint16 l16(uint16 s){
	if (isBigEndian) return bswap16(s); else return s;
}

uint32 l32(uint32 s){
	if (isBigEndian) return bswap32(s); else return s;
}

// fix the endianness
void fix(uint16& u){
	u = l16(u);
}

void fix(uint32& u){
	u = l32(u);
}

//*********** RIFF data structures and helpers

typedef struct {
	uint32	ckID;	// Chunk type identifier
	uint32	ckSize; // Chunk size field (size of ckData)
	uint8	ckData[]; // Chunk data (sizeof ckSize)
} Chunk;

uint32 fourCC(const char* c){
	return (*(uint32*)c);
}

// checks if u equals str
bool fourCCequals(uint32 u, const char* str){
	return fourCC(str)==u;
}

void put8(uint8 d, FILE* fp){
	fwrite(&d,1,1,fp);
}

void put16(uint16 d, FILE* fp){
	fwrite(&d,2,1,fp);
}

void put32(uint32 d, FILE* fp){
	fwrite(&d,4,1,fp);
}

void putFourCC(const char* c, FILE* fp){
	put32(fourCC(c),fp);
}

void putData(void* what, int length, FILE* fp){
	fwrite(what,length,1,fp);
}

bool get8(uint8* u, FILE* fp){
	return 1==fread((void*)u, sizeof(uint8),1,fp);
}

bool get16(uint16* u, FILE* fp){
	return 1==fread((void*)u, sizeof(uint16),1,fp);
}

bool get32(uint32* u, FILE* fp){
	return 1==fread((void*)u, sizeof(uint32),1,fp);
}

bool getData(void* u, size_t sz, FILE* fp){
	return 1==fread(u, sz,1,fp);
}


//********* APF RIFF structures

typedef struct {
	uint32 show_grid;
	uint32 grid_width;
	uint32 grid_height;
} SettingsDataV1;

#define FILTER_TYPE_UNCOMPRESSED 0
#define FORMAT_TYPE_CRGBRGB 0

typedef struct {
	uint32 width;
	uint32 height;
	uint32 filter;
	uint32 format;
} ImageDetailsV1;

typedef struct {
	uint32 name; // a four character name
	uint32 mode; // compositing mode (0 for now)
	uint32 index; // index into layer list (should mirror order in file)
	uint32 dataSize; // size of the data chunk immediately following this (redundancy check)
	// uint8* data; // the data
} LayerV1;

// fix the endianness
void fix(SettingsDataV1& s){
	fix(s.show_grid);
	fix(s.grid_width);
	fix(s.grid_height);
}

void fix(ImageDetailsV1& v){
	fix(v.width);
	fix(v.height);
	fix(v.filter);
	fix(v.format);
}

void fix(LayerV1& l){
	// l.name
	fix(l.mode);
	fix(l.index);
	fix(l.dataSize);
}


//*********** ApfFile

bool ApfFile::Save(std::string filename){
	detectBigEndianness();

	FILE* fp = fopen(filename.c_str(), "wb");
	if(fp == NULL) {
		printf("The file %s could not be saved.\n", filename.c_str());
		return false;
	}
	else {
		// riff header
		putFourCC("RIFF",fp);
		fpos_t posRiffSize; // store the location to write the size later
		fgetpos(fp,&posRiffSize);
		put32(0,fp); // blank size for now
		uint32 riffSize = 0;

			// APF_ header
			putFourCC("APF ",fp);
			riffSize += 4;

				// settings
				SettingsDataV1 settingsData;
				settingsData.show_grid = l32(app->showGrid?1:0);
				settingsData.grid_width = l32(app->gridW);
				settingsData.grid_height = l32(app->gridH);
				uint32 settingsSz = sizeof(uint32) + sizeof settingsData;
				putFourCC("sett",fp);
				put32(l32(settingsSz),fp);
				put32(l32(1),fp); // settingsData version
				putData((void*)&settingsData,sizeof settingsData,fp);
				if (settingsSz&1){
					put8(0,fp); // pad byte
					riffSize++;
				}
				riffSize += 4+4+settingsSz;

				// image details
				ImageDetailsV1 imgData;
				imgData.width = app->canvasWidth;
				imgData.height = app->canvasHeight;
				imgData.filter = FILTER_TYPE_UNCOMPRESSED;
				imgData.format = FORMAT_TYPE_CRGBRGB;
				uint32 imgDetailsSize = sizeof(uint32) + sizeof imgData;
				putFourCC("imgd",fp);
				put32(l32(imgDetailsSize),fp);
				put32(l32(1),fp); // imgData version
				putData((void*)&imgData,sizeof imgData,fp);
				if (imgDetailsSize&1){
					put8(0,fp);
					riffSize++;
				}
				riffSize += 4+4+imgDetailsSize;

				// now write the layers as a RIFF list
				// Assume imgData filter = uncompressed, and imgData format = CRGB
				uint32 layerImageSize = imgData.width*imgData.height*7;
				uint32 layerChunkSize = sizeof(uint32) /* version */
						+ sizeof(LayerV1) /* header */
						+ layerImageSize; /* data */


				//uint32 LISTSize = sizeof(uint32) /*list type: "layr"*/
				//				+ sizeof(uint32) /*first "layr"*/
				//				+ sizeof(uint32) /*layr size*/
				//				+ layerChunkSize
				//				+ ((layerChunkSize&1)?1:0);

				//putFourCC("LIST",fp);
				//put32(l32(LISTSize),fp);
				//putFourCC("LAYR",fp); // layr list
					putFourCC("layr",fp); // layer
					put32(l32(layerChunkSize),fp);
						put32(l32(1),fp); // version!
						putFourCC("dflt",fp); // name of layer
						put32(l32(0),fp); // mode
						put32(l32(0),fp); // index
						put32(l32(layerImageSize),fp);

						// now write out the data
						CanvasImage *img = app->getCanvasImage();

						// Write the brush data for every brush in the image
						for(int x = 0; x < app->canvasWidth; x++) {
							for(int y = 0; y < app->canvasHeight; y++) {
								Brush b = (*img)[x * app->canvasHeight + y];
								put8(b.symbol, fp);
								put8(b.fore.r, fp);
								put8(b.fore.g, fp);
								put8(b.fore.b, fp);
								put8(b.back.r, fp);
								put8(b.back.g, fp);
								put8(b.back.b, fp);
							}
						}
					if (layerChunkSize&1){
						put8(0,fp); // padding bit
						riffSize++;
					}

				riffSize += 2*sizeof(uint32)+layerChunkSize;

		fsetpos(fp,&posRiffSize);
		put32(l32(riffSize),fp);
	}

	fclose(fp);
	return true;
}


struct LayerData {
	LayerV1 header;
	uint8* data; // dynamically allocated
};

struct Data {
	ImageDetailsV1 details;
	SettingsDataV1 settings;
	LayerData* currentLayer;
	TCODList<LayerData*> layers;
};

bool ApfFile::Load(std::string filename){
	detectBigEndianness();

	uint32 sett = fourCC("sett");
	uint32 imgd = fourCC("imgd");
	uint32 LIST = fourCC("LIST");
	uint32 LAYR = fourCC("LAYR");
	uint32 layr = fourCC("layr");

	Data data; // File data

	data.details.width = 1;
	data.details.height = 1;
	data.details.filter = FILTER_TYPE_UNCOMPRESSED;
	data.details.format = FORMAT_TYPE_CRGBRGB;

	data.settings.show_grid = true;
	data.settings.grid_width = 10;
	data.settings.grid_height = 10;

	data.currentLayer = NULL;

	#define ERR(x) {printf("Error: %s\n. Aborting operation.",x); return false;}
	#define ERR_NEWER(x) {printf("Error: It looks like this file was made with a newer version of Ascii-Paint\n. In particular the %s field. Aborting operation.",x); return false;}

	FILE* fp = fopen(filename.c_str(), "rb");
	if(fp == NULL) {
		printf("The file %s could not be loaded.\n", filename.c_str());
		return false;
	}
	else {
		// read the header
		uint32 riff;
		if (not get32(&riff,fp)
			or
			not fourCCequals(riff,"RIFF")){
			ERR("File doesn't have a RIFF header");
		}
		// else
		uint32 riffSize;
		if (!get32(&riffSize,fp)) ERR("No RIFF size field!");
		fix(riffSize);

		bool keepGoing = true;
		while(keepGoing and fp){ // for each subfield, try to find the APF_ field
			uint32 apf;
			if (not get32(&apf,fp)) break;
			if (fourCCequals(apf,"APF ")){
				// Process APF segment
				while(keepGoing and fp){
					uint32 seg;
					if (not get32(&seg,fp)){
						keepGoing = false;
						break;
					}
					else {
						if (seg==sett){
							// size
							uint32 sz;
							get32(&sz,fp);
							fix(sz);
							// version
							uint32 ver;
							get32(&ver,fp);
							fix(ver);
							if (ver!=1) ERR_NEWER("settings");
							// ver must be 1
							SettingsDataV1 settingsData;
							if (not getData((void*)&settingsData,sizeof settingsData,fp)) ERR("Can't read settings.");
							data.settings = settingsData;
							fix(data.settings);

							// Change app settings
							app->setGridDimensions(data.settings.grid_width,data.settings.grid_height);
							app->setShowGrid(data.settings.show_grid==1);
						}
						else if (seg==imgd){
							// sz
							uint32 sz;
							get32(&sz,fp);
							fix(sz);
							// version
							uint32 ver;
							get32(&ver,fp);
							fix(ver);
							if (ver!=1) ERR_NEWER("image details");
							// ver must be 1
							ImageDetailsV1 dets;
							if (not getData((void*)&dets, sizeof dets, fp)) ERR("Can't read image details.");
							data.details = dets;
							fix(data.details);

							// get canvas ready
							app->canvasWidth = data.details.width;
							app->canvasHeight = data.details.height;
							app->initCanvas();
						}
						else if (seg==layr){
							// printf("Found a layer\n");

							// sz
							uint32 sz;
							get32(&sz,fp);
							fix(sz);
							// version
							uint32 ver;
							get32(&ver,fp);
							fix(ver);
							if (ver!=1) ERR_NEWER("layer spec");
							LayerV1 layerHeader;
							if (not getData((void*)&layerHeader, sizeof layerHeader, fp)) ERR("Can't read layer header.");
							fix(layerHeader);

							// creat new layer data
							LayerData* ld = new LayerData;
							ld->header = layerHeader; // already fix'd
							ld->data = new uint8[ld->header.dataSize];

							// Read in the data chunk
							getData((void*) ld->data, ld->header.dataSize, fp);

							// push layer onto the list
							data.currentLayer = ld;
							data.layers.push(ld);
						}
						else {
							// skip unknown segment
							uint32 sz;
							get32(&sz,fp);
							fix(sz);
							fseek(fp,sz,SEEK_CUR);
						}
					}
				}

				// we're done!
				keepGoing = false;
			}
			else {
				// skip this segment
				uint32 sz;
				get32(&sz,fp);
				fseek(fp,sz,SEEK_CUR);
			}
		}

		// finally, copy the layers into the current document
		if (data.layers.size()>0){
			// for now, just load the first layer
			LayerData* l = data.layers.get(0);

			// TODO: parse header
			// l->header
			uint8* imgData = l->data;

			CanvasImage *img = new CanvasImage;
			// Write the brush data for every brush in the image
			int index = 0;
			for(int x = 0; x < app->canvasWidth; x++) {
				for(int y = 0; y < app->canvasHeight; y++) {
					Brush b;
					b.symbol = (unsigned char)(imgData[index++]);
					b.fore.r = (uint8)(imgData[index++]);
					b.fore.g = (uint8)(imgData[index++]);
					b.fore.b = (uint8)(imgData[index++]);
					b.back.r = (uint8)(imgData[index++]);
					b.back.g = (uint8)(imgData[index++]);
					b.back.b = (uint8)(imgData[index++]);
					b.solid = false; // deprecated
					b.walkable = true; // deprecated
					img->push_back(b);
				}
			}

			app->setCanvasImage(*img);
			delete img;

			// then free all the temporary layer data
			for(int i=0;i<data.layers.size();i++){
				delete[]data.layers.get(i)->data;
				delete data.layers.get(i);
			}
		}


	}
	fclose(fp);

	return true;
}
