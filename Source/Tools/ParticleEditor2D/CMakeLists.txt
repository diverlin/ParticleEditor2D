#
# Copyright (c) 2014 the ParticleEditor2D project.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

# Define target name
set (TARGET_NAME ParticleEditor2D)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(OpenGL REQUIRED)

add_definitions(${QT_DEFINITIONS})

# Find Urho3D library
find_package (Urho3D REQUIRED)
include_directories (${URHO3D_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

# Define source files
define_source_files ()

# Moccing h files
qt5_wrap_cpp( MOC_FILES ${H_FILES} )

# Compile ui files
file (GLOB UI_FILES *.ui)
qt5_wrap_ui( UI_HEADERS ${UI_FILES} )

# Rccing qrc files
file (GLOB QRC_FILES *.qrc)
qt5_add_resources( RCC_FILES ${QRC_FILES} )

set (SOURCE_FILES ${SOURCE_FILES} ${MOC_FILES} ${UI_HEADERS} ${RCC_FILES})

# Setup target with resource copying
setup_executable (TOOL)

target_link_libraries(${TARGET_NAME} Qt5::Core Qt5::Gui Qt5::Widgets ${OPENGL_LIBRARIES})


macro(make_rc_symlink)
    set(resource_root "${CMAKE_CURRENT_SOURCE_DIR}/../../../Bin")
    set(dist_root "${CMAKE_CURRENT_BINARY_DIR}/../bin/tool")

    add_custom_target(link_target ALL
                      COMMAND ${CMAKE_COMMAND} -E create_symlink ${resource_root}/CoreData ${dist_root}/CoreData
                      COMMAND ${CMAKE_COMMAND} -E create_symlink ${resource_root}/Data ${dist_root}/Data
                      )
endmacro(make_rc_symlink)

if (NOT URHO3D_TOOLS)
    make_rc_symlink()
endif()

