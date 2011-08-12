ADD_EXECUTABLE(ascii-paint
    app.cpp
    app_data.cpp
    app_gui.cpp
    app_user.cpp
    asc_file.cpp
    cell_operation.cpp
    color_box.cpp
    colour_widget.cpp
    copy_operation.cpp
    export_operation.cpp
    fill_operation.cpp
    fov_operation.cpp
    get_string_message_box.cpp
    import_operation.cpp
    lbutton.cpp
    line_operation.cpp
    lradio_button.cpp
    main.cpp
    message_box.cpp
    misc.cpp
    new_operation.cpp
    open_operation.cpp
    operation.cpp
    oval_operation.cpp
    paste_operation.cpp
    path_operation.cpp
    pick_operation.cpp
    quit_operation.cpp
    rect_operation.cpp
    redo_operation.cpp
    resize_operation.cpp
    save_operation.cpp
    slider2.cpp
    small_toolbar.cpp
    symbols_widget.cpp
    text_operation.cpp
    undo_operation.cpp
)

target_link_libraries(ascii-paint tcod-mingw tcod-gui-mingw)

INSTALL(TARGETS ascii-paint DESTINATION .)
if(WIN32)
	file(COPY ${BASE_DIR}/lib/libtcod-gui-mingw.dll 
						${BASE_DIR}/lib/libtcod-mingw.dll 
						${BASE_DIR}/lib/SDL.dll 
			DESTINATION ${BASE_DIR}/bin)
endif(WIN32)
