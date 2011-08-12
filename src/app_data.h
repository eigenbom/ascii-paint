#ifndef ASCII_PAINT_APP_DATA_H
#define ASCII_PAINT_APP_DATA_H

#include "app_user.h"
#include <string>

class AppData : public AppUser {
	public:
		AppData();
		~AppData();

		void load();
		void save();
		
		void setFilename(char *name);

		std::string filename;
};


#endif
