cmake_minimum_required (VERSION 3.10)

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

include(CheckCXXCompilerFlag)

if(MSVC)
    #disable size_t warning conversion... on 64build
    #add_compile_options( /wd4267 )
endif()

# create CMAKE_BUILD_TYPE for non-windows platform
# if( NOT WIN32 AND NOT CMAKE_BUILD_TYPE )
#     set( CMAKE_BUILD_TYPE Release CACHE STRING
#             "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
#             FORCE )
#     set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS None Debug Release RelWithDebInfo MinSizeRel)
# endif()

#
# MACROS
#

macro(unix_check_cpu_flag flag var )
if(WIN32)
    # execute_process(
    #     COMMAND coreinfo.exe -v
    #     RESULT_VARIABLE coreinfo_result
    #     OUTPUT_VARIABLE coreinfo_output
    #     ERROR_QUIET
    # )

    set(COMPILER_SUPPORTS_REQUIRED_INTRINSICS OFF)
    if (${flag} STREQUAL "neon")
        if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
            set(COMPILER_SUPPORTS_REQUIRED_INTRINSICS ON)
        else()
            set(COMPILER_SUPPORTS_REQUIRED_INTRINSICS OFF)
        endif()
    elseif(${flag} STREQUAL "sse3")
        if(NOT ${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
            include(CheckCXXSourceRuns)
            if (MSVC)
                set(CMAKE_REQUIRED_FLAGS "/arch:SSE2")
            else()
                set(CMAKE_REQUIRED_FLAGS "-msse3")
            endif()
            set(SSE3_TEST_CODE "
                #include <pmmintrin.h>
                int main() {
                    __m128 a = _mm_set1_ps(1.0); // SSE3 intrinsic
                    __m128 b = _mm_set1_ps(2.0); // SSE3 intrinsic
                    __m128 c = _mm_addsub_ps(a, b); // SSE3 intrinsic
                    return 0;
                }"
            )
            check_cxx_source_runs("${SSE3_TEST_CODE}" SSE3_SUPPORTED)
            unset(CMAKE_REQUIRED_FLAGS)
            if (SSE3_SUPPORTED)
                set(COMPILER_SUPPORTS_REQUIRED_INTRINSICS ON)
            else()
                set(COMPILER_SUPPORTS_REQUIRED_INTRINSICS OFF)
            endif()
        else()
            set(COMPILER_SUPPORTS_REQUIRED_INTRINSICS OFF)
        endif()
    elseif(${flag} STREQUAL "sse4_1")
        if(NOT ${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
            include(CheckCXXSourceRuns)
            if (MSVC)
                set(CMAKE_REQUIRED_FLAGS "/arch:SSE2")
            else()
                set(CMAKE_REQUIRED_FLAGS "-msse4.1")
            endif()
            set(SSE41_TEST_CODE "
                #include <smmintrin.h>
                int main() {
                    __m128i a = _mm_set1_epi32(1); // SSE4.1 intrinsic
                    __m128i b = _mm_set1_epi32(2); // SSE4.1 intrinsic
                    __m128i c = _mm_blend_epi16(a, b, 0xF0); // SSE4.1 intrinsic
                    return 0;
                }"
            )
            check_cxx_source_runs("${SSE41_TEST_CODE}" SSE41_SUPPORTED)
            unset(CMAKE_REQUIRED_FLAGS)
            if (SSE41_SUPPORTED)
                set(COMPILER_SUPPORTS_REQUIRED_INTRINSICS ON)
            else()
                set(COMPILER_SUPPORTS_REQUIRED_INTRINSICS OFF)
            endif()
        else()
            set(COMPILER_SUPPORTS_REQUIRED_INTRINSICS OFF)
        endif()
    elseif(${flag} STREQUAL "avx2")
        if(NOT ${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")# AND CMAKE_SIZEOF_VOID_P EQUAL 8)
            include(CheckCXXSourceRuns)
            if (MSVC)
                set(CMAKE_REQUIRED_FLAGS "/arch:AVX2")
            else()
                set(CMAKE_REQUIRED_FLAGS "-mavx2")
            endif()
            set(AVX2_TEST_CODE "
                #include <immintrin.h>
                int main() {
                    __m256i vec = _mm256_set1_epi32(0);
                    return 0;
                }"
            )
            check_cxx_source_runs("${AVX2_TEST_CODE}" AVX2_SUPPORTED)
            unset(CMAKE_REQUIRED_FLAGS)
            if (AVX2_SUPPORTED)
                set(COMPILER_SUPPORTS_REQUIRED_INTRINSICS ON)
            else()
                set(COMPILER_SUPPORTS_REQUIRED_INTRINSICS OFF)
            endif()
        else()
            set(COMPILER_SUPPORTS_REQUIRED_INTRINSICS OFF)
        endif()
    endif()

    if (COMPILER_SUPPORTS_REQUIRED_INTRINSICS)
        set(${var} ON)
    else()
        set(${var} OFF)
    endif()
    
    # if(coreinfo_result EQUAL 0 AND coreinfo_output MATCHES "${to_test}")
    #     set(${var} ON)
    # else()
    #     set(${var} OFF)
    # endif()
elseif(APPLE)
    if (${flag} STREQUAL "neon")
        # on arm systems it always supports neon
        if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
            set(${var} ON)
        else()
            set(${var} OFF)
        endif()
    else()
        if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
            set(${var} OFF)
        else()
            set(to_test "EMPTY_CPU_FLAG")
            if(${flag} STREQUAL "sse3")
                set(to_test "SSE 3")
            elseif(${flag} STREQUAL "sse4_1")
                set(to_test "SSE 4.1")
            elseif(${flag} STREQUAL "avx2")
                set(to_test "AVX2")
            endif()
            execute_process(
                COMMAND sysctl -a
                OUTPUT_VARIABLE sysctl_output
            )
            if(sysctl_output MATCHES "${to_test}")
                set(${var} ON)
            else()
                set(${var} OFF)
            endif()
        endif()
    endif()
elseif(UNIX)
    # aarch64 always has neon
    execute_process(
        COMMAND uname -i
        OUTPUT_VARIABLE UNAME_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if ( ${flag} STREQUAL "neon" AND 
        ( ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64" OR 
            ( ${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm" AND ${UNAME_OUTPUT} MATCHES "aarch64" )
        ) )
        set(${var} ON)
    else()
        execute_process(
            COMMAND bash -c "grep -m1 '${flag}' /proc/cpuinfo"
            RESULT_VARIABLE grep_result
            OUTPUT_VARIABLE grep_output
            ERROR_QUIET
        )
        if(grep_result EQUAL 0)
            set(${var} ON)
        else()
            set(${var} OFF)
        endif()
    endif()
endif()
endmacro()

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

macro(to_unique_list output)
    set(${output})
    foreach(var IN ITEMS ${ARGN})
        if (NOT "${var}" IN_LIST ${output})
            list(APPEND ${output} ${var})
        endif()
    endforeach()
endmacro()

macro(configure_build_flags projectname inputfile outputfile)

    #output
    set(configure_COMPILE_DEFINITIONS "") # generate buildFlags.h
    set(configure_COMPILE_OPTIONS "") # generate opts.cmake
    set(configure_LINK_LIBRARIES "") # generate opts.cmake
    set(configure_LINK_OPTIONS "") # generate opts.cmake
    set(configure_INCLUDE_DIRECTORIES "") # generate opts.cmake
    set(configure_LIB_CMAKE_FLAGS "") # generate opts.cmake

    set(target_COMPILE_DEFINITIONS)
    set(target_COMPILE_OPTIONS)
    set(target_LINK_LIBRARIES)
    set(target_LINK_OPTIONS)
    set(target_INCLUDE_DIRECTORIES)

    if (TARGET ${projectname})
        get_target_property(target_COMPILE_DEFINITIONS ${projectname} COMPILE_DEFINITIONS)
        if ("${target_COMPILE_DEFINITIONS}" STREQUAL "target_COMPILE_DEFINITIONS-NOTFOUND")
            set(target_COMPILE_DEFINITIONS)
        endif()

        get_target_property(target_COMPILE_OPTIONS ${projectname} COMPILE_OPTIONS)
        if ("${target_COMPILE_OPTIONS}" STREQUAL "target_COMPILE_OPTIONS-NOTFOUND")
            set(target_COMPILE_OPTIONS)
        endif()

        get_target_property(target_LINK_LIBRARIES ${projectname} INTERFACE_LINK_LIBRARIES)
        if ("${target_LINK_LIBRARIES}" STREQUAL "target_LINK_LIBRARIES-NOTFOUND")
            set(target_LINK_LIBRARIES)
        endif()

        get_target_property(target_LINK_OPTIONS ${projectname} INTERFACE_LINK_OPTIONS)
        if ("${target_LINK_OPTIONS}" STREQUAL "target_LINK_OPTIONS-NOTFOUND")
            set(target_LINK_OPTIONS)
        endif()

        get_target_property(target_INCLUDE_DIRECTORIES ${projectname} INTERFACE_INCLUDE_DIRECTORIES)
        if ("${target_INCLUDE_DIRECTORIES}" STREQUAL "target_INCLUDE_DIRECTORIES-NOTFOUND")
            set(target_INCLUDE_DIRECTORIES)
        endif()

    endif()

    # get build flags
    #get_directory_property(aux COMPILE_DEFINITIONS)
    #
    # configure_COMPILE_DEFINITIONS
    #
    to_unique_list(aux ${target_COMPILE_DEFINITIONS})
    foreach(var ${aux})
        if("${var}" STREQUAL "ITK_SSE2")
            set(configure_COMPILE_DEFINITIONS "${configure_COMPILE_DEFINITIONS}#if defined(__x86_64__) || defined(_M_X64) || defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)\n")
            set(configure_COMPILE_DEFINITIONS "${configure_COMPILE_DEFINITIONS}#    ifndef ${var}\n#        define ${var}\n#    endif\n")
            set(configure_COMPILE_DEFINITIONS "${configure_COMPILE_DEFINITIONS}#endif\n")
        elseif("${var}" STREQUAL "ITK_AVX2")
            set(configure_COMPILE_DEFINITIONS "${configure_COMPILE_DEFINITIONS}#if defined(__x86_64__) || defined(_M_X64) || defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)\n")
            set(configure_COMPILE_DEFINITIONS "${configure_COMPILE_DEFINITIONS}#    ifndef ${var}\n#        define ${var}\n#    endif\n")
            set(configure_COMPILE_DEFINITIONS "${configure_COMPILE_DEFINITIONS}#endif\n")
        elseif("${var}" STREQUAL "ITK_NEON")
            set(configure_COMPILE_DEFINITIONS "${configure_COMPILE_DEFINITIONS}#if defined(__arm__) || defined(_M_ARM) || defined(__aarch64__)\n")
            set(configure_COMPILE_DEFINITIONS "${configure_COMPILE_DEFINITIONS}#    ifndef ${var}\n#        define ${var}\n#    endif\n")
            set(configure_COMPILE_DEFINITIONS "${configure_COMPILE_DEFINITIONS}#endif\n")
        elseif(NOT "${var}" STREQUAL "NDEBUG")
            set(configure_COMPILE_DEFINITIONS "${configure_COMPILE_DEFINITIONS}#ifndef ${var}\n#    define ${var}\n#endif\n")
        endif()
    endforeach()

    #unset(INTERACTIVETOOLKIT_INCLUDE_DIRS)

    #
    # COMPILE_OPTIONS
    #
    to_unique_list(aux ${target_COMPILE_OPTIONS})
    foreach(var ${aux})
        set(configure_COMPILE_OPTIONS "${configure_COMPILE_OPTIONS}    list(APPEND INTERACTIVETOOLKIT_COMPILE_OPTIONS \"${var}\")\n")
    endforeach()

    #
    # LINK_LIBRARIES
    #
    to_unique_list(aux ${target_LINK_LIBRARIES})
    foreach(var ${aux})
        set(configure_LINK_LIBRARIES "${configure_LINK_LIBRARIES}    list(APPEND INTERACTIVETOOLKIT_LIBRARIES \"${var}\")\n")
    endforeach()

    #
    # LINK_OPTIONS
    #
    to_unique_list(aux ${target_LINK_OPTIONS})
    foreach(var ${aux})
        set(configure_LINK_OPTIONS "${configure_LINK_OPTIONS}    list(APPEND INTERACTIVETOOLKIT_LINK_OPTIONS \"${var}\")\n")
    endforeach()

    #
    # INCLUDE_DIRECTORIES
    #
    to_unique_list(aux ${target_INCLUDE_DIRECTORIES})
    foreach(var ${aux})
        if (var STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}/include")
            continue()
        endif()
        set(configure_INCLUDE_DIRECTORIES "${configure_INCLUDE_DIRECTORIES}    list(APPEND INTERACTIVETOOLKIT_INCLUDE_DIRS \"${var}\")\n")
    endforeach()

    #
    # CMAKE LIB_FLAGS
    #
    set(configure_LIB_CMAKE_FLAGS "${configure_LIB_CMAKE_FLAGS}    set(ITK_RPI ${ITK_RPI})\n")
    set(configure_LIB_CMAKE_FLAGS "${configure_LIB_CMAKE_FLAGS}    set(ITK_NEON ${ITK_NEON})\n")
    set(configure_LIB_CMAKE_FLAGS "${configure_LIB_CMAKE_FLAGS}    set(ITK_OPENMP ${ITK_OPENMP})\n")
    set(configure_LIB_CMAKE_FLAGS "${configure_LIB_CMAKE_FLAGS}    set(ITK_SSE2 ${ITK_SSE2})\n")
    set(configure_LIB_CMAKE_FLAGS "${configure_LIB_CMAKE_FLAGS}    set(ITK_AVX2 ${ITK_AVX2})\n")
    set(configure_LIB_CMAKE_FLAGS "${configure_LIB_CMAKE_FLAGS}    set(ITK_SSE_SKIP_SSE41 ${ITK_SSE_SKIP_SSE41})\n")
    set(configure_LIB_CMAKE_FLAGS "${configure_LIB_CMAKE_FLAGS}    set(ITK_FORCE_USE_RSQRT_CARMACK ${ITK_FORCE_USE_RSQRT_CARMACK})\n")
    
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/${inputfile}"
        "${CMAKE_CURRENT_SOURCE_DIR}/${outputfile}"
        @ONLY
    )
endmacro()

#
# Auxiliary vars
#

set(ITK_COMPILE_DEFINITIONS)
set(ITK_COMPILE_OPTIONS)
set(ITK_LINK_OPTIONS)
set(ITK_LINK_LIBRARIES)
set(ITK_LINK_LIBRARIES)
set(ITK_INCLUDE_DIRECTORIES)


# # configure compile vars
# if(WIN32)
#     #
#     # Windows
#     #
#     if(CMAKE_COMPILER_IS_GNUCXX)
#         create_missing_cmake_build_type()
#         if (CMAKE_BUILD_TYPE STREQUAL Release)
#             #add_definitions(-DNDEBUG)
#             list(APPEND ITK_COMPILE_DEFINITIONS NDEBUG)
#             #add_compile_options(-O3 -fno-strict-aliasing)
#         endif()
#     elseif(MSVC)
#         # change the multithread debug DLL to multithread debug static compilation
#         # avoid link with another VS DLL...
#         foreach(flag
#                 CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
#                 CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO

#                 CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
#                 CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO )
#             if(${flag} MATCHES "/MD")
#                 string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
#                 SET ( ${flag} "${${flag}}" CACHE STRING "" FORCE)
#             endif()
#             set("${flag}" "${${flag}} /MP")
#         endforeach()

#         #SET(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} /O2")
#         #SET(CMAKE_C_FLAGS_RELEASE  "${CMAKE_C_FLAGS_RELEASE} /O2")
#     endif()
# elseif(APPLE)
#     #
#     # MacOS X, iOS, watchOS, tvOS (since 3.10.3)
#     #
#     if (CMAKE_BUILD_TYPE STREQUAL Release)
#         #add_definitions(-DNDEBUG)
#         list(APPEND ITK_COMPILE_DEFINITIONS NDEBUG)
#         #add_compile_options(-O3 -fno-strict-aliasing)
#     endif()
#     #SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -all_load")
#     #SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -v")
#     #SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -ObjC")
#     list(APPEND ITK_LINK_OPTIONS -ObjC)
# elseif(UNIX)
#     #
#     # Linux, BSD, Solaris, Minix
#     #
#     if (CMAKE_BUILD_TYPE STREQUAL Release)
#         #add_definitions(-DNDEBUG)
#         list(APPEND ITK_COMPILE_DEFINITIONS NDEBUG)
#         #add_compile_options(-O3 -fno-strict-aliasing)
#     elseif( CMAKE_BUILD_TYPE STREQUAL Debug)
#         #add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
#         #SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -static-libasan")
#     endif()
# else()
#     message(FATAL_ERROR "Cmake not configured for this OS")
# endif()
############################################################################
# Detect Raspberry Pi
############################################################################
if(UNIX AND EXISTS "/opt/vc/include/bcm_host.h" AND
    (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm" OR
    ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64"))
    set(ITK_RPI ON CACHE BOOL "Set this if you are buinding for Raspberry PI" )
else()
    set(ITK_RPI OFF CACHE BOOL "Set this if you are buinding for Raspberry PI" )
endif()
if (ITK_RPI)
    #add_definitions(-DITK_RPI)
    list(APPEND ITK_COMPILE_DEFINITIONS ITK_RPI)
    # RPI OMX Compilation Flag on 32bit: -DOMX_SKIP64BIT
    if( CMAKE_SIZEOF_VOID_P EQUAL 4 )
        #add_definitions(-DOMX_SKIP64BIT)
        list(APPEND ITK_COMPILE_DEFINITIONS OMX_SKIP64BIT)
    endif()
    # link_directories(/opt/vc/lib/)
    # include_directories(
    #     /opt/vc/include/interface/vcos/pthreads
    #     /opt/vc/include/interface/vmcs_host/linux
    #     /opt/vc/include
    # )
    list(APPEND ITK_INCLUDE_DIRECTORIES
        /opt/vc/include/interface/vcos/pthreads
        /opt/vc/include/interface/vmcs_host/linux
        /opt/vc/include)
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
# check_cxx_compiler_flag("-mfpu=neon" COMPILER_SUPPORTS_NEON)

unix_check_cpu_flag("neon" COMPILER_SUPPORTS_NEON)

#if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
if(COMPILER_SUPPORTS_NEON)
    set(ITK_NEON ON CACHE BOOL "Turn on NEON optimizations." )
else()
    set(ITK_NEON OFF CACHE BOOL "Turn on NEON optimizations." )
endif()
if (ITK_NEON)
    #add_definitions(-DITK_NEON)
    list(APPEND ITK_COMPILE_DEFINITIONS ITK_NEON)
    #add_compile_options(-mfpu=neon -finline-functions)
#    if(NOT CMAKE_GENERATOR STREQUAL Xcode)
#        list(APPEND ITK_COMPILE_OPTIONS -mfloat-abi=hard -mfpu=neon -finline-functions)
#    else()
#        list(APPEND ITK_COMPILE_OPTIONS -finline-functions)
#    endif()

    # aarch64 includes neon automatically
    if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
        list(APPEND ITK_COMPILE_OPTIONS -finline-functions)
    else()
        list(APPEND ITK_COMPILE_OPTIONS -mfloat-abi=hard -mfpu=neon -finline-functions)
    endif()

    #message(STATUS "[NEON ENABLED]")
endif()
############################################################################
# Detect OpenMP compatible environment
############################################################################
if(NOT CMAKE_GENERATOR STREQUAL Xcode)
    find_package(OpenMP QUIET)
    if (OPENMP_FOUND)
        set(ITK_OPENMP ON CACHE BOOL "Turn on OPENMP optimizations." )
    else()
        set(ITK_OPENMP OFF CACHE BOOL "Turn on OPENMP optimizations." )
    endif()
else()
    set(ITK_OPENMP OFF CACHE BOOL "Turn on OPENMP optimizations." )
endif()
if (ITK_OPENMP)
    if(CMAKE_GENERATOR STREQUAL Xcode)
        message(FATAL_ERROR "CANNOT ENABLE OPENMP WITH XCODE BUILD.")
    endif()
    find_package(OpenMP REQUIRED)
    #add_definitions(-DITK_OPENMP)
    list(APPEND ITK_COMPILE_DEFINITIONS ITK_OPENMP)
    # include_directories(${OpenMP_C_INCLUDE_DIRS} ${OpenMP_CXX_INCLUDE_DIRS})
    list(APPEND ITK_INCLUDE_DIRECTORIES ${OpenMP_C_INCLUDE_DIRS} ${OpenMP_CXX_INCLUDE_DIRS})
    # set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    # set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    list(APPEND ITK_COMPILE_OPTIONS "${OpenMP_CXX_FLAGS}")
    # set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    list(APPEND ITK_LINK_OPTIONS "${OpenMP_EXE_LINKER_FLAGS}")
    list(APPEND ITK_LINK_LIBRARIES "${OpenMP_CXX_LIBRARIES}")
endif()
############################################################################
# Detect SSE2 compatible environment
############################################################################
# check_cxx_compiler_flag("-msse3" COMPILER_SUPPORTS_SSE3)
# check_cxx_compiler_flag("-msse4.1" COMPILER_SUPPORTS_SSE41)
# check_cxx_compiler_flag("-mavx2" COMPILER_SUPPORTS_AVX2)

unix_check_cpu_flag("sse3" COMPILER_SUPPORTS_SSE3)
unix_check_cpu_flag("sse4_1" COMPILER_SUPPORTS_SSE41)
unix_check_cpu_flag("avx2" COMPILER_SUPPORTS_AVX2)

#if(NOT ${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
if(COMPILER_SUPPORTS_SSE41)
    set(ITK_SSE_SKIP_SSE41 OFF CACHE BOOL "Skip SSE41 on some special CPUs.")
else()
    set(ITK_SSE_SKIP_SSE41 ON CACHE BOOL "Skip SSE41 on some special CPUs.")
endif()
if(COMPILER_SUPPORTS_SSE3)
    set(ITK_SSE2 ON CACHE BOOL "Turn on SSE2 optimizations.")
else()
    set(ITK_SSE2 OFF CACHE BOOL "Turn on SSE2 optimizations.")
endif()
if(COMPILER_SUPPORTS_AVX2)
    set(ITK_AVX2 ON CACHE BOOL "Turn on AVX2 optimizations (requires SSE2 to work).")
else()
    set(ITK_AVX2 OFF CACHE BOOL "Turn on AVX2 optimizations (requires SSE2 to work).")
endif()

if (ITK_SSE2)
    if (ITK_SSE_SKIP_SSE41)
        list(APPEND ITK_COMPILE_DEFINITIONS ITK_SSE_SKIP_SSE41)
    endif()
    #add_definitions(-DITK_SSE2)
    list(APPEND ITK_COMPILE_DEFINITIONS ITK_SSE2)
    if (ITK_AVX2)
        list(APPEND ITK_COMPILE_DEFINITIONS ITK_AVX2)
    endif()
    if(NOT MSVC)
        list(APPEND ITK_COMPILE_OPTIONS -mmmx -msse -msse2 -msse3)
        if (NOT ITK_SSE_SKIP_SSE41)
            list(APPEND ITK_COMPILE_OPTIONS -msse4.1)
        endif()
        if (ITK_AVX2)
            list(APPEND ITK_COMPILE_OPTIONS -mavx -mavx2)
        endif()
        list(APPEND ITK_COMPILE_OPTIONS -mfpmath=sse -minline-all-stringops -finline-functions)
    else()
        #visual studio
        if (ITK_AVX2)
            list(APPEND ITK_COMPILE_OPTIONS /arch:AVX2)
        elseif(ITK_SSE2)
            list(APPEND ITK_COMPILE_OPTIONS /arch:SSE2)
        endif()
    endif()
endif()
############################################################################
# Turn on RSQRT algorithm
############################################################################
if (ITK_SSE2)# OR (ITK_NEON AND CMAKE_SIZEOF_VOID_P EQUAL 4 ) )
    set(ITK_FORCE_USE_RSQRT_CARMACK OFF CACHE BOOL "Force use of the alternative RSQRT algorithm.")
else()
    set(ITK_FORCE_USE_RSQRT_CARMACK ON CACHE BOOL "Force use of the alternative RSQRT algorithm.")
endif()
if (ITK_FORCE_USE_RSQRT_CARMACK)
    list(APPEND ITK_COMPILE_DEFINITIONS ITK_FORCE_USE_RSQRT_CARMACK)
endif()
############################################################################
# Select trigonometric optimization
############################################################################
if( NOT ITK_TRIGONOMETRIC )
    set( ITK_TRIGONOMETRIC FAST_LESS_MEMORY CACHE STRING
                "Choose the type of trigonometric optimizations."
                FORCE )
endif()
set_property(CACHE ITK_TRIGONOMETRIC PROPERTY STRINGS FASTEST_MORE_MEMORY FAST_LESS_MEMORY STANDARD_LIBRARY)
if(ITK_TRIGONOMETRIC STREQUAL FASTEST_MORE_MEMORY)
    list(APPEND ITK_COMPILE_DEFINITIONS ITK_TRIGONOMETRIC_FASTEST_MORE_MEMORY)
elseif(ITK_TRIGONOMETRIC STREQUAL FAST_LESS_MEMORY)
    list(APPEND ITK_COMPILE_DEFINITIONS ITK_TRIGONOMETRIC_FAST_LESS_MEMORY)
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
message(STATUS "[ITK_AVX2   ${ITK_AVX2}]")
message(STATUS "[ITK_SSE_SKIP_SSE41   ${ITK_SSE_SKIP_SSE41}]")
message(STATUS "[ITK_FORCE_USE_RSQRT_CARMACK   ${ITK_FORCE_USE_RSQRT_CARMACK}]")
message(STATUS "")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#
# Generate common link libraries...
#
if (ITK_RPI)
    list(APPEND ITK_LINK_LIBRARIES ${RPI_LIBRARIES} )
    list(APPEND ITK_LINK_OPTIONS -L/opt/vc/lib/)
endif()
if(WIN32)
    #
    # Windows
    #
    list(APPEND ITK_LINK_LIBRARIES
                shlwapi iphlpapi #path operations
                winmm #multimedia calls
                ws2_32 #WinSock2
    )
elseif(APPLE)
    #
    # MacOS X, iOS, watchOS, tvOS (since 3.10.3)
    #
    list(APPEND ITK_LINK_OPTIONS -ObjC)
elseif(UNIX)
    #
    # Linux, BSD, Solaris, Minix
    #
    list(APPEND ITK_LINK_LIBRARIES pthread rt )
else()
    message(FATAL_ERROR "Platform not tested error")
endif()

#
# Create Library
#

# create/export include dirs/libs
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
#set(INTERACTIVETOOLKIT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE STRING "${PROJECT_NAME}: Include Directories" FORCE)
#set(INTERACTIVETOOLKIT_LIBRARIES ${ITK_LINK_LIBRARIES} CACHE STRING "${PROJECT_NAME}: Link Libraries" FORCE)

FILE( GLOB_RECURSE PUBLIC_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h)
FILE( GLOB_RECURSE PUBLIC_INL RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.inl)
FILE( GLOB_RECURSE SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.cpp)

define_source_group(${PUBLIC_HEADERS} ${PUBLIC_INL} ${SRC})

#add_library(${PROJECT_NAME} INTERFACE)
#target_sources(${PROJECT_NAME} INTERFACE ${PUBLIC_HEADERS} ${SRC})
#add_custom_target(${PROJECT_NAME} SOURCES ${PUBLIC_HEADERS} ${PUBLIC_INL} ${SRC})
add_library(${PROJECT_NAME} INTERFACE ${PUBLIC_HEADERS} ${PUBLIC_INL} ${SRC})

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "LIBS")

set_property(TARGET ${PROJECT_NAME}
            PROPERTY COMPILE_DEFINITIONS
            ${ITK_COMPILE_DEFINITIONS})

set_property(TARGET ${PROJECT_NAME}
            PROPERTY COMPILE_OPTIONS
            ${ITK_COMPILE_OPTIONS})

set_property(TARGET ${PROJECT_NAME}
            PROPERTY INTERFACE_LINK_OPTIONS
            ${ITK_LINK_OPTIONS})

set_property(TARGET ${PROJECT_NAME}
            PROPERTY INTERFACE_INCLUDE_DIRECTORIES
            ${ITK_INCLUDE_DIRECTORIES})

set_property(TARGET ${PROJECT_NAME}
            PROPERTY  INTERFACE_LINK_LIBRARIES
            ${ITK_LINK_LIBRARIES})

#
# for direct reference to project
#
target_compile_options(${PROJECT_NAME} 
INTERFACE
    ${ITK_COMPILE_OPTIONS}
)

target_link_options(${PROJECT_NAME} 
INTERFACE
    ${ITK_LINK_OPTIONS}
)

target_include_directories(${PROJECT_NAME} 
INTERFACE
    ${ITK_INCLUDE_DIRECTORIES}
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(${PROJECT_NAME} 
INTERFACE 
    ${ITK_LINK_LIBRARIES}
)

# set_target_properties(${PROJECT_NAME}
#                       PROPERTIES COMPILE_DEFINITIONS
#                       "${ITK_COMPILE_DEFINITIONS}")

# set_target_properties(${PROJECT_NAME}
#                       PROPERTIES COMPILE_OPTIONS
#                       "${ITK_COMPILE_OPTIONS}")

# set_target_properties(${PROJECT_NAME}
#                       PROPERTIES INTERFACE_LINK_OPTIONS
#                       "${ITK_LINK_OPTIONS}")

# set_target_properties(${PROJECT_NAME}
#                       PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
#                       "${ITK_INCLUDE_DIRECTORIES}")

# set_target_properties(${PROJECT_NAME}
#                       PROPERTIES INTERFACE_LINK_LIBRARIES
#                       "${ITK_LINK_LIBRARIES}")

#target_link_libraries(${PROJECT_NAME} INTERFACE ${ITK_LINK_LIBRARIES})

# generate buildFlags.h
configure_build_flags( ${PROJECT_NAME} "include/InteractiveToolkit/buildFlags.h.in" "include/InteractiveToolkit/buildFlags.h")
# configure_build_flags( ${PROJECT_NAME} "cmake-modules/InteractiveToolkitConfig.in.cmake" "cmake-modules/InteractiveToolkitConfig.cmake")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules/InteractiveToolkitConfig.cmake" "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules/FindInteractiveToolkit.cmake" COPYONLY)
configure_build_flags( ${PROJECT_NAME} "include/InteractiveToolkit/opts.cmake.in" "include/InteractiveToolkit/opts.cmake")

option(ITK_SKIP_INSTALL OFF)

if( NOT MSVC AND NOT ITK_SKIP_INSTALL )
    
    # Install setup
    # install(
    #     TARGETS ${PROJECT_NAME}
    #     ARCHIVE DESTINATION lib
    # )


    foreach(file ${PUBLIC_HEADERS} ${PUBLIC_INL} "include/InteractiveToolkit/opts.cmake")
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

# Export module path and prefix path
#
# The main app needs to use this pattern 
# to avoid increase the module and prefix path each config time.
# You need to add to your main project:
#
# set (CMAKE_MODULE_PATH "")
# set (CMAKE_PREFIX_PATH "")
# tool_make_global(CMAKE_MODULE_PATH)
# tool_make_global(CMAKE_PREFIX_PATH)
#

macro ( tool_make_global _var )
    set ( ${_var} ${${_var}} CACHE INTERNAL "hide this!" FORCE )
endmacro( )

macro ( tool_append_if_not_exists_and_make_global _var _value )
    foreach(entry IN ITEMS ${${_var}})
        if("${entry}" STREQUAL "${_value}")
            return()
        endif()
    endforeach()
    set(${_var} ${${_var}} "${_value}")
    tool_make_global(${_var})
endmacro( )

tool_append_if_not_exists_and_make_global(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake-modules")
tool_append_if_not_exists_and_make_global(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")

MESSAGE(STATUS "#######################################")
MESSAGE(STATUS "#         InteractiveToolkit          #")
MESSAGE(STATUS "#######################################")
MESSAGE(STATUS "#                                     #")
MESSAGE(STATUS "# You need to add the folowing        #")
MESSAGE(STATUS "# lines to your root 'CMakeLists.txt' #")
MESSAGE(STATUS "# before import this library:         #")
MESSAGE(STATUS "#                                     #")
MESSAGE(STATUS "# unset (CMAKE_MODULE_PATH CACHE)     #")
MESSAGE(STATUS "# unset (CMAKE_PREFIX_PATH CACHE)     #")
MESSAGE(STATUS "#                                     #")
MESSAGE(STATUS "#######################################")
MESSAGE(STATUS "")


# get_directory_property(hasParent PARENT_DIRECTORY)
# if(hasParent)
#     set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake-modules" PARENT_SCOPE)
#     set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/include" PARENT_SCOPE)
# endif()

#find_package (InteractiveToolkit QUIET PATHS ${CMAKE_CURRENT_SOURCE_DIR}/include)
