# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\NEHNES_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\NEHNES_autogen.dir\\ParseCache.txt"
  "NEHNES_autogen"
  )
endif()
