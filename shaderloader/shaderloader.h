#pragma once
#ifndef SHADERLOADER_H
#define SHADERLOADER_H

#if defined(_WIN32)
#include <windows.h>
#endif
#include <GL/gl.h>

GLuint shader_load_from_files(
	GLenum shader_unit,
	char const * const * const filepaths );

typedef struct shader_program_sources {
	GLenum unit;
	char const * const * paths;
} shader_program_sources;

GLuint shader_program_load_from_files(
	shader_program_sources const * const sources );

#endif/*SHADERLOADER_H*/
