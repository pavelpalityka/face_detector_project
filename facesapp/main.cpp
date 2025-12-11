//
//  main.cpp
//  FaceDetection
//
//  Created by Pavel Palityka on 10.12.25.
//
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <mutex>
#include <vector>
#include <thread>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <random>
#include <json/json.h>

std::string generate_random_string(size_t length) { // unical name
    const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    std::string result;
    for (size_t i = 0; i < length; ++i)
        result += charset[dist(rng)];
    return result;
}

#if defined(_WIN32)
    #include <windows.h>
    using lib_handle = HMODULE;
    inline lib_handle load_lib(const char* name) { return LoadLibraryA(name); }
    inline void close_lib(lib_handle h) { FreeLibrary(h); }
    inline void* load_symbol(lib_handle h, const char* sym) { return (void*)GetProcAddress(h, sym); }

#else
    #include <dlfcn.h>
    using lib_handle = void*;
    inline lib_handle load_lib(const char* name) { return dlopen(name, RTLD_LAZY); }
    inline void close_lib(lib_handle h) { dlclose(h); }
    inline void* load_symbol(lib_handle h, const char* sym) { return dlsym(h, sym); }

#endif

using face_fn = int(*)(const char* input_path, const char* output_path, const char* exe_dir_path,
                       char** out_json_buffer, size_t* out_json_buffer_sz);



int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "Usage faceapp <path_to_folder>" << std::endl;
        return 1;
    }

    std::string rootDir = argv[1];
#ifdef _WIN32
    const char* libName = "faceslib.dll";
#elif defined(__APPLE__)
    const char* libName = "libfaceslib.dylib";
#else
    const char* libName = "libfaceslib.so";
#endif
    
    //Checking the library availability
    std::filesystem::path exePath = std::filesystem::absolute(argv[0]).parent_path();
    std::filesystem::path libPath = exePath / libName;
    if (!std::filesystem::exists(libPath)) {
        std::cout << "Not found faces lib" << std::endl;
        return 1;
    }
    
    //Checking if the path exists and if it is a folder.
    std::filesystem::path rootPath(rootDir);
    if (!std::filesystem::is_directory(rootPath) || !std::filesystem::exists(rootPath))
    {
        std::cout << "Path is not a folder path" << std::endl;
        return 1;
    }
    
    //Checking the model availability
    std::filesystem::path modelPath = exePath / "models" / "face_detection_yunet_2023mar.onnx";
    if (!std::filesystem::exists(modelPath))
    {
        std::cout << "Not found model face_detection_yunet_2023mar.onnx" << std::endl;
        return 1;
    }
    
    // loading the library
    lib_handle handle = load_lib(libPath.c_str());
    if (!handle) {
#if defined(_WIN32)
        std::cerr << "LoadLibrary failed: " << GetLastError() << "\n";
#else
        std::cerr << "dlopen failed: " << dlerror() << "\n";
#endif
        return 1;
    }

    
    auto face_process = (face_fn)load_symbol(handle, "detect_faces");
    if (!face_process) {
        std::cerr << "Failed to load symbol 'face_process'" << std::endl;
        close_lib(handle);
        return 1;
    }

    //Using a thread pool
    boost::asio::thread_pool pool(std::thread::hardware_concurrency());
    std::mutex mtx;
    std::vector<Json::Value> results;



    for (auto& entry : std::filesystem::recursive_directory_iterator(rootPath)) {
        if (!std::filesystem::is_regular_file(entry.path()))
            continue;

        std::string filepath = entry.path().string();
        std::string ext = entry.path().extension().string();
        if (ext != ".jpg" && ext != ".jpeg" && ext != ".png")
            continue;
        
        std::string random_suffix = generate_random_string(3); // For a unique name, if the file names are the same
        std::string outpath = rootPath.string() + "/" +  entry.path().stem().string() + random_suffix + "_blur.jpg";

        boost::asio::post(pool, [&, filepath, exePath, outpath]() {
            char* json_cstr = nullptr;
            size_t json_sz = 0;
            
            int ret = face_process( filepath.c_str(), outpath.c_str(), exePath.string().c_str(),
                &json_cstr, &json_sz
            );

            if (ret != 0 || !json_cstr || json_sz == 0) {
                std::cerr << "Failed to process file: " << filepath << "\n";
                return;
            }

            std::string json_str(json_cstr, json_sz);
            delete[] json_cstr;  

            Json::Value root;
            Json::CharReaderBuilder builder;
            std::string errs;
            std::stringstream ss(json_str);
            Json::parseFromStream(builder, ss, &root, &errs);

            {
                std::lock_guard<std::mutex> lock(mtx);
                results.push_back(root);
            }
        });
    }

    pool.join();


    Json::Value finalJson(Json::arrayValue);
    for (auto& r : results)
        finalJson.append(r);

    std::string out = Json::writeString(Json::StreamWriterBuilder{}, finalJson);
    
    // save json
    std::string outPath = (std::filesystem::path(rootDir) / "result.json").string();
    std::ofstream fstream(outPath);
    fstream<< out;
    fstream.close();

    std::cout << "Saved: " << outPath << "\n";

    close_lib(handle);
    return 0;
}
