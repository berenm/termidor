cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(CMakeExtraBootstrap)
include(GNUInstallDirs)
include(AddSubmodule)
include(FindCXX11)
include(FixCCache)
include(FixStaticLink)


project(anyterm CXX)

add_submodule(cpp-netlib git://github.com/cpp-netlib/cpp-netlib.git lib/cpp-netlib TAG 0.9.4 INCLUDE_DIRS .)

find_package(Boost COMPONENTS thread program_options filesystem regex iostreams date_time system serialization REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(VTE REQUIRED vte)

include_directories(
  ${VTE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)
link_directories(
  ${VTE_LIBRARY_DIRS}
  ${Boost_LIBRARY_DIRS}
)

# -------------------------------------------------------------------------
# project version
file(READ version ANYTERM_VERSION)
string(STRIP "${ANYTERM_VERSION}" ANYTERM_VERSION)
string(REGEX REPLACE "\\..*$" "" ANYTERM_SOVERSION "${ANYTERM_VERSION}")

include_directories(
  include
  ext/cgi/
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# -------------------------------------------------------------------------
# anyterm
file(GLOB_RECURSE ANYTERM_SOURCES src/anyterm/*.cpp)
add_library(anyterm SHARED ${ANYTERM_SOURCES})
set_target_properties(anyterm PROPERTIES
  VERSION   "${ANYTERM_VERSION}"
  SOVERSION "${ANYTERM_SOVERSION}"
  SONAME    "anyterm${ANYTERM_SOVERSION}"
)
target_link_libraries(anyterm ${VTE_LIBRARIES} ${Boost_LIBRARIES})

# -------------------------------------------------------------------------
# anytermd
add_executable(anytermd src/anytermd.cpp)
target_link_libraries(anytermd anyterm ${Boost_LIBRARIES} cppnetlib-uri)

# -------------------------------------------------------------------------
# installation
install(TARGETS anytermd anyterm
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY include
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
