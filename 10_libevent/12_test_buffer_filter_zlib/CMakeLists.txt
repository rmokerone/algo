
ADD_EXECUTABLE(12_test_buffer_filter_zlib main.cpp client.cpp server.cpp)
TARGET_LINK_LIBRARIES(12_test_buffer_filter_zlib event z)