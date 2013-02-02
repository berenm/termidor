if(__CMAKE_EXTRA_BOOTSTRAP_CMAKE__)
  return()
endif()
set(__CMAKE_EXTRA_BOOTSTRAP_CMAKE__ TRUE)


function(__cmake_extra_bootstrap_error)
  message(FATAL_ERROR "An error occured while checking out cmake-extra submodule.\n"
                      "Please get a copy of cmake-extra sources from https://github.com/berenm/cmake-extra and copy them in ${CMAKE_CURRENT_SOURCE_DIR}/cmake/extra.\n"
                      "For short :\n"
                      "  cd ${CMAKE_CURRENT_SOURCE_DIR}/cmake/extra/\n"
                      "  wget https://github.com/berenm/cmake-extra/archive/master.tar.gz\n"
                      "  tar xzvf master.tar.gz\n")
endfunction()


find_package(Git)
if(NOT GIT_FOUND)
  if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/extra)
    __cmake_extra_bootstrap_error()
  endif()
  return()
endif()

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/extra)
  message(STATUS "Checking out cmake-extra submodule...")
  execute_process(
    COMMAND ${GIT_EXECUTABLE} submodule add --quiet git://github.com/berenm/cmake-extra.git cmake/extra
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE CMAKE_EXTRA_STATUS
  )

  if(NOT CMAKE_EXTRA_STATUS EQUAL 0)
    __cmake_extra_bootstrap_error()
    return()
  endif()
endif()

message(STATUS "Updating cmake-extra submodule...")
execute_process(
  COMMAND ${GIT_EXECUTABLE} submodule update --quiet --init cmake/extra
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
execute_process(
  COMMAND ${GIT_EXECUTABLE} fetch --quiet origin
  COMMAND ${GIT_EXECUTABLE} checkout --quiet origin/master -B master
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/cmake/extra/CMakeExtraBootstrap.cmake ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CMakeExtraBootstrap.cmake
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cmake/extra
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/extra)
