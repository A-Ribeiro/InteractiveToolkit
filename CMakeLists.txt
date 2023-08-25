cmake_minimum_required (VERSION 3.5.1)

project(InteractiveToolkit)

# "For this is how God loved the world:
# he gave his only Son, so that everyone
# who believes in him may not perish
# but may have eternal life."
#
# John 3:16

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

ENABLE_LANGUAGE(CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(MSVC)
    #disable size_t warning conversion... on 64build
    #add_compile_options( /wd4267 )
endif()

# create CMAKE_BUILD_TYPE for non-windows platform
if( NOT WIN32 AND NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Release CACHE STRING
            "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
            FORCE )
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS None Debug Release RelWithDebInfo MinSizeRel)
endif()

# automatically setup IDE source group. Useful for VSStudio
macro(define_source_group )
    foreach(entry IN ITEMS ${ARGN})
        get_filename_component(dirname "${entry}" DIRECTORY )
        if (dirname)
            string(REPLACE "/" "\\" dirname_replaced ${dirname})
            source_group(${dirname_replaced} FILES ${entry})
        else()
            source_group("" FILES ${entry})
        endif()
    endforeach()
endmacro()

macro(configure_build_flags projectname inputfile outputfile)
    set(cmake_build_flags "")

    set(target_compile_defs)
    if (TARGET ${projectname})
        get_target_property(target_compile_defs ${projectname} COMPILE_DEFINITIONS)
        if ("${target_compile_defs}" STREQUAL "target_compile_defs-NOTFOUND")
            set(target_compile_defs)
        endif()
    endif()

    # get build flags
    #get_directory_property(aux COMPILE_DEFINITIONS)
    set(aux)

    # copy target defs that are not in dir defs
    foreach(target_compile_opt ${target_compile_defs})
        if (NOT "${target_compile_opt}" IN_LIST aux)
            list(APPEND aux ${target_compile_opt})
        endif()
    endforeach()


    foreach(define ${aux})
        if(NOT "${define}" STREQUAL "NDEBUG")
            set(cmake_build_flags "${cmake_build_flags}#ifndef ${define}\n    #define ${define}\n#endif\n")
        endif()
    endforeach()

    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/${inputfile}"
        "${CMAKE_CURRENT_SOURCE_DIR}/${outputfile}"
        @ONLY
    )
endmacro()

set(DEFS)

