# this macro group source into there folders
macro(source_group_by_dir SOURCE_FILES)
    foreach(FILE ${SOURCE_FILES})
        get_filename_component(PARENT_DIR "${FILE}" PATH)
        string(REGEX REPLACE "(\\./)?(Source)/?" "" GROUP "${PARENT_DIR}")
        string(REPLACE "/" "\\" GROUP "${GROUP}")
        source_group("${GROUP}" FILES "${FILE}")
    endforeach(FILE)
endmacro(source_group_by_dir)