ADD_EXECUTABLE(test_consumer test_consumer.cpp cshmfifo.cpp ${src_base})
TARGET_LINK_LIBRARIES(test_consumer)

ADD_EXECUTABLE(test_producer test_producer.cpp cshmfifo.cpp ${src_base})
TARGET_LINK_LIBRARIES(test_producer)