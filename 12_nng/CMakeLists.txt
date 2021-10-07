
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/3rdparty/include)
ADD_EXECUTABLE(1_pubsub 1_pubsub.c)
TARGET_LINK_LIBRARIES(1_pubsub nng pthread)

ADD_EXECUTABLE(2_routing 2_routing.c)
TARGET_LINK_LIBRARIES(2_routing nng pthread)