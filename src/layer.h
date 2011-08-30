#ifndef ASCIIPAINT_LAYER_H
#define ASCIIPAINT_LAYER_H

#include "libtcod.hpp"

#include <string>

enum LayerCompositingMode {
	Normal,
	// More to come!
};

struct Layer {
	/** The name of the layer.
	 * It must:
	 * - be unique (in a document)
	 * - consist of only four characters
	 */
	std::string name;
	int fgalpha; // 0-255
	int bgalpha; // 0-255
	LayerCompositingMode compositingMode;
	bool visible;
	TCODConsole *canvasCon;
};

#endif
