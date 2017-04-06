

SET(VR_SRC Vr.cpp
				Vr.h)
file(GLOB SRC_FILES ${VR_SRC})

ADD_LIBRARY(IVR ${SRC_FILES})

target_include_directories(IVR PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


#ADD_EXECUTABLE(VR ${EXE_GUI_TYPE} ${VR_SRC})



## OpenVR API path


# If not set, determines the running platform architecture.
if(NOT PLATFORM)
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(PLATFORM 64)
  else()
    set(PLATFORM 32)
  endif()
endif()

  # Binaries path for thirdparties are not generics so we try to guess their suffixes.
  set(WINDOWS_PATH_SUFFIXES win${PLATFORM} Win${PLATFORM} x${PLATFORM})


find_library(OPENVR_LIBRARIES
  NAMES
    openvr_api
  PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/openvr/bin
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/openvr/lib
  PATH_SUFFIXES
    osx32
    linux64
    ${WINDOWS_PATH_SUFFIXES}
  NO_DEFAULT_PATH
  NO_CMAKE_FIND_ROOT_PATH
)
set(OPENVR_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../lib/openvr/headers)

message(STATUS "THIS IS THE CURRENT DIRECTORY: ${CMAKE_CURRENT_SOURCE_DIR}")


include_directories(
  ${OPENVR_INCLUDE_DIR}
)


#TARGET_LINK_LIBRARIES(VR ${OPENVR_LIBRARIES})

