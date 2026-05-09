cmake_minimum_required(VERSION 3.15)

project(cppserial)

set(CPPSERIAL_SRC
			${PROJECT_NAME}/binaryarchive.cpp
			${PROJECT_NAME}/serializer.cpp
			${PROJECT_NAME}/endiannessprovider.cpp
)

set(CPPSERIAL_HEADERS 
			${PROJECT_NAME}/binaryarchive.h
			${PROJECT_NAME}/context.h
			${PROJECT_NAME}/serializable.h
			${PROJECT_NAME}/serializer.h
)

add_library(${PROJECT_NAME} STATIC ${CPPSERIAL_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(FILES ${CPPSERIAL_HEADERS} DESTINATION ${CPPSERIAL_INSTALL_INCLUDE}/${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME} DESTINATION ${CPPSERIAL_INSTALL_LIB})