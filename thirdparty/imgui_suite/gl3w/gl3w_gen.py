#!/usr/bin/env python

#   This file is part of gl3w, hosted at https://github.com/skaslev/gl3w
#
#   This is free and unencumbered software released into the public domain.
#
#   Anyone is free to copy, modify, publish, use, compile, sell, or
#   distribute this software, either in source code form or as a compiled
#   binary, for any purpose, commercial or non-commercial, and by any
#   means.
#
#   In jurisdictions that recognize copyright laws, the author or authors
#   of this software dedicate any and all copyright interest in the
#   software to the public domain. We make this dedication for the benefit
#   of the public at large and to the detriment of our heirs and
#   successors. We intend this dedication to be an overt act of
#   relinquishment in perpetuity of all present and future rights to this
#   software under copyright law.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
#   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
#   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
#   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
#   OTHER DEALINGS IN THE SOFTWARE.

# Allow Python 2.6+ to use the print() function
from __future__ import print_function

import argparse
import os
import re

# Try to import Python 3 library urllib.request
# and if it fails, fall back to Python 2 urllib2
try:
    import urllib.request as urllib2
except ImportError:
    import urllib2

# UNLICENSE copyright header
UNLICENSE = r'''/*
 * This file was generated with gl3w_gen.py, part of gl3w
 * (hosted at https://github.com/skaslev/gl3w)
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

'''

EXT_SUFFIX = ['ARB', 'EXT', 'KHR', 'OVR', 'NV', 'AMD', 'INTEL']

def is_ext(proc):
    return any(proc.endswith(suffix) for suffix in EXT_SUFFIX)

def write(f, s):
    f.write(s.encode('utf-8'))

def touch_dir(path):
    if not os.path.exists(path):
        os.makedirs(path)

def download(url, dst):
    if os.path.exists(dst):
        print('Reusing {0}...'.format(dst))
        return

    print('Downloading {0}...'.format(dst))
    web = urllib2.urlopen(urllib2.Request(url, headers={'User-Agent': 'Mozilla/5.0'}))
    with open(dst, 'wb') as f:
        f.writelines(web.readlines())

parser = argparse.ArgumentParser(description='gl3w generator script')
parser.add_argument('--ext', action='store_true', help='Load extensions')
parser.add_argument('--root', type=str, default='', help='Root directory')
args = parser.parse_args()

# Create directories
touch_dir(os.path.join(args.root, 'include/GL'))
touch_dir(os.path.join(args.root, 'include/KHR'))
touch_dir(os.path.join(args.root, 'src'))

# Download glcorearb.h and khrplatform.h
download('https://www.khronos.org/registry/OpenGL/api/GL/glcorearb.h',
         os.path.join(args.root, 'include/GL/glcorearb.h'))
download('https://www.khronos.org/registry/EGL/api/KHR/khrplatform.h',
         os.path.join(args.root, 'include/KHR/khrplatform.h'))

# Parse function names from glcorearb.h
print('Parsing glcorearb.h header...')
procs = []
p = re.compile(r'GLAPI.*APIENTRY\s+(\w+)')
with open(os.path.join(args.root, 'include/GL/glcorearb.h'), 'r') as f:
    for line in f:
        m = p.match(line)
        if not m:
            continue
        proc = m.group(1)
        if args.ext or not is_ext(proc):
            procs.append(proc)
procs.sort()

