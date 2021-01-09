cmake_minimum_required(VERSION 2.8.3)
project(draco_point_cloud_rviz_plugin)

find_package(catkin REQUIRED 
  COMPONENTS
  roscpp
  rviz
  sensor_msgs
  roslib
  draco_point_cloud_transport
)

find_package(PCL REQUIRED)
find_package(Draco REQUIRED)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME} ${draco_LIBRARY_DIR}
  CATKIN_DEPENDS rviz draco_point_cloud_transport sensor_msgs
)

set(CMAKE_AUTOMOC ON)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets OpenGL)
add_definitions( ${QT_DEFINITIONS} )

###########
## Build ##
###########

include_directories(
  include
  ${CMAKE_BINARY_DIR}
  ${catkin_INCLUDE_DIRS}
  ${PCL_INCLUDE_DIRS}
  ${draco_INCLUDE_DIR}/..
  ${draco_INCLUDE_DIR}
)

link_directories(${draco_LIBRARY_DIR})

set(HEADER_FILES
    include/${PROJECT_NAME}/compressed_cloud_display.h
    include/${PROJECT_NAME}/DracotoPC2.h
    include/${PROJECT_NAME}/point_cloud_common.h
    include/${PROJECT_NAME}/point_cloud.h
    include/${PROJECT_NAME}/conversion_utilities.h

    #Draco
    lib/draco/src/draco/point_cloud/point_cloud.h
    lib/draco/src/draco/core/decoder_buffer.h
    lib/draco/src/draco/core/data_buffer.h
    lib/draco/src/draco/core/options.h
    lib/draco/src/draco/core/draco_types.h
    lib/draco/src/draco/core/quantization_utils.h
    lib/draco/src/draco/attributes/attribute_quantization_transform.h
    lib/draco/src/draco/attributes/geometry_attribute.h
    lib/draco/src/draco/attributes/point_attribute.h
    lib/draco/src/draco/attributes/attribute_transform.h
    lib/draco/src/draco/compression/decode.h
    lib/draco/src/draco/compression/attributes/kd_tree_attributes_decoder.h
    lib/draco/src/draco/compression/attributes/attributes_decoder.h
    lib/draco/src/draco/compression/point_cloud/point_cloud_kd_tree_decoder.h
    lib/draco/src/draco/compression/point_cloud/point_cloud_decoder.h
    lib/draco/src/draco/compression/point_cloud/point_cloud_sequential_decoder.h
    lib/draco/src/draco/compression/bit_coders/rans_bit_decoder.h
    lib/draco/src/draco/compression/bit_coders/direct_bit_decoder.h
    lib/draco/src/draco/metadata/metadata.h
    lib/draco/src/draco/metadata/metadata_decoder.h
)

set(SRC_FILES
    src/compressed_cloud_display.cpp
    src/DracotoPC2.cpp
    src/point_cloud_common.cpp
    src/point_cloud.cpp
    src/conversion_utilities.cpp

    #Draco
    lib/draco/src/draco/point_cloud/point_cloud.cc
    lib/draco/src/draco/core/decoder_buffer.cc
    lib/draco/src/draco/core/data_buffer.cc
    lib/draco/src/draco/core/options.cc
    lib/draco/src/draco/core/draco_types.cc
    lib/draco/src/draco/core/quantization_utils.cc
    lib/draco/src/draco/attributes/attribute_quantization_transform.cc
    lib/draco/src/draco/attributes/geometry_attribute.cc
    lib/draco/src/draco/attributes/point_attribute.cc
    lib/draco/src/draco/attributes/attribute_transform.cc
    lib/draco/src/draco/compression/decode.cc
    lib/draco/src/draco/compression/attributes/kd_tree_attributes_decoder.cc
    lib/draco/src/draco/compression/attributes/attributes_decoder.cc
    lib/draco/src/draco/compression/point_cloud/point_cloud_kd_tree_decoder.cc
    lib/draco/src/draco/compression/point_cloud/point_cloud_decoder.cc
    lib/draco/src/draco/compression/point_cloud/point_cloud_sequential_decoder.cc
    lib/draco/src/draco/compression/bit_coders/rans_bit_decoder.cc
    lib/draco/src/draco/compression/bit_coders/direct_bit_decoder.cc
    lib/draco/src/draco/metadata/metadata.cc
    lib/draco/src/draco/metadata/metadata_decoder.cc
)

add_library(${PROJECT_NAME} ${SRC_FILES} ${HEADER_FILES})

add_dependencies(${PROJECT_NAME} ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

target_link_libraries(${PROJECT_NAME}
  ${catkin_LIBRARIES}
  ${PCL_LIBRARIES}
  ${draco_LIBRARIES}
)

#############
## Install ##
#############

install(TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
)

install(
  DIRECTORY include/${PROJECT_NAME}/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
)
