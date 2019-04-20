#ifndef _AGPU_GL_UTILITY_HPP_
#define _AGPU_GL_UTILITY_HPP_

#include "device.hpp"

namespace AgpuGL
{

inline GLenum mapFieldType(agpu_field_type type)
{
	switch(type)
	{
	case AGPU_FLOAT: return GL_FLOAT;
	case AGPU_HALF_FLOAT: return GL_HALF_FLOAT;
	case AGPU_DOUBLE: return GL_DOUBLE;
	case AGPU_FIXED: return GL_FIXED;
	case AGPU_BYTE: return GL_BYTE;
	case AGPU_UNSIGNED_BYTE: return GL_UNSIGNED_BYTE;
	case AGPU_SHORT: return GL_SHORT;
	case AGPU_UNSIGNED_SHORT: return GL_UNSIGNED_SHORT;
	case AGPU_INT: return GL_INT;
	case AGPU_UNSIGNED_INT: return GL_UNSIGNED_INT;
	default: abort();
	}
}

} // End of namespace AgpuGL

#endif //_AGPU_GL_UTILITY_HPP_
