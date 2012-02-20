cmake_minimum_required(VERSION 2.0)
PROJECT(Minigolf)

INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR} ${GLU_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})

SET(MinigolfTest_SOURCE_FILES 
	testmain.c
	testlinkedlist.c
	testphysics.c
	
	../src/linkedlist.c
	../src/physics.c
	../src/objects.c
	)
	
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/test ${PROJECT_SOURCE_DIR}/src)

ADD_EXECUTABLE(MinigolfTest ${MinigolfTest_SOURCE_FILES})
TARGET_LINK_LIBRARIES(MinigolfTest
	${OPENGL_LIBRARIES} ${GLU_LIBRARIES} ${GLUT_LIBRARIES})
