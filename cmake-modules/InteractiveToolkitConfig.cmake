# "For this is how God loved the world:
# he gave his only Son, so that everyone
# who believes in him may not perish
# but may have eternal life."
#
# John 3:16
if (INTERACTIVETOOLKIT_INCLUDE_DIRS)
	unset(INTERACTIVETOOLKIT_INCLUDE_DIRS)
	unset(INTERACTIVETOOLKIT_COMPILE_OPTIONS)
	unset(INTERACTIVETOOLKIT_LIBRARIES)
	unset(INTERACTIVETOOLKIT_LINK_OPTIONS)
endif()

find_path(INTERACTIVETOOLKIT_INCLUDE_DIRS InteractiveToolkit/InteractiveToolkit.h)

if(INTERACTIVETOOLKIT_INCLUDE_DIRS)

	set(INTERACTIVETOOLKIT_COMPILE_OPTIONS "")
	set(INTERACTIVETOOLKIT_LIBRARIES "")
	set(INTERACTIVETOOLKIT_LINK_OPTIONS "")

	include(${INTERACTIVETOOLKIT_INCLUDE_DIRS}/InteractiveToolkit/opts.cmake)

	if (NOT ${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY)
		MESSAGE(STATUS "${CMAKE_FIND_PACKAGE_NAME}:")
		MESSAGE(STATUS "    INTERACTIVETOOLKIT_INCLUDE_DIRS")
		MESSAGE(STATUS "        ${INTERACTIVETOOLKIT_INCLUDE_DIRS}")
		MESSAGE(STATUS "    INTERACTIVETOOLKIT_LIBRARIES")
		MESSAGE(STATUS "        ${INTERACTIVETOOLKIT_LIBRARIES}")
		MESSAGE(STATUS "    INTERACTIVETOOLKIT_COMPILE_OPTIONS")
		MESSAGE(STATUS "        ${INTERACTIVETOOLKIT_COMPILE_OPTIONS}")
		MESSAGE(STATUS "    INTERACTIVETOOLKIT_LINK_OPTIONS")
		MESSAGE(STATUS "        ${INTERACTIVETOOLKIT_LINK_OPTIONS}")

		#MESSAGE(STATUS "Found ${CMAKE_FIND_PACKAGE_NAME} include:  ${INTERACTIVETOOLKIT_INCLUDE_DIR}/InteractiveToolkit/InteractiveToolkit.h")
		#MESSAGE(STATUS "Found ${CMAKE_FIND_PACKAGE_NAME} libs:  ${INTERACTIVETOOLKIT_LIBRARIES}")
		#MESSAGE(STATUS "Found ${CMAKE_FIND_PACKAGE_NAME} compile opts:  ${INTERACTIVETOOLKIT_COMPILE_OPTIONS}")
	endif()
else()
	if(${CMAKE_FIND_PACKAGE_NAME}_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could NOT find ${CMAKE_FIND_PACKAGE_NAME} development files")
	endif()
endif()

