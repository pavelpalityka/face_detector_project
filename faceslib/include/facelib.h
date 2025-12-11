#pragma once
#include <cstddef>

// C ABI for face detector plugin.

#if defined(_MSC_VER)
#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)
#elif defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
#define EXPORT __attribute__((visibility("default")))
#define IMPORT
#else
static_assert(true, "Building with unknown compiler")
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Returns:
 *   0 — OK
 *   1 — failed to load input image
 *   2 — failed to save output image
 *   3 — not found model
 
 */

EXPORT int detect_faces(
    const char* input_path, // original path to the photo
    const char* output_path, // path to save the reduced copy
    const char* exe_dir_path, // path where the application is located to find the model file
    char** out_json_buffer, // json will be written to this array
    size_t* out_json_buffer_sz // array size
);

#ifdef __cplusplus
}
#endif
