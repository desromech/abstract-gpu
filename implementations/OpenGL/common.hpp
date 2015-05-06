#ifndef _AGPU_GL_COMMON_HPP
#define _AGPU_GL_COMMON_HPP

#include <AGPU/agpu.h>
#include <GL/gl.h>

#define CHECK_POINTER(pointer) if (!(pointer)) return AGPU_NULL_POINTER;
#define MAKE_CURRENT() if (!makeCurrent) return AGPU_NOT_CURRENT_CONTEXT;
#define CHECK_CURRENT() if (!isCurrentContext()) return AGPU_NOT_CURRENT_CONTEXT;

#endif //_AGPU_GL_COMMON_HPP
