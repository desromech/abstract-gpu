#ifndef _AGPU_GL_PLATFORM_H_
#define _AGPU_GL_PLATFORM_H_

#include "object.hpp"

struct _agpu_platform
{
    agpu_icd_dispatch *dispatch;
};

extern _agpu_platform theGLPlatform;

#endif //_AGPU_GL_PLATFORM_H_

