ADD_EXECUTABLE(cthreadpool ctask.cpp main.cpp cthreadpool.cpp)
TARGET_LINK_LIBRARIES(cthreadpool pthread)