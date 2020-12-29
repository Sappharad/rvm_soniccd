project(soniccd)

cmake_minimum_required(VERSION 3.0)

include(FindPkgConfig)

set(CMAKE_C_FLAGS "-g -DLINUX -DSDL_DISABLE_IMMINTRIN_H -DSDL_DISABLE_MMINTRIN_H -std=c99 ${CMAKE_C_FLAGS}")

pkg_check_modules(SDL2 sdl2 REQUIRED)
if(NSWITCH)
  set(CMAKE_C_FLAGS "-DGLEW_NO_GLU ${CMAKE_C_FLAGS}")
  find_package(GLEW REQUIRED)
  find_library(GLEW_LIBRARY NAMES GLEW)
  set(EXTRA_LIBS mpg123 modplug mikmod vorbisfile vorbis ogg FLAC fluidsynth opusfile opus ${GLEW_LIBRARY} stdc++)
else()
  set(OpenGL_GL_PREFERENCE LEGACY)
  find_package(OpenGL REQUIRED)
  set(EXTRA_LIBS ${OPENGL_gl_LIBRARY})
endif(NSWITCH)

include_directories(
  ${SDL2_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/rvm/Core
)

file(GLOB SRC rvm/*.c rvm/Core/*.c)
add_executable(${CMAKE_PROJECT_NAME}
  ${SRC}
)

target_link_libraries(${CMAKE_PROJECT_NAME}
  SDL2_mixer
  ${SDL2_LIBRARIES}
  ${EXTRA_LIBS}
  m
)

if(NSWITCH)
  add_definitions(-D__SWITCH__)
  add_custom_target(${CMAKE_PROJECT_NAME}.nro
    DEPENDS ${CMAKE_PROJECT_NAME}
    COMMAND nacptool --create "Sonic CD" "Sappharad, usineur" "0.7.5" ${CMAKE_PROJECT_NAME}.nacp
    COMMAND elf2nro ${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME}.nro --icon=${CMAKE_SOURCE_DIR}/icon.jpg --nacp=${CMAKE_PROJECT_NAME}.nacp
  )
  add_custom_target(nxlink
    COMMAND nxlink -a $(SWITCHIP) ${CMAKE_PROJECT_NAME}.nro -s -p ${CMAKE_PROJECT_NAME}/${CMAKE_PROJECT_NAME}.nro
    DEPENDS ${CMAKE_PROJECT_NAME}.nro
  )
endif(NSWITCH)
