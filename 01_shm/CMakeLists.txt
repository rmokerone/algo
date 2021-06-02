
ADD_EXECUTABLE(1_create_remove_shm 1_create_remove_shm.c)
TARGET_LINK_LIBRARIES(1_create_remove_shm rt)


ADD_EXECUTABLE(2_read 2_read.c)
TARGET_LINK_LIBRARIES(2_read rt)

ADD_EXECUTABLE(2_write 2_write.c)
TARGET_LINK_LIBRARIES(2_write rt)