# configure compile vars
if(WIN32)
    #
    # Windows
    #
    if(CMAKE_COMPILER_IS_GNUCXX)
        create_missing_cmake_build_type()
        if (CMAKE_BUILD_TYPE STREQUAL Release)
            #add_definitions(-DNDEBUG)
            list(APPEND DEFS NDEBUG)
            #add_compile_options(-O3 -fno-strict-aliasing)
        endif()
    elseif(MSVC)
        # change the multithread debug DLL to multithread debug static compilation
        # avoid link with another VS DLL...
        foreach(flag
                CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO

                CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
                CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO )
            if(${flag} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
                SET ( ${flag} "${${flag}}" CACHE STRING "" FORCE)
            endif()
            set("${flag}" "${${flag}} /MP")
        endforeach()

        #SET(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} /O2")
        #SET(CMAKE_C_FLAGS_RELEASE  "${CMAKE_C_FLAGS_RELEASE} /O2")
    endif()
elseif(APPLE)
    #
    # MacOS X, iOS, watchOS, tvOS (since 3.10.3)
    #
    if (CMAKE_BUILD_TYPE STREQUAL Release)
        #add_definitions(-DNDEBUG)
        list(APPEND DEFS NDEBUG)
        #add_compile_options(-O3 -fno-strict-aliasing)
    endif()
    #SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -all_load")
    #SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -v")
    SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -ObjC")
elseif(UNIX)
    #
    # Linux, BSD, Solaris, Minix
    #
    if (CMAKE_BUILD_TYPE STREQUAL Release)
        #add_definitions(-DNDEBUG)
        list(APPEND DEFS NDEBUG)
        #add_compile_options(-O3 -fno-strict-aliasing)
    elseif( CMAKE_BUILD_TYPE STREQUAL Debug)
        #add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -static-libasan")
    endif()
else()
    message(FATAL_ERROR "Cmake not configured for this OS")
endif()
############################################################################
# Detect Raspberry Pi
############################################################################
if(UNIX AND EXISTS "/opt/vc/include/bcm_host.h" AND ${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
    set(ITK_RPI true CACHE BOOL "Set this if you are buinding for Raspberry PI" )
    #add_definitions(-DITK_RPI)
    list(APPEND DEFS ITK_RPI)
    # RPI OMX Compilation Flag on 32bit: -DOMX_SKIP64BIT
    if( CMAKE_SIZEOF_VOID_P EQUAL 4 )
        #add_definitions(-DOMX_SKIP64BIT)
        list(APPEND DEFS OMX_SKIP64BIT)
    endif()
    link_directories(/opt/vc/lib/)
    include_directories(
        /opt/vc/include/interface/vcos/pthreads
        /opt/vc/include/interface/vmcs_host/linux
        /opt/vc/include
    )
    set(EGL_INCLUDE_DIR /opt/vc/include)
    set(EGL_LIBRARY /opt/vc/lib/libbrcmEGL.so)
    set(GLES_INCLUDE_DIR /opt/vc/include)
    set(GLES_LIBRARY /opt/vc/lib/libbrcmGLESv2.so)
    set(RPI_LIBRARIES bcm_host openmaxil vcos vchiq_arm dl)
    message(STATUS "[RPI Detector] Defined Libraries Variables: " )
    message(STATUS "    RPI_LIBRARIES")
    message(STATUS "    GLES_LIBRARY")
    message(STATUS "    EGL_LIBRARY")
endif()
############################################################################
# Detect NEON compatible environment
############################################################################
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
    set(ITK_NEON ON CACHE BOOL "Turn on NEON optimizations." )
    if (ITK_NEON)
		#add_definitions(-DITK_NEON)
        list(APPEND DEFS ITK_NEON)
        #add_compile_options(-mfpu=neon -finline-functions)
        message(STATUS "[NEON ENABLED]")
    endif()
endif()
############################################################################
# Detect OpenMP compatible environment
############################################################################
if(NOT CMAKE_GENERATOR STREQUAL Xcode)
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set(ITK_OPENMP ON CACHE BOOL "Turn on OPENMP optimizations." )
        if (ITK_OPENMP)
            #add_definitions(-DITK_OPENMP)
            list(APPEND DEFS ITK_OPENMP)
            include_directories(${OpenMP_C_INCLUDE_DIRS} ${OpenMP_CXX_INCLUDE_DIRS})
            set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
            set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
            set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
        endif()
    endif()
endif()
############################################################################
# Detect SSE2 compatible environment
############################################################################
if(NOT ${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
    set(ITK_SSE2 ON CACHE BOOL "Turn on SSE2 optimizations.")
    if (ITK_SSE2)
		if(MSVC)
			#message(STATUS "DETECT_SSE2: You are building the code for Visual Studio.")
			#message(STATUS "             the containers (vector, set, map, etc...)")
			#message(STATUS "             need to be aligned with ssealign<T,16>")
			#add_definitions(-DITK_SSE2) # /arch:SSE2 /Gz __stdcall
            list(APPEND DEFS ITK_SSE2)
		else()
			#add_definitions(-DITK_SSE2)
            list(APPEND DEFS ITK_SSE2)
			#add_compile_options(-mmmx -msse -msse2 -msse3 -msse4.1 -mfpmath=sse -minline-all-stringops -finline-functions)
		endif()
    endif()
endif()


############################################################################
# Print Result
############################################################################
message(STATUS "")
message(STATUS "All Detection Set:")
message(STATUS "")
message(STATUS "[ITK_RPI    ${ITK_RPI}]")
message(STATUS "[ITK_NEON   ${ITK_NEON}]")
message(STATUS "[ITK_OPENMP ${ITK_OPENMP}]")
message(STATUS "[ITK_SSE2   ${ITK_SSE2}]")
message(STATUS "")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
# export include dirs
set(INTERACTIVETOOLKIT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE STRING "${PROJECT_NAME}: Include Directories" FORCE)

FILE( GLOB_RECURSE PUBLIC_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h)
FILE( GLOB_RECURSE PUBLIC_INL RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.inl)
FILE( GLOB_RECURSE SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.cpp)

define_source_group(${PUBLIC_HEADERS} ${PUBLIC_INL} ${SRC})

add_library(${PROJECT_NAME} INTERFACE ${PUBLIC_HEADERS} ${SRC})

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "LIB")

set_target_properties(${PROJECT_NAME}
                      PROPERTIES COMPILE_DEFINITIONS
                      "${DEFS}")

#
# Generate common link libraries...
#
set(TARGET_LIBS "")
if (ITK_RPI)
    list(APPEND TARGET_LIBS ${RPI_LIBRARIES} )
endif()
if(WIN32)
    #
    # Windows
    #
    list(APPEND TARGET_LIBS
                shlwapi iphlpapi #path operations
                winmm #multimedia calls
                ws2_32 #WinSock2
    )
elseif(APPLE)
    #
    # MacOS X, iOS, watchOS, tvOS (since 3.10.3)
    #
elseif(UNIX)
    #
    # Linux, BSD, Solaris, Minix
    #
    list(APPEND TARGET_LIBS pthread rt )
else()
    message(FATAL_ERROR "Platform not tested error")
endif()

set(INTERACTIVETOOLKIT_LIBRARIES ${TARGET_LIBS} CACHE STRING "${PROJECT_NAME}: Link Libraries" FORCE)

target_link_libraries(${PROJECT_NAME} INTERFACE ${TARGET_LIBS})

# generate buildFlags.h
configure_build_flags( ${PROJECT_NAME} "include/InteractiveToolkit/buildFlags.h.in" "include/InteractiveToolkit/buildFlags.h")

option(ITK_SKIP_INSTALL OFF)

if( NOT MSVC AND NOT ITK_SKIP_INSTALL )
    
    # Install setup
    # install(
    #     TARGETS ${PROJECT_NAME}
    #     ARCHIVE DESTINATION lib
    # )


    foreach(file ${PUBLIC_HEADERS} ${PUBLIC_INL})
        get_filename_component(DST_DIR ${file} DIRECTORY)
        install(
            FILES ${file}
            DESTINATION ${DST_DIR}
        )
    endforeach()

    install(
        FILES "LICENSE"
        DESTINATION include/${PROJECT_NAME}
    )

    install(
        FILES "${CMAKE_CURRENT_LIST_DIR}/cmake-modules/InteractiveToolkitConfig.cmake"
        DESTINATION lib/${PROJECT_NAME}
    )
endif()

get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
    set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_LIST_DIR}/cmake-modules" PARENT_SCOPE)
endif()
