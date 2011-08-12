#ifndef ASCII_PAINT_BRUSH_H
#define ASCII_PAINT_BRUSH_H

class TCODConsole;
struct Brush {
	TCODColor fore;
	TCODColor back;
	unsigned char symbol;
	bool solid;
	bool walkable;
};

void applyBrushToCell(int x, int y, Brush &brush, TCODConsole* con);

#endif
