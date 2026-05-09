cmake_minimum_required(VERSION 3.15)

project(cppserial-test)

set(CPPSERIAL_TEST_SRC	main.cpp
						src/binaryarchivetest.cpp
						src/serializertest.cpp
)

add_executable(${PROJECT_NAME} ${CPPSERIAL_TEST_SRC})

target_link_libraries(${PROJECT_NAME} PUBLIC cppserial)
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest)

if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE "/EHsc")
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION ${CPPSERIAL_INSTALL_BIN})