# Generate gl3w.h
print('Generating {0}...'.format(os.path.join(args.root, 'include/GL/gl3w.h')))
with open(os.path.join(args.root, 'include/GL/gl3w.h'), 'wb') as f:
    write(f, UNLICENSE)
    write(f, r'''#ifndef __gl3w_h_
#define __gl3w_h_

#include <GL/glcorearb.h>

#ifndef GL3W_API
#define GL3W_API
#endif

#ifndef __gl_h_
#define __gl_h_
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define GL3W_OK 0
#define GL3W_ERROR_INIT -1
#define GL3W_ERROR_LIBRARY_OPEN -2
#define GL3W_ERROR_OPENGL_VERSION -3

typedef void (*GL3WglProc)(void);
typedef GL3WglProc (*GL3WGetProcAddressProc)(const char *proc);

/* gl3w api */
GL3W_API int gl3wInit(void);
GL3W_API int gl3wInit2(GL3WGetProcAddressProc proc);
GL3W_API int gl3wIsSupported(int major, int minor);
GL3W_API GL3WglProc gl3wGetProcAddress(const char *proc);

/* gl3w internal state */
''')
    write(f, 'union GL3WProcs {\n')
    write(f, '\tGL3WglProc ptr[{0}];\n'.format(len(procs)))
    write(f, '\tstruct {\n')
    for proc in procs:
        write(f, '\t\t{0: <55} {1};\n'.format('PFN{0}PROC'.format(proc.upper()), proc[2:]))
    write(f, r'''	} gl;
};

GL3W_API extern union GL3WProcs gl3wProcs;

/* OpenGL functions */
''')
    for proc in procs:
        write(f, '#define {0: <48} gl3wProcs.gl.{1}\n'.format(proc, proc[2:]))
    write(f, r'''
#ifdef __cplusplus
}
#endif

#endif
''')

