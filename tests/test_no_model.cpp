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

/* Test to check if a model file exists */

TEST(FaceDetect, NoModel)
{
    std::filesystem::path exeDir = std::filesystem::current_path();
    std::filesystem::path modelPath =
        exeDir / "models" / "face_detection_yunet_2023mar.onnx";

    ASSERT_TRUE(std::filesystem::exists(modelPath))
        << "Model not found: " << modelPath;

}
