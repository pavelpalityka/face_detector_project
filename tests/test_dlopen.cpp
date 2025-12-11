//
//  facelib.cpp
//  FaceDetection
//
//  Created by Pavel Palityka on 10.12.25.
//
#include <gtest/gtest.h>
#include <filesystem>
#include <json/json.h>
#include "../faceslib/include/facelib.h"
#if defined(_WIN32)
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

/* Library loading check test */

TEST(DLOPEN, LoadDetectFaces)
{
#if defined(__APPLE__)
    const char* libname = "libfaceslib.dylib";
#elif defined(_WIN32)
    const char* libname = "faceslib.dll";
#else
    const char* libname = "libfaceslib.so";
#endif

    std::filesystem::path exeDir = std::filesystem::current_path();
    std::filesystem::path libPath = exeDir / libname;

    ASSERT_TRUE(std::filesystem::exists(libPath));

    void* handle = dlopen(libPath.c_str(), RTLD_LAZY);
    ASSERT_NE(handle, nullptr);

    using detect_fn = int(*)(const char*, const char*, const char*, char**, size_t*);
    detect_fn f = (detect_fn)dlsym(handle, "detect_faces");
    ASSERT_NE(f, nullptr);

    dlclose(handle);
}
