cmake_minimum_required(VERSION 2.8)

project(gaxtapper C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src/3rdparty/include)

if(MSVC)
    if(CMAKE_CL_64)
        set(MSVC_ARCHITECTURE_NAME x64)
    else()
        set(MSVC_ARCHITECTURE_NAME win32)
    endif()

    # Use bundled dependencies
    set(ZLIB_ROOT ${CMAKE_SOURCE_DIR}/dependencies/zlib)
    set(ZLIB_LIBRARY ${CMAKE_SOURCE_DIR}/dependencies/zlib/lib/${MSVC_ARCHITECTURE_NAME}/zlibstat.lib)
    add_definitions(-DZLIB_WINAPI)
endif()

find_package(ZLIB REQUIRED)

if(MSVC)
    option(STATIC_CRT "Use static CRT libraries" ON)

    # Rewrite command line flags to use /MT if necessary
    if(STATIC_CRT)
        foreach(flag_var
                CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
            if(${flag_var} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
            endif(${flag_var} MATCHES "/MD")
        endforeach(flag_var)
    endif()

    # Allow for wildcards in command-line path arguments
    # http://msdn.microsoft.com/en-us/library/8bch7bkk.aspx
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} setargv.obj")

    # Disable MSVC specific secure error
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

#============================================================================
# gaxtapper
#============================================================================

set(SRCS
    src/main.cpp
    src/gaxtapper/cartridge.cpp
    src/gaxtapper/gsf_writer.cpp
    src/gaxtapper/gax_driver.cpp
    src/gaxtapper/gax_song_header.cpp
    src/gaxtapper/psf_writer.cpp
    src/gaxtapper/gaxtapper.cpp
)

set(HDRS
    src/3rdparty/include/args.hxx
    src/3rdparty/include/strict_fstream.hpp
    src/3rdparty/include/zstr.hpp
    src/gaxtapper/arm.hpp
    src/gaxtapper/bytes.hpp
    src/gaxtapper/cartridge.hpp
    src/gaxtapper/gsf_header.hpp
    src/gaxtapper/gsf_writer.hpp
    src/gaxtapper/gax_minigsf_driver_param.hpp
    src/gaxtapper/gax_song_param.hpp
    src/gaxtapper/gax_song_header.hpp
    src/gaxtapper/gax_version.hpp
    src/gaxtapper/gax_driver.hpp
    src/gaxtapper/gax_driver_param.hpp
    src/gaxtapper/psf_writer.hpp
    src/gaxtapper/gaxtapper.hpp
    src/gaxtapper/tabulate.hpp
    src/gaxtapper/types.hpp
)

add_executable(gaxtapper ${SRCS} ${HDRS})

if(ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIRS})
    target_link_libraries(gaxtapper ${ZLIB_LIBRARIES})
endif(ZLIB_FOUND)
