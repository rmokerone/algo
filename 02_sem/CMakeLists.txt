
ADD_EXECUTABLE(1_create_remove_sem 1_create_remove_sem.c)
TARGET_LINK_LIBRARIES(1_create_remove_sem pthread)


ADD_EXECUTABLE(2_post 2_post.c)
TARGET_LINK_LIBRARIES(2_post pthread)

ADD_EXECUTABLE(2_wait 2_wait.c)
TARGET_LINK_LIBRARIES(2_wait pthread)

ADD_EXECUTABLE(3_sem_limit 3_sem_limit.c)
TARGET_LINK_LIBRARIES(3_sem_limit pthread)