#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "device.hpp"
#include "context.hpp"

_agpu_device:: _agpu_device()
{
    immediateContext = new agpu_context(new AgpuGLImmediateContext(this));
}

// Helper to check for extension string presence.  Adapted from:
//   http://www.opengl.org/resources/features/OGLextensions/
bool _agpu_device::isExtensionSupported(const char *extList, const char *extension)
{
    const char *start;
    const char *where, *terminator;

    /* Extension names should not have spaces. */
    where = strchr(extension, ' ');
    if (where || *extension == '\0')
        return false;

    /* It takes a bit of care to be fool-proof about parsing the
     OpenGL extensions string. Don't be fooled by sub-strings,
     etc. */
    for (start=extList;;) {
        where = strstr(start, extension);

        if (!where)
            break;

        terminator = where + strlen(extension);

        if ( where == start || *(where - 1) == ' ' )
        if ( *terminator == ' ' || *terminator == '\0' )
            return true;

        start = terminator;
    }

    return false;
}

agpu_context* _agpu_device::getImmediateContext()
{
    immediateContext->retain();
    return immediateContext;
}

agpu_context* _agpu_device::createDeferredContext()
{
    return nullptr;
}

void _agpu_device::lostReferences()
{
}

void _agpu_device::readVersionInformation()
{
    int majorVersion, minorVersion;
    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
    versionNumber = OpenGLVersion(majorVersion*10 + minorVersion);
}

AGPU_EXPORT agpu_error agpuAddDeviceReference ( agpu_device *device )
{
    return device->retain();
}

AGPU_EXPORT agpu_error agpuReleaseDevice ( agpu_device *device )
{
    return device->release();
}

AGPU_EXPORT agpu_context* agpuGetImmediateContext ( agpu_device* device )
{
    return device->getImmediateContext();
}

AGPU_EXPORT agpu_context* agpuCreateDeferredContext ( agpu_device* device )
{
    return device->createDeferredContext();
}

AGPU_EXPORT agpu_error agpuSwapBuffers ( agpu_device* device )
{
    return device->swapBuffers();
}

