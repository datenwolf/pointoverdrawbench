#include "shaderloader.h"

#include <stdlib.h>
#include <stdio.h>

#if   defined(_WIN32)
#include <windows.h>
#define shader_gl_proc(x) wglGetProcAddress(#x)
#elif defined(__APPLE__) && defined(__MACH__)
#define shader_gl_proc(x) ((void(*)())x)
#else
#include <GL/glx.h>
#define shader_gl_proc(x) glXGetProcAddress(#x)
#endif

static
size_t linecount(char const * str)
{
	size_t count = 0;
	for(;*str;str++) { count += ('\n' == *str); }
	return count;
}

/* OpenGL function loading -- uses no caching, but shader loading is a
 * seldomly executed operation and never called from time critical sections. */
typedef void(*shader_func_ptr)();
static
shader_func_ptr shader_gl_proc_address(char const * const procname)
{
	return NULL;
}

static
GLuint shader_glCreateShader(GLenum unit)
{
	GLuint r = 0;
	shader_func_ptr const proc = shader_gl_proc(glCreateShader);
	if( proc ) { r = ((PFNGLCREATESHADERPROC)proc)(unit); }
	return r;
}

static
void shader_glShaderSource(
	GLuint shader,
	GLsizei count,
	GLchar const * const * const string,
	GLint  const * const lengths)
{
	shader_func_ptr const proc = shader_gl_proc(glShaderSource);
	if( proc ) { ((PFNGLSHADERSOURCEPROC)proc)(
			shader, count, string, lengths);
	}
}

static
void shader_glCompileShader(GLuint shader)
{
	shader_func_ptr const proc = shader_gl_proc(glCompileShader);
	if( proc ) { ((PFNGLCOMPILESHADERPROC)proc)(shader); }
}

static
void shader_glGetShaderiv(GLuint shader, GLenum pname, GLint *params)
{
	shader_func_ptr const proc = shader_gl_proc(glGetShaderiv);
	if( proc ) { ((PFNGLGETSHADERIVPROC)proc)(shader, pname, params); }
}

