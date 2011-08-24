#ifndef ASCIIPAINT_APFFILE_H
#define ASCIIPAINT_APFFILE_H

#include "app_user.h"
#include <string>

/**
 * A .apf file is the new file-format for ascii-paint.
 * This class provides helpers for loading and saving to a .apf file.
 *
 * @author ben
 */
class ApfFile: public AppUser {
	public:

	/// Version of .apf this code supports
	static const int VERSION = 1;

	/// Save the current document to file filename
	static bool Save(std::string filename);

	/// Load the file into the current document
	static bool Load(std::string filename);
};

#endif
