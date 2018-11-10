cmake_minimum_required(VERSION 3.0) # setting this is required

project(gfold_lr_project CXX)            # this sets the project name

SET (CMAKE_C_COMPILER             "/usr/bin/clang")
SET (CMAKE_C_FLAGS                "-Wall -std=c99")
SET (CMAKE_C_FLAGS_DEBUG          "-g")
SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
SET (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++")
SET (CMAKE_CXX_FLAGS                "-Wall")
SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

set(CMAKE_CXX_STANDARD 11)

message(${CMAKE_CXX_COMPILER})

###############################################################################
## file globbing ##############################################################
###############################################################################

# These instructions search the directory tree when cmake is
# invoked and put all files that match the pattern in the variables
# `sources` and `data`.
file(GLOB_RECURSE sources main.cpp *.h)
# You can use set(sources src/main.cpp) etc if you don't want to
# use globbing to find files automatically.

###############################################################################
## target definitions #########################################################
###############################################################################

# The data is just added to the executable, because in some IDEs (QtCreator)
# files are invisible when they are not explicitly part of the project.
add_executable(gfold_lr ${sources})
# Just for example add some compiler flags.
target_compile_options(gfold_lr PUBLIC -Wall -Wfloat-conversion)
message(${CMAKE_CXX_COMPILER})

# This allows to include files relative to the root of the src directory with a <> pair
# target_include_directories(gfold_lr PUBLIC src/main)

# This copies all resource files in the build directory.
# We need this, because we want to work with paths relative to the executable.
file(COPY ${data} DESTINATION resources)

###############################################################################
## dependencies ###############################################################
###############################################################################

# This defines the variables Boost_LIBRARIES that containts all library names
# that we need to link into the program.
FIND_PACKAGE(GSL REQUIRED)
set(LIBS ${LIBS} ${GSL_LIBRARIES} ${GSLCBLAS_LIBRARIES})

target_link_libraries(gfold_lr PUBLIC
        ${GSL_LIBRARIES}     # here you can add any library dependencies
        )

include_directories(${GSL_INCLUDE_DIRS} ${GSLCBLAS_INCLUDE_DIRS})

message(${CMAKE_CXX_COMPILER})

###############################################################################
## packaging ##################################################################
###############################################################################

# All install commands get the same destination. this allows us to use paths
# relative to the executable.
install(TARGETS gfold_lr DESTINATION example_destination)

# Now comes everything we need, to create a package
# there are a lot more variables you can set, and some
# you need to set for some package types, but we want to
# be minimal here.
set(CPACK_PACKAGE_NAME "gfold_lr")
set(CPACK_PACKAGE_VERSION "1.0.0")

# We don't want to split our program up into several incomplete pieces.
set(CPACK_MONOLITHIC_INSTALL 1)

# This must be last
include(CPack)