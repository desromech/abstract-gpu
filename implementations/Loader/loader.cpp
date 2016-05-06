#include <algorithm>
#include <string>
#include <vector>
#include <AGPU/agpu.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef max
#undef min
#elif defined(__unix__) || defined(__APPLE__)
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <dirent.h>
#else
#error unsupported platform
#endif

#ifdef HAS_XLIB
#include <X11/Xlib.h>

struct CallXInitThreads
{
    CallXInitThreads()
    {
        XInitThreads();
    }
} callXInitThreads;

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

    agpu_bool hasRealMultithreading;
    agpu_bool isNative;
    agpu_bool isCrossPlatform;

};

PlatformInfo::PlatformInfo()
{
}

PlatformInfo::~PlatformInfo()
{
}

static bool hasBeenLoaded = false;
static std::vector<PlatformInfo*> loadedPlatforms;

std::wstring utf8ToUtf16(const std::string &utf8)
{
    int c = 0;
    size_t position = 0;
    std::wstring utf16;
    while (position < utf8.size())
    {
        int startChar = utf8[position++];
        if ((startChar & 0x80) == 0)
        {
            utf16.push_back(startChar);
            continue;
        }

        int charSize = 1;
        while ((startChar & (1 << (7 - charSize))) != 0)
        {
            charSize++;
        }

        --charSize;
        if (size_t(position + charSize) > utf8.size())
            break;

        printf("char size %d\n", charSize);
    }

    return utf16;
}

std::string utf16ToUtf8(const std::wstring &utf16)
{
    int c = 0;
    size_t position = 0;
    std::string utf8;
    while (position < utf16.size())
    {
        int startChar = utf16[position++];
        if (startChar < 128)
        {
            utf8.push_back(startChar);
            continue;
        }

        abort();
    }

    return utf8;
}
template<typename FT>
void dirEntriesDo(const std::string &dirPath, const FT &f)
{
#if defined(_WIN32)
    auto dirPathUtf16 = utf8ToUtf16(dirPath) + L"*";
    WIN32_FIND_DATAW findData;
    auto handle = FindFirstFileW(dirPathUtf16.c_str(), &findData);
    if (handle == INVALID_HANDLE_VALUE)
        return;

    f(utf16ToUtf8(findData.cFileName));
    while(FindNextFileW(handle, &findData))
        f(utf16ToUtf8(findData.cFileName));

    FindClose(handle);
#elif defined(__unix__) || defined(__APPLE__)
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
        lastPos = std::max(lastPos, lastBPos);
#endif
    if(lastPos == std::string::npos)
        return path;

    return path.substr(0, lastPos);
}

static std::string extension(const std::string &path)
{
    auto dotPos = path.rfind('.');
    if (dotPos == std::string::npos)
        return std::string();
    return path.substr(dotPos + 1);
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
    auto pathUtf16 = utf8ToUtf16(path);
    auto attributes = GetFileAttributesW(pathUtf16.c_str());
    return (attributes != INVALID_FILE_ATTRIBUTES) && ((attributes & FILE_ATTRIBUTE_DIRECTORY) == 0);
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
    return path1 + "\\" + path2;
#else
    if(path1.back() == '/')
        return path1 + path2;
#endif
    return path1 + "/" + path2;
}


static void loadDriver(const std::string &path)
{
#if defined(_WIN32)
    auto ext = extension(path);
    if (ext != "dll")
        return;

    auto pathUtf16 = utf8ToUtf16(path);
    auto handle = LoadLibraryW(pathUtf16.c_str());
    if (handle == NULL)
        return;

    // Try to get the platform defined by the library.
    agpuGetPlatforms_FUN getPlatforms = (agpuGetPlatforms_FUN)GetProcAddress(handle, "agpuGetPlatforms");
    if (!getPlatforms)
    {
        FreeLibrary(handle);
        return;
    }

    // Get the driver platform count.
    agpu_size platformCount;
    getPlatforms(0, nullptr, &platformCount);

    // Get the driver platforms
    std::vector<agpu_platform*> platforms(platformCount);
    getPlatforms(platforms.size(), &platforms[0], &platformCount);

    // Ensure there is at least one platform defined.
    if (!platformCount)
    {
        FreeLibrary(handle);
        return;
    }

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
    {
        fprintf(stderr, "Failed to load %s: %s\n", path.c_str(), dlerror());
        return;
    }

    // Try to get the platform defined by the library.
    agpuGetPlatforms_FUN getPlatforms = (agpuGetPlatforms_FUN)dlsym(handle, "agpuGetPlatforms");
    if(!getPlatforms)
    {
        dlclose(handle);
        return;
    }

    // Get the driver platform count.
    agpu_size platformCount;
    getPlatforms(0, nullptr, &platformCount);

    // Get the driver platforms
    std::vector<agpu_platform*> platforms(platformCount);
    getPlatforms(platforms.size(), &platforms[0], &platformCount);

    // Ensure there is at least one platform defined.
    if(!platformCount)
    {
        dlclose(handle);
        return;
    }

#endif

    // Got the platforms, store them.
    for (agpu_size i = 0; i < platformCount; ++i)
    {
        auto platform = platforms[i];
        auto platformInfo = new PlatformInfo();
        platformInfo->platform = platform;
        platformInfo->moduleHandle = handle;
        platformInfo->isNative = agpuIsNativePlatform(platform);
        platformInfo->hasRealMultithreading = agpuPlatformHasRealMultithreading(platform);
        platformInfo->isCrossPlatform = agpuIsCrossPlatform(platform);
        loadedPlatforms.push_back(platformInfo);
    }
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
#elif defined(_WIN32)
    HMODULE handle;
    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)&loadPlatforms, &handle))
    {
        WCHAR modulePathBuffer[MAX_PATH];
        auto pathLength = GetModuleFileNameW(handle, modulePathBuffer, MAX_PATH);
        if (pathLength != MAX_PATH && pathLength != 0)
        {
            std::wstring modulePath(modulePathBuffer, modulePathBuffer + pathLength);
            loadDriversInPath(joinPath(dirname(utf16ToUtf8(modulePath)), "AgpuIcd\\"));
        }
    }
#endif

    // TODO: Executable relative path
    //loadDriversInPath("AgpuIcd");
    hasBeenLoaded = true;

    // Sort the platforms. Keep the relative orders given by a driver.
    std::stable_sort(loadedPlatforms.begin(), loadedPlatforms.end(), [](PlatformInfo *a, PlatformInfo *b) {
        if (a->hasRealMultithreading == b->hasRealMultithreading)
        {
            if (a->isNative == b->isNative)
                return a->isCrossPlatform > b->isCrossPlatform;
            return a->isNative > b->isNative;
        }

        return a->hasRealMultithreading > b->hasRealMultithreading;
    });
}

AGPU_EXPORT agpu_error agpuGetPlatforms ( agpu_size numplatforms, agpu_platform** platforms, agpu_size* ret_numplatforms )
{
    if (!hasBeenLoaded)
        loadPlatforms();

    if (!platforms && numplatforms == 0)
    {
        if (ret_numplatforms != nullptr)
        {
            *ret_numplatforms = loadedPlatforms.size();
            return AGPU_OK;
        }
        else
            return AGPU_NULL_POINTER;
    }

    size_t retCount = std::min(numplatforms, (agpu_size)loadedPlatforms.size());

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
