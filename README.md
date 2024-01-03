> For this is how God loved the world:  
he gave his only Son, so that everyone  
who believes in him may not perish  
but may have eternal life.  
  \
John 3:16

This is a header-only modern C++ library to deal with basics on math, collision, event and algorithms.

## The Library

I'm writing this library since 2006.

There were several versions.

The current version is the result of an effort to update the [aRibeiroCore](https://github.com/A-Ribeiro/aRibeiroCore) lib to use modern features of C++.

I implemented the templates and code to be compatible with C++11.

## Examples

There are several examples using this library at: [https://github.com/A-Ribeiro/InteractiveToolkit-Examples](https://github.com/A-Ribeiro/InteractiveToolkit-Examples)

## How to Import the Library?

There are three ways to import the library.

### a) Installing in the /usr/local/

__HTTPS__

```bash
git clone https://github.com/A-Ribeiro/InteractiveToolkit.git
cd InteractiveToolkit
mkdir build
cd build
cmake ..
sudo make install
```

To uninstall:

```bash
cat install_manifest.txt | sudo xargs rm
```

__SSH__

```bash
git clone git@github.com:A-Ribeiro/InteractiveToolkit.git
cd InteractiveToolkit
mkdir build
cd build
cmake ..
sudo make install
```

To uninstall:

```bash
cat install_manifest.txt | sudo xargs rm
```

### b) Importing directly from this github project

__HTTPS__

```bash
mkdir -p libs/InteractiveToolkit
git clone https://github.com/A-Ribeiro/InteractiveToolkit.git libs/InteractiveToolkit
```

In your CMakeLists.txt:

```cmake
# at the beginning of yout root 'CMakeLists.txt':

unset (CMAKE_MODULE_PATH CACHE)
unset (CMAKE_PREFIX_PATH CACHE)

# include the lib:

add_subdirectory(libs/InteractiveToolkit "${CMAKE_BINARY_DIR}/lib/InteractiveToolkit")
```

__SSH__

```bash
mkdir -p libs/InteractiveToolkit
git clone git@github.com:A-Ribeiro/InteractiveToolkit.git libs/InteractiveToolkit
```

In your CMakeLists.txt:

```cmake
# at the beginning of yout root 'CMakeLists.txt':

unset (CMAKE_MODULE_PATH CACHE)
unset (CMAKE_PREFIX_PATH CACHE)

# include the lib:

add_subdirectory(libs/InteractiveToolkit "${CMAKE_BINARY_DIR}/lib/InteractiveToolkit")
```

### c) Importing using submodules

__HTTPS__

```bash
git init
mkdir libs
git submodule add https://github.com/A-Ribeiro/InteractiveToolkit.git libs/InteractiveToolkit
```

In your CMakeLists.txt:

```cmake
# at the beginning of yout root 'CMakeLists.txt':

unset (CMAKE_MODULE_PATH CACHE)
unset (CMAKE_PREFIX_PATH CACHE)

# include the lib:

add_subdirectory(libs/InteractiveToolkit "${CMAKE_BINARY_DIR}/lib/InteractiveToolkit")
```

You can update your submodules with the command:

```bash
git submodule update --remote --merge
```

__SSH__

```bash
git init
mkdir libs
git submodule add git@github.com:A-Ribeiro/InteractiveToolkit.git libs/InteractiveToolkit
```

In your CMakeLists.txt:

```cmake
# at the beginning of yout root 'CMakeLists.txt':

unset (CMAKE_MODULE_PATH CACHE)
unset (CMAKE_PREFIX_PATH CACHE)

# include the lib:

add_subdirectory(libs/InteractiveToolkit "${CMAKE_BINARY_DIR}/lib/InteractiveToolkit")
```

You can update your submodules with the command:

```bash
git submodule update --remote --merge
```

## Using find_package

After installing or add_subdirectory, you will be able to use the find_package script.

Take a look at the example below:

```cmake
find_package(InteractiveToolkit REQUIRED)
include_directories(${INTERACTIVETOOLKIT_INCLUDE_DIRS})
add_compile_options(${INTERACTIVETOOLKIT_COMPILE_OPTIONS})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${INTERACTIVETOOLKIT_LINK_OPTIONS}")

# ...

target_link_libraries(${PROJECT_NAME} ${INTERACTIVETOOLKIT_LIBRARIES})
```

## Authors

***Alessandro Ribeiro da Silva*** obtained his Bachelor's degree in Computer Science from Pontifical Catholic 
University of Minas Gerais and a Master's degree in Computer Science from the Federal University of Minas Gerais, 
in 2005 and 2008 respectively. He taught at PUC and UFMG as a substitute/assistant professor in the courses 
of Digital Arts, Computer Science, Computer Engineering and Digital Games. He have work experience with interactive
software. He worked with OpenGL, post-processing, out-of-core rendering, Unity3D and game consoles. Today 
he work with freelance projects related to Computer Graphics, Virtual Reality, Augmented Reality, WebGL, web server 
and mobile apps (andoid/iOS).

More information on: https://alessandroribeiro.thegeneralsolution.com/en/
