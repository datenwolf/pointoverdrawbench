#include "debuggl.h"
#include <GL/gl.h>
#include <stdio.h>

int debuggl_check_call(
	char const * const file,
	char const * const func,
	unsigned int const line,
	char const * const what)
{
	int errors = 0;
        GLenum err;
	while( (err = glGetError()) != GL_NO_ERROR) {
		++errors;
		char const * errstr = NULL;
		switch( err ) {
			case GL_INVALID_ENUM:      errstr = "invalid enum"; break;
			case GL_INVALID_VALUE:     errstr = "invalid value"; break;
			case GL_INVALID_OPERATION: errstr = "invalid operation"; break;
			case GL_STACK_OVERFLOW:    errstr = "invalid overflow"; break;
			case GL_STACK_UNDERFLOW:   errstr = "invalid underflow"; break;
			case GL_OUT_OF_MEMORY:     errstr = "invalid memory"; break;
		}
		fprintf(stderr,
			"%s(%d)/%s: %s => OpenGL Error: %s\n",
			file, (int)line, func, what,
			errstr );
	}
	return errors;
}

int debuggl_trace_call(
	char const * const file,
	char const * const func,
	unsigned int const line,
	char const * const what)
{
	int errors = 0;
        GLenum err;
	do {
		err = glGetError();
		char const * errstr = NULL;
		switch( err ) {
			case GL_INVALID_ENUM:      errstr = "invalid enum"; break;
			case GL_INVALID_VALUE:     errstr = "invalid value"; break;
			case GL_INVALID_OPERATION: errstr = "invalid operation"; break;
			case GL_STACK_OVERFLOW:    errstr = "invalid overflow"; break;
			case GL_STACK_UNDERFLOW:   errstr = "invalid underflow"; break;
			case GL_OUT_OF_MEMORY:     errstr = "invalid memory"; break;
		}

		if( errstr ) {
			++errors;
			fprintf(stderr,
				"%s(%d)/%s: %s => OpenGL Error: %s\n",
				file, (int)line, func, what,
				errstr );
		}
		else {
			fprintf(stderr,
				"%s(%d): %s OK\n",
				func, (int)line, what);
		}
	} while( GL_NO_ERROR != err );
	return errors;
}

