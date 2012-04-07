cmake_minimum_required(VERSION 2.0)
PROJECT(Minigolf)

INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR} ${GLU_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})

SET(MinigolfTest_SOURCE_FILES 
	testmain.cpp
	testlinkedlist.cpp
	testphysics.cpp
	testfileparsing.cpp
	
	../src/linkedlist.cpp
	../src/physics.cpp
	../src/ballcuptee.cpp
	)
	
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/test ${PROJECT_SOURCE_DIR}/src)

ADD_EXECUTABLE(MinigolfTest ${MinigolfTest_SOURCE_FILES})
TARGET_LINK_LIBRARIES(MinigolfTest
	${OPENGL_LIBRARIES} ${GLU_LIBRARIES} ${GLUT_LIBRARIES})
