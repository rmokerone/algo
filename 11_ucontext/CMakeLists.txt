ADD_EXECUTABLE(1_sample_example 1_sample_example.c)
TARGET_LINK_LIBRARIES(1_sample_example)


ADD_EXECUTABLE(2_test_context 2_test_context.c)
TARGET_LINK_LIBRARIES(2_test_context)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/11_ucontext/uthread)
ADD_EXECUTABLE(3_test_uthread 3_test_uthread.cpp uthread/uthread.cpp)
# TARGET_LINK_LIBRARIES(3_test_uthread)