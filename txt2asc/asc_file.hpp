#ifndef ASCII_PAINT_ASC_LOAD_HPP
#define ASCII_PAINT_ASC_LOAD_HPP

struct AscRgb {
	int r;
	int g;
	int b;

};

class AscFile {
	public:
		AscFile();
		~AscFile();

		int load(char *filename);
		int save(char *filename);

		int getWidth();
		int getHeight();

		void setSize(int width, int height);

		void setChar(int x, int y, unsigned char chr);

		void setFore(int x, int y, int r, int g, int b);
		void setFore(int x, int y, AscRgb f);

		void setBack(int x, int y, int r, int g, int b);
		void setBack(int x, int y, AscRgb b);
		
		void setSolid(int x, int y, bool solid);
		void setWalkable(int x, int y, bool walkable);

		unsigned char getChar(int x, int y);
		AscRgb getFore(int x, int y);
		AscRgb getBack(int x, int y);
		bool getSolid(int x, int y);
		bool getWalkable(int x, int y);

	private:
		int width;
		int height;
		
		AscRgb *fore;
		AscRgb *back;
		unsigned char *chars;
		bool *solid;
		bool *walkable;
};


#endif
