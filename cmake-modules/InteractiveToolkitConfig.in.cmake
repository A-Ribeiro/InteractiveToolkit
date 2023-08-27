# "For this is how God loved the world:
# he gave his only Son, so that everyone
# who believes in him may not perish
# but may have eternal life."
#
# John 3:16
if (INTERACTIVETOOLKIT_INCLUDE_DIR AND INTERACTIVETOOLKIT_LIBRARIES)
	#return()
	unset(INTERACTIVETOOLKIT_INCLUDE_DIR)
	unset(INTERACTIVETOOLKIT_LIBRARIES)
endif()

find_path(INTERACTIVETOOLKIT_INCLUDE_DIR InteractiveToolkit/InteractiveToolkit.h)

if(INTERACTIVETOOLKIT_INCLUDE_DIR)

	set(TARGET_LIBS "")
	if (UNIX AND EXISTS "/opt/vc/include/bcm_host.h" AND ${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
		link_directories(/opt/vc/lib/)
		#SET(CMAKE_EXE_LINKER_FLAGS  "-L/opt/vc/lib/ ${CMAKE_EXE_LINKER_FLAGS}")

		list(APPEND INTERACTIVETOOLKIT_INCLUDE_DIR 
			/opt/vc/include/interface/vcos/pthreads
			/opt/vc/include/interface/vmcs_host/linux
			/opt/vc/include)
		list(APPEND TARGET_LIBS bcm_host openmaxil vcos vchiq_arm dl )
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

	set(INTERACTIVETOOLKIT_LIBRARIES "${TARGET_LIBS}")
	set(INTERACTIVETOOLKIT_COMPILE_OPTIONS "")

@cmake_compile_opts@

	if (NOT ${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY)
		MESSAGE(STATUS "Created vars: INTERACTIVETOOLKIT_INCLUDE_DIR")
		MESSAGE(STATUS "              INTERACTIVETOOLKIT_LIBRARIES")
		MESSAGE(STATUS "              INTERACTIVETOOLKIT_COMPILE_OPTIONS")

		MESSAGE(STATUS "Found ${CMAKE_FIND_PACKAGE_NAME} include:  ${INTERACTIVETOOLKIT_INCLUDE_DIR}/InteractiveToolkit/InteractiveToolkit.h")
		MESSAGE(STATUS "Found ${CMAKE_FIND_PACKAGE_NAME} libs:  ${INTERACTIVETOOLKIT_LIBRARIES}")
		MESSAGE(STATUS "Found ${CMAKE_FIND_PACKAGE_NAME} compile opts:  ${INTERACTIVETOOLKIT_COMPILE_OPTIONS}")
	endif()
else()
	if(${CMAKE_FIND_PACKAGE_NAME}_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could NOT find ${CMAKE_FIND_PACKAGE_NAME} development files")
	endif()
endif()

