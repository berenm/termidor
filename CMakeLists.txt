cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(CMakeExtraBootstrap)
include(GNUInstallDirs)
include(AddSubmodule)
include(FindCXX11)
include(FixCCache)
include(FixStaticLink)


project(termidor CXX)

add_submodule(cpp-netlib git://github.com/cpp-netlib/cpp-netlib.git lib/cpp-netlib TAG cpp-netlib-0.11.0-final INCLUDE_DIRS .)

find_package(Boost COMPONENTS thread program_options filesystem regex iostreams date_time system serialization REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(VTE REQUIRED vte-2.90)

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
file(READ version TERMIDOR_VERSION)
string(STRIP "${TERMIDOR_VERSION}" TERMIDOR_VERSION)
string(REGEX REPLACE "\\..*$" "" TERMIDOR_SOVERSION "${TERMIDOR_VERSION}")

include_directories(
  include
  ext/cgi/
)

# -------------------------------------------------------------------------
# termidor
file(GLOB_RECURSE TERMIDOR_SOURCES src/termidor/*.cpp)
add_library(termidor SHARED ${TERMIDOR_SOURCES})
set_target_properties(termidor PROPERTIES
  VERSION   "${TERMIDOR_VERSION}"
  SOVERSION "${TERMIDOR_SOVERSION}"
  SONAME    "termidor${TERMIDOR_SOVERSION}"
)
target_link_libraries(termidor ${VTE_LIBRARIES} ${Boost_LIBRARIES})

# -------------------------------------------------------------------------
# termidord
add_executable(termidord src/termidord.cpp)
target_link_libraries(termidord termidor ${Boost_LIBRARIES} cppnetlib-uri)

# -------------------------------------------------------------------------
# installation
install(TARGETS termidord termidor
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY include
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# -------------------------------------------------------------------------
# tests
if(BUILD_TESTS)
  enable_testing()
endif()

if(ENABLE_GCOVERALLS)
  include(GCoveralls)
  enable_gcoveralls()
endif()