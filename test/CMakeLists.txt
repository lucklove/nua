SET(TEST_SOURCES
	test.cc
    stack.cc
    MetatableRegistry.cc
    primitives.cc
    Selector.cc
)

INCLUDE_DIRECTORIES(../inc)
ADD_EXECUTABLE(nua_test ${TEST_SOURCES})
TARGET_LINK_LIBRARIES(nua_test lua)
ADD_CUSTOM_TARGET(run_test COMMAND ${CMAKE_BINARY_DIR}/test/nua_test DEPENDS nua_test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
