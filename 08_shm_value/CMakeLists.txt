ADD_EXECUTABLE(push test_csvalue_push.c csvalue.c ${src_base})
TARGET_LINK_LIBRARIES(push)

ADD_EXECUTABLE(pull test_csvalue_pull.c csvalue.c ${src_base})
TARGET_LINK_LIBRARIES(push)