static
void shader_glGetShaderInfoLog(
	GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
{
	shader_func_ptr const proc = shader_gl_proc(glGetShaderInfoLog);
	if( proc ) { ((PFNGLGETSHADERINFOLOGPROC)proc)(
			shader, bufSize, length, infoLog);
	}
}

static
void shader_glDeleteShader(GLuint shader)
{
	shader_func_ptr const proc = shader_gl_proc(glDeleteShader);
	if( proc ) { ((PFNGLDELETESHADERPROC)proc)(shader); }
}

static
GLuint shader_glCreateProgram()
{
	GLuint r = 0;
	shader_func_ptr const proc = shader_gl_proc(glCreateProgram);
	if( proc ) { r = ((PFNGLCREATEPROGRAMPROC)proc)(); }
	return r;
}

static
void shader_glAttachShader(GLuint program, GLuint shader)
{
	shader_func_ptr const proc = shader_gl_proc(glAttachShader);
	if( proc ) { ((PFNGLATTACHSHADERPROC)proc)(program, shader); }
}

static
void shader_glLinkProgram(GLuint program)
{
	shader_func_ptr const proc = shader_gl_proc(glLinkProgram);
	if( proc ) { ((PFNGLLINKPROGRAMPROC)proc)(program); }
}

static
void shader_glGetProgramiv(GLuint program, GLenum pname, GLint *params)
{
	shader_func_ptr const proc = shader_gl_proc(glGetProgramiv);
	if( proc ) { ((PFNGLGETPROGRAMIVPROC)proc)(program, pname, params); }
}

static
void shader_glGetProgramInfoLog(
	GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
{
	shader_func_ptr const proc = shader_gl_proc(glGetProgramInfoLog);
	if( proc ) { ((PFNGLGETSHADERINFOLOGPROC)proc)(
			program, bufSize, length, infoLog);
	}
}

static
void shader_glDeleteProgram(GLuint program)
{
	shader_func_ptr const proc = shader_gl_proc(glDeleteProgram);
	if( proc ) { ((PFNGLDELETEPROGRAMPROC)proc)(program); }
}

/* shader loader functions */

GLuint shader_load_from_files(
	GLenum shader_unit,
	char const * const * const filepaths)
{
	size_t i;
	size_t n_sources_allocated = 0;
	GLchar const ** sources = NULL;
	GLuint * lengths = NULL;
	GLint shader_status = GL_FALSE;
	GLuint shader = 0;

	shader = shader_glCreateShader(shader_unit);
	if(!shader) { goto failed; }

	int filecount = 0;
	while( filepaths[filecount] ) { filecount++; }

	sources = malloc(sizeof(char*)*(filecount+1));
	if(!sources) { goto failed; }
	sources[filecount] = 0;

	lengths = malloc(sizeof(GLuint)*(filecount+1));
	if(!lengths) { goto failed; }
	lengths[filecount] = 0;

	for(i = 0; i < filecount; i++) {
		size_t length = 0;
		size_t read_length = 0;
		GLchar *sourcestring = NULL;
		FILE *fil = fopen(filepaths[i], "r");
		if(!fil) {
			fprintf(stderr,
				"could not open file '%s'\n",
				filepaths[i] );
			goto failed;
		}

		fseek(fil, 0, SEEK_END);
		length = ftell(fil);
		lengths[i] = length;
		rewind(fil);
		
		sourcestring = malloc(length);
		if( !sourcestring ) { goto failed; }
		sources[i] = sourcestring;
		n_sources_allocated = i + 1;

		read_length = fread(sourcestring, 1, length, fil);
		fclose(fil);
		if( length != read_length ) { goto failed; }
	}


	shader_glShaderSource(shader, 1, sources, lengths);

	shader_glCompileShader(shader);

	shader_glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_status);
	if( shader_status == GL_FALSE ) {
		GLint log_length, returned_length;
		shader_glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
		char *shader_infolog = malloc(log_length);
		if( shader_infolog ) {
			shader_glGetShaderInfoLog(
				shader,
				log_length,
				&returned_length,
				shader_infolog );
			fprintf(stderr, "shader compilation failed; sources:\n");
			for(i = 0; i < filecount; i++) {
				fprintf(stderr, " %.2d: %s\n", i, filepaths[i]);
			}
			fputs("compile log:\n", stderr);
			fwrite(shader_infolog, returned_length, 1, stderr);
			free(shader_infolog);
		}
		goto failed;
	}

cleanup:
	free(lengths);
	for(i = 0; i < n_sources_allocated; ++i) { free((void*)sources[i]); }
	free(sources);

	return shader;

failed:
	shader_glDeleteShader(shader);	
	shader = 0;
	goto cleanup;
}

GLuint shader_program_load_from_files(
	shader_program_sources const * const sources )
{
	size_t i;
	size_t n_shaders = 0;
	GLuint program = 0;
	GLuint *shaders = NULL;
	GLint linkStatus;

	for( n_shaders = 0
	   ; sources[n_shaders].unit && sources[n_shaders].paths
	   ; ++n_shaders);
	if( !n_shaders ) { goto failed; };
	shaders = calloc(sizeof(*shaders), n_shaders);
	if( !shaders ) { goto failed; }

	program = shader_glCreateProgram();
	if(!program) { goto failed; }

	for( i = 0; i < n_shaders; ++i ) {
		shaders[i] =
			shader_load_from_files(
				sources[i].unit,
				sources[i].paths );
		if( !shaders[i] ) { goto failed; }
		shader_glAttachShader(program, shaders[i]);
	}

	shader_glLinkProgram(program);
	shader_glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if( linkStatus == GL_FALSE ) {
		GLint log_length, returned_length;
		shader_glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
		char *program_infolog= malloc(log_length);
		if( program_infolog ) {
			shader_glGetProgramInfoLog(
				program,
				log_length,
				&returned_length,
				program_infolog);
			fwrite(program_infolog, returned_length, 1, stderr);
			free( program_infolog );
		}
	}

cleanup:
	/* shaders will get actually deleted when program gets deleted */
	for( i = 0; i < n_shaders; ++i ) {
		if( shaders[i] ) {
			shader_glDeleteShader(shaders[i]);
		}
	}
	free(shaders);

	return program;

failed:
	shader_glDeleteProgram(program);
	program = 0;
	goto cleanup;
}
