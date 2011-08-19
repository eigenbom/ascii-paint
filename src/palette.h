#ifndef ASCIIPAINT_PALETTE_H
#define ASCIIPAINT_PALETTE_H

#include <string>
#include "libtcod.hpp"

struct Palette {
	std::string name;
	TCODList<TCODColor> colours;
	int width; // number of colours in each row
	int height; // number of colours in each column (derived from colours.size() and width)
};

#endif
