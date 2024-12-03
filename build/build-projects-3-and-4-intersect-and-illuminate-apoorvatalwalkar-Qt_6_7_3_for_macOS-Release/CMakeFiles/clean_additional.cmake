# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles/projects_ray_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/projects_ray_autogen.dir/ParseCache.txt"
  "projects_ray_autogen"
  )
endif()
