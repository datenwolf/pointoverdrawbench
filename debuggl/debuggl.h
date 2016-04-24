#pragma once
#ifndef DEBUGGL_H
#define DEBUGGL_H

/* execute a OpenGL but check for errors only if compiled for debugging */
#ifndef NDEBUG
#define debuggl_trace(glcmd) \
	({ glcmd; debuggl_trace_call(__FILE__, __func__, __LINE__, #glcmd); })
#else
#define debuggl_trace(glcmd) glcmd
#endif

/* check for OpenGL errors */
#define debuggl_check(glcmd) \
	({ glcmd; debuggl_check_call(__FILE__, __func__, __LINE__, #glcmd); })

int debuggl_check_call(
	char const * const file,
	char const * const func,
	unsigned int const line,
	char const * const what);

int debuggl_trace_call(
	char const * const file,
	char const * const func,
	unsigned int const line,
	char const * const what);

#endif/*DEBUGGL_H*/
