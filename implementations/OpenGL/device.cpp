#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "device.hpp"

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

void _agpu_device::lostReferences()
{
}


AGPU_EXPORT agpu_error agpuAddDeviceReference ( agpu_device *device )
{
    return device->retain();
}

AGPU_EXPORT agpu_error agpuReleaseDevice ( agpu_device *device )
{
    return device->release();
}
