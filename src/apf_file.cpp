#include "apf_file.h"

#include <fstream>
#include <iostream>

#include "libtcod.hpp"

#include "app.h"
#include "base64.h"

class Indenter{
public:
	Indenter(int _amount):amount(_amount){};
	int amount;
};

std::ostream& operator<<(std::ostream& o, const Indenter& ind){
	for (int i=0;i<ind.amount;i++){
		o << "\t";
	}
	return o;
}

bool ApfFile::Save(std::string filename){
	std::ofstream out(filename.c_str());
	if (!out) return false;

	int in = 0;
	#define OUT out<<Indenter(in)

	OUT << "// Image created by Ascii-Paint\n\n";
	OUT << "ascii_paint_image {\n";
	in++;

	// Version number
	OUT << "version = " << ApfFile::VERSION << "\n";

	// Width, height, encoding, and format
	OUT << "int width = " << app->canvasWidth << "\n";
	OUT << "int height = " << app->canvasHeight << "\n";
	OUT << "string encoding = \"base64\"\n";
	OUT << "string format = \"crgb\"\n";

	OUT << "bool show_grid = " << (app->showGrid?"true":"false") << "\n";
	OUT << "int grid_width = " << app->gridW << "\n";
	OUT << "int grid_height = " << app->gridH << "\n";

	// Canvas Image (Layer)

	OUT << "struct layer {\n";
	{
		in++;

		// Convert image/layer to an encoded binary stream
		const unsigned int numBytesPerPixel = 7; // char + fr + fg + fb + br + bg + bb
		const unsigned int numBytes = numBytesPerPixel*app->canvasHeight*app->canvasWidth;
		unsigned char* layerData = new unsigned char[numBytes];

		// Get image from canvas
		CanvasImage *img = app->getCanvasImage();

		int index = 0;
		// Write the brush data for every brush in the image
		for(int x = 0; x < app->canvasWidth; x++) {
			for(int y = 0; y < app->canvasHeight; y++) {
				Brush b = (*img)[x * app->canvasHeight + y];
				layerData[index++] = b.symbol;
				layerData[index++] = (unsigned char)(b.fore.r);
				layerData[index++] = (unsigned char)(b.fore.g);
				layerData[index++] = (unsigned char)(b.fore.b);
				layerData[index++] = (unsigned char)(b.back.r);
				layerData[index++] = (unsigned char)(b.back.g);
				layerData[index++] = (unsigned char)(b.back.b);
			}
		}

		std::string encodedLayerData = base64_encode(layerData,numBytes);
		delete[]layerData;

		// Output data to file
		OUT << "string data = \"" << encodedLayerData << "\"\n";

		in--;
	}
	OUT << "}\n";

	in--;
	OUT << "}\n";

	#undef OUT

	return true;
}

class ApfFileListener: public ITCODParserListener, AppUser {
public:
	bool imageFound;
	int version;

	int width, height;

	bool showGrid;
	int gridW, gridH;

	std::string encoding, format;

	struct Layer {
		std::string data;
	};

	Layer* currentLayer;
	TCODList<Layer*> layers;

	ApfFileListener():ITCODParserListener(){
		imageFound = false;
		version = -1;

		width = 1;
		height = 1;

		showGrid = true;
		gridW = -1; // by default, we don't change the grid
		gridH = -1;

		encoding = "base64";
		format = "crgb";

		currentLayer = NULL;
	}

	bool parserNewStruct(TCODParser *parser,const TCODParserStruct *str,const char *name) {
		//std::cerr << "str: " << str->getName() << "\n";

		if (strcmp(str->getName(),"ascii_paint_image")==0){
			if (imageFound){
				std::cerr << "Multiple images in one file not allowed\n";
				return false;
			}
			imageFound = true;
		}
		else if (strcmp(str->getName(),"layer")==0) {
			if (imageFound){
				Layer* l = new Layer;
				currentLayer = l;
				layers.push(l);
			}
			else
				return false;
		}

		return true;
	}

