cmake_minimum_required(VERSION 2.8.3)
project(noah_powerboard)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  #  mrobot_driver_msgs 
  #  roscan
)

catkin_package(
  CATKIN_DEPENDS roscpp std_msgs
)

###########
## Build ##
###########
set (CMAKE_CXX_FLAGS "-std=c++11")
include_directories(
  ${catkin_INCLUDE_DIRS}
  include/noah_powerboard
)

add_executable( noah_powerboard_node
    src/main.cpp
	src/uart.cpp
	src/powerboard.cpp
)
target_link_libraries(noah_powerboard_node
  ${catkin_LIBRARIES} 
)

#############
## Install ##
#############


#############
## Testing ##
#############


install(TARGETS noah_powerboard_node
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})

## Install project namespaced headers
install(DIRECTORY include/${PROJECT_NAME}/
        DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
        FILES_MATCHING PATTERN "*.h"
        PATTERN ".svn" EXCLUDE)

install(DIRECTORY launch

    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
    USE_SOURCE_PERMISSIONS
    )








