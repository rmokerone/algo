ADD_EXECUTABLE(dumps dumps.c ${src_base})
TARGET_LINK_LIBRARIES(dumps)

ADD_EXECUTABLE(loads loads.c ${src_base})
TARGET_LINK_LIBRARIES(loads)