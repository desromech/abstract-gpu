#include <algorithm>
#include <string>
#include <vector>
#include <AGPU/agpu.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(__unix__)
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <dirent.h>
#else
#error unsupported platform
#endif

/**
 * ICD loaded platform info.
 */
class PlatformInfo
{
public:
    PlatformInfo();
    ~PlatformInfo();

    agpu_platform *platform;

#ifdef _WIN32
    HMODULE moduleHandle;
#else
    void *moduleHandle;
#endif
};

PlatformInfo::PlatformInfo()
{
}

PlatformInfo::~PlatformInfo()
{
}

static bool hasBeenLoaded = false;
static std::vector<PlatformInfo*> loadedPlatforms;

template<typename FT>
void dirEntriesDo(const std::string &dirPath, const FT &f)
{
#if defined(_WIN32)
#error unimplemented
#elif defined(__unix__)
    auto dir = opendir(dirPath.c_str());
    if(!dir)
        return;

    struct dirent *entry;
    while((entry = readdir(dir)) != nullptr)
        f(entry->d_name);

    closedir(dir);
#else
#error unsupported platform
#endif
}

static std::string dirname(const std::string &path)
{
    auto lastPos = path.rfind('/');
#if defined(_WIN32)
    auto lastBPos = path.rfind('\\');
    if(lastPos == std::string::npos)
        lastPos = lastBPos;
    else if(lastPos != std::string::npos && lastBPos != std::string::npos)
        lastPos = std::max(lastPos, lastBPos;
#endif
    if(lastPos == std::string::npos)
        return path;

    return path.substr(0, lastPos);
}

static bool isAbsolutePath(const std::string &path)
{
#if defined(_WIN32)
    return path.size() >= 2 && path[2] == ':';
#else
    return path.size() && path[0] == '/';
#endif
}

static bool isFile(const std::string &path)
{
#if defined(_WIN32)
#error unimplemented
#else
    struct stat s;
    auto res = stat(path.c_str(), &s);
    return res == 0 && S_ISREG(s.st_mode);
#endif
}

static std::string joinPath(const std::string &path1, const std::string &path2)
{
    if(isAbsolutePath(path2))
        return path2;
#if defined(_WIN32)
    if(path1.back() == '\\' || path1.back() == '/')
        return path1 + path2;
#else
    if(path1.back() == '/')
        return path1 + path2;
#endif
    return path1 + "/" + path2;
}


static void loadDriver(const std::string &path)
{
#if defined(_WIN32)
#error unimplemented
#else
    int flags = RTLD_NOW | RTLD_LOCAL;
#if defined(__linux__)
    flags |= RTLD_DEEPBIND;
#endif

    // Load only once.
    auto handle = dlopen(path.c_str(), flags | RTLD_NOLOAD );
    if(handle)
        return;

    // Is this a library?
    handle = dlopen(path.c_str(), flags);
    if(!handle)
        return;

    // Try to get the platform defined by the library.
    agpuGetPlatforms_FUN getPlatforms = (agpuGetPlatforms_FUN)dlsym(handle, "agpuGetPlatforms");
    if(!getPlatforms)
    {
        dlclose(handle);
        return;
    }

    // Get the driver platform
    agpu_platform *platform;
    agpu_size platformCount;
    getPlatforms(1, &platform, &platformCount);

    // Ensure there is at least one platform defined.
    if(!platformCount)
    {
        dlclose(handle);
        return;
    }

    // Got the platform, store it.
    auto platformInfo = new PlatformInfo();
    platformInfo->platform = platform;
    platformInfo->moduleHandle = handle;
    loadedPlatforms.push_back(platformInfo);
#endif
}

static void loadDriversInPath(const std::string &folder)
{
    dirEntriesDo(folder, [&](const std::string &fileName) {
        auto fullPath = joinPath(folder, fileName);
        if(isFile(fullPath))
            loadDriver(fullPath);
    });
}

static void loadPlatforms()
{
    // Single driver path
    auto driverPath = getenv("AGPU_DRIVER_PATH");
    if(driverPath)
    {
        loadDriver(driverPath);
        return;
    }

    // Environment variable
    auto envPath = getenv("AGPU_DRIVERS_PATH");
    if(envPath)
        loadDriversInPath(envPath);

    // Library relative path.
#if defined(__APPLE__) || defined(__linux__) || defined(__FreeBSD__)
    Dl_info info;
    if(dladdr((void*)&loadPlatforms, &info))
        loadDriversInPath(joinPath(dirname(info.dli_fname), "AgpuIcd/"));
#endif

    // TODO: Executable relative path
    //loadDriversInPath("AgpuIcd");
    hasBeenLoaded = true;
}

AGPU_EXPORT agpu_error agpuGetPlatforms ( agpu_size numplatforms, agpu_platform** platforms, agpu_size* ret_numplatforms )
{
    if(!platforms)
        return AGPU_NULL_POINTER;

    if(!hasBeenLoaded)
        loadPlatforms();

    size_t retCount = std::min(numplatforms, loadedPlatforms.size());

    // Pass the platforms
    for(size_t i = 0; i < retCount; ++i)
        platforms[i] = loadedPlatforms[i]->platform;

    // Set the rest to null.
    for(size_t i = retCount; i < numplatforms; ++i)
        platforms[i] = nullptr;

    if(ret_numplatforms)
        *ret_numplatforms = retCount;
    return AGPU_OK;
}
