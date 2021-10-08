ADD_EXECUTABLE(cring_test 1_cring_test.c cring.c ${src_base})
TARGET_LINK_LIBRARIES(cring_test)

ADD_EXECUTABLE(sring_producer 2_sring_producer.c ipc.c  cring.c sring.c)
TARGET_LINK_LIBRARIES(sring_producer)

ADD_EXECUTABLE(sring_consumer 3_sring_consumer.c ipc.c cring.c sring.c )
TARGET_LINK_LIBRARIES(sring_consumer)