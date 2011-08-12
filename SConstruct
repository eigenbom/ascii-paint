source_files = Split("""
		app.cpp
		app_data.cpp
		app_gui.cpp
		app_user.cpp
		asc_file.cpp
		cell_operation.cpp
		color_box.cpp
		export_operation.cpp
		fill_operation.cpp
		fov_operation.cpp
		get_string_message_box.cpp
		import_operation.cpp
		line_operation.cpp
		main.cpp
		message_box.cpp
		misc.cpp
		new_operation.cpp
		operation.cpp
		open_operation.cpp
		oval_operation.cpp
		path_operation.cpp
		pick_operation.cpp
		quit_operation.cpp
		rect_operation.cpp
		redo_operation.cpp
		save_operation.cpp
		text_operation.cpp
		undo_operation.cpp
		""");

library_files = Split("""
		libtcod
		libtcod++
		libtcod-gui
		""");

Program('ascii-paint', source_files, LIBS=library_files, CPPPATH='include', LIBPATH='.', CCFLAGS='-g -Wall');