	bool parserFlag(TCODParser *parser,const char *name) {
		return true;
	}

	bool parserProperty(TCODParser *parser,const char *name, TCOD_value_type_t type, TCOD_value_t value) {
		// std::cerr << "prop: " << name << "\n";

		if(strcmp(name,"version")==0){
			version = value.i;
			if (version > ApfFile::VERSION){
				std::cerr << "ERROR! Cannot load file as it looks like it was made with a newer version of Ascii-Paint\n";
				return false;
			}
		}
		else if(strcmp(name,"width")==0){
			width = value.i;
		}
		else if (strcmp(name,"height")==0){
			height = value.i;
		}
		else if (strcmp(name,"encoding")==0){
			encoding = value.s;
			if (encoding!="base64"){
				std::cerr << "Unknown encoding scheme!\n";
				return false;
			}
		}
		else if (strcmp(name,"format")==0){
			format = value.s;
			if (format!="crgb"){
				std::cerr << "Unknown format!\n";
				return false;
			}
		}
		else if (strcmp(name,"show_grid")==0){
			showGrid = value.b;
		}
		else if (strcmp(name,"grid_width")==0){
			gridW = value.i;
		}
		else if (strcmp(name,"grid_height")==0){
			gridH = value.i;
		}
		else if (strcmp(name, "data")==0){
			// add the data to this current layer
			if (currentLayer==NULL){
				std::cerr << "\"data\" found outside layer!\n";
				return false;
			}
			else {
				currentLayer->data = std::string(value.s);
			}
		}
		else return false;

		return true;
	}

	bool parserEndStruct(TCODParser *parser,const TCODParserStruct *str,const char *name) {
		//std::cerr << "end: " << str->getName() << "\n";

		if (strcmp(str->getName(),"ascii_paint_image")==0){
			loadData();
		}
		else if (strcmp(str->getName(),"layer")==0){
			if (currentLayer){
				currentLayer = NULL;
			}
		}
		return true;
	}

	void error(const char *msg) {
		std::cerr << "ERROR: " << (msg==NULL?"null":msg) << "\n";
	}

	void loadData(){
		//std::cerr << "loadData\n";

		// get canvas ready
		app->canvasWidth = width;
		app->canvasHeight = height;
		app->initCanvas();

		// grid
		if (gridW!=-1 and gridH!=-1){
			app->setGridDimensions(gridW,gridH);
			app->setShowGrid(showGrid);
		}

		// decode layers
		if (layers.size()>0){
			// for now, just load the first layer
			Layer* l = layers.get(0);

			// decode the data
			std::string decoded = base64_decode(l->data);

			CanvasImage *img = new CanvasImage;

			// Write the brush data for every brush in the image
			int index = 0;
			for(int x = 0; x < app->canvasWidth; x++) {
				for(int y = 0; y < app->canvasHeight; y++) {
					Brush b;
					b.symbol = (unsigned char)(decoded[index++]);
					b.fore.r = (uint8)(decoded[index++]);
					b.fore.g = (uint8)(decoded[index++]);
					b.fore.b = (uint8)(decoded[index++]);
					b.back.r = (uint8)(decoded[index++]);
					b.back.g = (uint8)(decoded[index++]);
					b.back.b = (uint8)(decoded[index++]);
					b.solid = false;
					b.walkable = true;
					img->push_back(b);
				}
			}
			app->setCanvasImage(*img);
			delete img;

			// then free the encoded layer data
			for(int i=0;i<layers.size();i++){
				delete layers.get(i);
			}
		}
	}
};

bool ApfFile::Load(std::string filename){
	TCODParser parser;
	TCODParserStruct *asciiPaintImageStruct = parser.newStructure("ascii_paint_image");
	asciiPaintImageStruct->addProperty("version", TCOD_TYPE_INT, true);
	TCODParserStruct* layerStruct = parser.newStructure("layer");
	parser.run(filename.c_str(), new ApfFileListener());
	return true;
}
