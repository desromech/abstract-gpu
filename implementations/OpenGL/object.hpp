#ifndef _AGPU_GL_OBJECT_HPP_
#define _AGPU_GL_OBJECT_HPP_

#include <atomic>
#include <assert.h>
#include "common.hpp"

extern "C" agpu_icd_dispatch agpu_gl_icd_dispatch;

/**
 * AGPU OpenGL object.
 */
template<typename ST>
class Object
{
public:
    Object()
        : dispatch(&agpu_gl_icd_dispatch), refcount(1)
    {
    }

    ~Object()
    {
    }

    agpu_error retain()
    {
        auto old = refcount.fetch_add(1,  std::memory_order_relaxed);
        assert(old > 0 && "the object has to still exist");
        if(old == 0)
            return AGPU_INVALID_OPERATION;
        return AGPU_OK;
    }

    agpu_error release()
    {
        auto old = refcount.fetch_sub(1,  std::memory_order_relaxed);
        assert(old > 0 && "the object has to still exist");
        if(old == 0)
            return AGPU_INVALID_OPERATION;
        else if (old == 1)
        {
            static_cast<ST*> (this)->lostReferences();
            delete static_cast<ST*> (this);
        }
        return AGPU_OK;

    }

private:
    agpu_icd_dispatch *dispatch;
    std::atomic_int refcount;
};

#endif //_AGPU_GL_OBJECT_HPP_