# Generate gl3w.c
print('Generating {0}...'.format(os.path.join(args.root, 'src/gl3w.c')))
with open(os.path.join(args.root, 'src/gl3w.c'), 'wb') as f:
    write(f, UNLICENSE)
    write(f, r'''#include <GL/gl3w.h>
#include <stdlib.h>

#define ARRAY_SIZE(x)  (sizeof(x) / sizeof((x)[0]))

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>

static HMODULE libgl;
typedef PROC(__stdcall* GL3WglGetProcAddr)(LPCSTR);
static GL3WglGetProcAddr wgl_get_proc_address;

static int open_libgl(void)
{
	libgl = LoadLibraryA("opengl32.dll");
	if (!libgl)
		return GL3W_ERROR_LIBRARY_OPEN;

	wgl_get_proc_address = (GL3WglGetProcAddr)GetProcAddress(libgl, "wglGetProcAddress");
	return GL3W_OK;
}

static void close_libgl(void)
{
	FreeLibrary(libgl);
}

static GL3WglProc get_proc(const char *proc)
{
	GL3WglProc res;

	res = (GL3WglProc)wgl_get_proc_address(proc);
	if (!res)
		res = (GL3WglProc)GetProcAddress(libgl, proc);
	return res;
}
#elif defined(__APPLE__)
#include <dlfcn.h>

static void *libgl;

static int open_libgl(void)
{
	libgl = dlopen("/System/Library/Frameworks/OpenGL.framework/OpenGL", RTLD_LAZY | RTLD_LOCAL);
	if (!libgl)
		return GL3W_ERROR_LIBRARY_OPEN;

	return GL3W_OK;
}

static void close_libgl(void)
{
	dlclose(libgl);
}

static GL3WglProc get_proc(const char *proc)
{
	GL3WglProc res;

	*(void **)(&res) = dlsym(libgl, proc);
	return res;
}
#else
#include <dlfcn.h>

static void *libgl;  /* OpenGL library */
static void *libglx;  /* GLX library */
static void *libegl;  /* EGL library */
static GL3WGetProcAddressProc gl_get_proc_address;

static void close_libgl(void)
{
	if (libgl) {
		dlclose(libgl);
		libgl = NULL;
	}
	if (libegl) {
		dlclose(libegl);
		libegl = NULL;
	}
	if (libglx) {
		dlclose(libglx);
		libglx = NULL;
	}
}

static int is_library_loaded(const char *name, void **lib)
{
	*lib = dlopen(name, RTLD_LAZY | RTLD_LOCAL | RTLD_NOLOAD);
	return *lib != NULL;
}

static int open_libs(void)
{
	/* On Linux we have two APIs to get process addresses: EGL and GLX.
	 * EGL is supported under both X11 and Wayland, whereas GLX is X11-specific.
	 * First check what's already loaded, the windowing library might have
	 * already loaded either EGL or GLX and we want to use the same one.
	 */

	if (is_library_loaded("libEGL.so.1", &libegl) ||
			is_library_loaded("libGLX.so.0", &libglx)) {
		libgl = dlopen("libOpenGL.so.0", RTLD_LAZY | RTLD_LOCAL);
		if (libgl)
			return GL3W_OK;
		else
			close_libgl();
	}

	if (is_library_loaded("libGL.so.1", &libgl))
		return GL3W_OK;

	/* Neither is already loaded, so we have to load one. Try EGL first
	 * because it is supported under both X11 and Wayland.
	 */

	/* Load OpenGL + EGL */
	libgl = dlopen("libOpenGL.so.0", RTLD_LAZY | RTLD_LOCAL);
	libegl = dlopen("libEGL.so.1", RTLD_LAZY | RTLD_LOCAL);
	if (libgl && libegl)
		return GL3W_OK;

	/* Fall back to legacy libGL, which includes GLX */
	close_libgl();
	libgl = dlopen("libGL.so.1", RTLD_LAZY | RTLD_LOCAL);
	if (libgl)
		return GL3W_OK;

	return GL3W_ERROR_LIBRARY_OPEN;
}

static int open_libgl(void)
{
	int res = open_libs();
	if (res)
		return res;

	if (libegl)
		*(void **)(&gl_get_proc_address) = dlsym(libegl, "eglGetProcAddress");
	else if (libglx)
		*(void **)(&gl_get_proc_address) = dlsym(libglx, "glXGetProcAddressARB");
	else
		*(void **)(&gl_get_proc_address) = dlsym(libgl, "glXGetProcAddressARB");

	if (!gl_get_proc_address) {
		close_libgl();
		return GL3W_ERROR_LIBRARY_OPEN;
	}

	return GL3W_OK;
}

static GL3WglProc get_proc(const char *proc)
{
	GL3WglProc res = NULL;

	/* Before EGL version 1.5, eglGetProcAddress doesn't support querying core
	 * functions and may return a dummy function if we try, so try to load the
	 * function from the GL library directly first.
	 */
	if (libegl)
		*(void **)(&res) = dlsym(libgl, proc);

	if (!res)
		res = gl_get_proc_address(proc);

	if (!libegl && !res)
		*(void **)(&res) = dlsym(libgl, proc);

	return res;
}
#endif

static struct {
	int major, minor;
} version;

static int parse_version(void)
{
	if (!glGetIntegerv)
		return GL3W_ERROR_INIT;

	glGetIntegerv(GL_MAJOR_VERSION, &version.major);
	glGetIntegerv(GL_MINOR_VERSION, &version.minor);

	if (version.major < 3)
		return GL3W_ERROR_OPENGL_VERSION;
	return GL3W_OK;
}

static void load_procs(GL3WGetProcAddressProc proc);

int gl3wInit(void)
{
	int res;

	res = open_libgl();
	if (res)
		return res;

	atexit(close_libgl);
	return gl3wInit2(get_proc);
}

int gl3wInit2(GL3WGetProcAddressProc proc)
{
	load_procs(proc);
	return parse_version();
}

int gl3wIsSupported(int major, int minor)
{
	if (major < 3)
		return 0;
	if (version.major == major)
		return version.minor >= minor;
	return version.major >= major;
}

GL3WglProc gl3wGetProcAddress(const char *proc)
{
	return get_proc(proc);
}

static const char *proc_names[] = {
''')
    for proc in procs:
        write(f, '\t"{0}",\n'.format(proc))
    write(f, r'''};

GL3W_API union GL3WProcs gl3wProcs;

static void load_procs(GL3WGetProcAddressProc proc)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(proc_names); i++)
		gl3wProcs.ptr[i] = proc(proc_names[i]);
}
''')
