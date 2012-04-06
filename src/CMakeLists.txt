cmake_minimum_required(VERSION 2.0)
PROJECT(Minigolf)

INCLUDE_DIRECTORIES(${GLUI_INCLUDE_DIR} ${GLUT_INCLUDE_DIR} ${GLU_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})

SET(Minigolf_SOURCE_FILES 
	main.cpp
	data.cpp
	linkedlist.cpp
	objects.cpp
	physics.cpp
	scorecard.cpp
	)

ADD_EXECUTABLE(Minigolf ${Minigolf_SOURCE_FILES})
TARGET_LINK_LIBRARIES(Minigolf
	${GLUI_LIBRARIES} ${GLUT_LIBRARIES} ${GLU_LIBRARIES} ${OPENGL_LIBRARIES})
