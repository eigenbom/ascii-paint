#include <cstdio>
#include "asc_file.hpp"

AscFile::AscFile() {
}

AscFile::~AscFile() {
}

int AscFile::load(char *filename) {
	FILE *fp;

	fp = fopen(filename, "r");

	if(fp == NULL) {
		return -1;
	}

	float version;
	fscanf(fp, "ASCII-Paint v%g", &version);

	fscanf(fp, "%i %i", &width, &height);

	setSize(width, height);

	// Scan until we hit #(the marker for the start of image data)
	while(fgetc(fp) != '#');

	for(int x = 0; x < width; x++) {
		for(int y = 0; y < height; y++) {
			char c = fgetc(fp);
			int fr = fgetc(fp);
			int fg = fgetc(fp);
			int fb = fgetc(fp);
			int br = fgetc(fp);
			int bg = fgetc(fp);
			int bb = fgetc(fp);
			int solid = fgetc(fp);
			int walkable = fgetc(fp);
			setChar(x, y, c);
			setFore(x, y, fr, fg, fb);
			setBack(x, y, br, bg, bb);
			setSolid(x, y, solid);
			setSolid(x, y, walkable);
		}
	}
	fclose(fp);

	return 1;

}

int AscFile::save(char *filename) {
	FILE *fp;

	fp = fopen(filename, "w");

	if(fp == NULL)
		return 0;

	float version = 0.1;
	fprintf(fp, "ASCII-Paint v%g\n", version);
	fprintf(fp, "%i %i\n", width, height);

	// Use the # character as a marker for the start of image data
	fputc('#', fp);

	// Write the brush data for every brush in the image
	for(int x = 0; x < width; x++) {
		for(int y = 0; y < height; y++) {
			char c = getChar(x, y);
			AscRgb f = getFore(x, y);
			AscRgb b = getBack(x, y);

			fputc(c, fp);
			fputc(f.r, fp);
			fputc(f.g, fp);
			fputc(f.b, fp);
			fputc(b.r, fp);
			fputc(b.g, fp);
			fputc(b.b, fp);
		}
	}
	fclose(fp);
	return 1;
}

int AscFile::getWidth() {
	return width;
}

int AscFile::getHeight() {
	return height;
}

void AscFile::setSize(int w, int h) {
	width = w;
	height = h;

	fore = new AscRgb[width * height];
	back = new AscRgb[width * height];
	chars = new unsigned char[width * height];
	solid = new bool[width * height];
	walkable = new bool[width * height];
}

void AscFile::setChar(int x, int y, unsigned char chr) {
	chars[x * height + y] = chr;
}

void AscFile::setFore(int x, int y, int r, int g, int b) {
	fore[x * height + y].r = r;
	fore[x * height + y].g = g;
	fore[x * height + y].b = b;
}

void AscFile::setFore(int x, int y, AscRgb f) {
	setFore(x, y, f.r, f.g, f.b);
}

void AscFile::setBack(int x, int y, int r, int g, int b) {
	back[x * height + y].r = r;
	back[x * height + y].g = g;
	back[x * height + y].b = b;
}

void AscFile::setBack(int x, int y, AscRgb b) {
	setFore(x, y, b.r, b.g, b.b);
}

void AscFile::setSolid(int x, int y, bool s) {
	solid[x * height + y] = s;
}

void AscFile::setWalkable(int x, int y, bool w) {
	solid[x * height + y] = w;
}

unsigned char AscFile::getChar(int x, int y) {
	return chars[x * height + y];
}

AscRgb AscFile::getFore(int x, int y) {
	AscRgb rgb;

	rgb.r = fore[x * height + y].r;
	rgb.g = fore[x * height + y].g;
	rgb.b = fore[x * height + y].b;

	return rgb;
}

AscRgb AscFile::getBack(int x, int y) {
	AscRgb rgb;

	rgb.r = back[x * height + y].r;
	rgb.g = back[x * height + y].g;
	rgb.b = back[x * height + y].b;

	return rgb;
}

bool AscFile::getSolid(int x, int y) {
	return solid[x * height + y];
}

bool AscFile::getWalkable(int x, int y) {
	return walkable[x * height + y];
}
