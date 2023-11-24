cmake_minimum_required(VERSION 3.12)
project(ctree)

# Add your source files
set(SOURCES src/main.c src/tree.c)

# Find and link against NCurses
find_library(NCURSES_LIBRARY NAMES ncurses)
if (NOT NCURSES_LIBRARY)
    message(FATAL_ERROR "NCurses library not found.")
endif()

# Find and link against Panels
set(PANELS_LIBRARY "/usr/lib/libpanelw.so")  # Adjust the path and library name

# Include directories for external dependencies
include_directories(/usr/lib/)
include_directories(include)

# Define the executable
add_executable(ctree ${SOURCES})

# Link against external libraries
target_link_libraries(ctree ${NCURSES_LIBRARY} ${PANELS_LIBRARY})
