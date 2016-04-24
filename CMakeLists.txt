cmake_minimum_required(VERSION 2.8)
project(pointoverdrawbench)

find_package(OpenGL)
find_package(GLUT)
find_package(GLEW)

if(MSVC)
set(EXTRA_LIBRARIES)
else(MSVC)
set(EXTRA_LIBRARIES m)
endif(MSVC)

add_executable(pointoverdrawbench
	main.c
	positiongen.c
	solid.c
	stats.c
	debuggl/debuggl.c
	shaderloader/shaderloader.c
)
target_link_libraries(pointoverdrawbench
	${OPENGL_gl_LIBRARY}
	${GLUT_LIBRARIES}
	${GLEW_LIBRARIES}
	${EXTRA_LIBRARIES}
)
file(COPY
	positiongen.glsl
	mvp.vs.glsl
	solid.fs.glsl
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
