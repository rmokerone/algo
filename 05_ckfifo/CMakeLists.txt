ADD_EXECUTABLE(ckfifo_test_consumer test_consumer.cpp ckfifo.cpp ${src_base})
TARGET_LINK_LIBRARIES(ckfifo_test_consumer)

ADD_EXECUTABLE(ckfifo_test_producer test_producer.cpp ckfifo.cpp ${src_base})
TARGET_LINK_LIBRARIES(ckfifo_test_producer)