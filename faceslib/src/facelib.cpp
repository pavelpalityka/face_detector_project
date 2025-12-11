//
//  facelib.cpp
//  FaceDetection
//
//  Created by Pavel Palityka on 10.12.25.
//
#include "facelib.h"
#include <json/json.h>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/face.hpp>


extern "C"
#ifdef _WIN32
__declspec(dllexport)
#endif
int detect_faces( const char* input_path, const char* output_path, const char* exe_dir_path,
    char** out_json_buffer, size_t* out_json_buffer_sz) {
    
    std::filesystem::path exeDir(exe_dir_path);

    std::filesystem::path modelPath = exeDir / "models" / "face_detection_yunet_2023mar.onnx";
    
    //Checking the model availability
    if (!std::filesystem::exists(modelPath)) {
        std::string err = "{\"error\":\"model not found: " +
                           modelPath.string() + "\"}";
        *out_json_buffer_sz = err.size() + 1;
        *out_json_buffer = new char[*out_json_buffer_sz];
        memcpy(*out_json_buffer, err.c_str(), *out_json_buffer_sz);
        std::cout << "model not found" << std::endl;
        return 3;
    }
    
    
    cv::Mat image = cv::imread(input_path);
    if (image.empty()) {
        std::string err = "{\"error\":\"cannot open image\"}";
        *out_json_buffer_sz = err.size() + 1;
        *out_json_buffer = new char[*out_json_buffer_sz];
        memcpy(*out_json_buffer, err.c_str(), *out_json_buffer_sz);
        std::cout << "cannot open image" << std::endl;
        return 1;
    }

    cv::Mat resized;
    cv::resize(image, resized, cv::Size(), 0.5, 0.5);

    // create detector
    cv::Ptr<cv::FaceDetectorYN> detector = cv::FaceDetectorYN::create(modelPath.string(),
                                   "", cv::Size(320, 320));

    detector->setInputSize(resized.size());

    cv::Mat faces;
    detector->detect(resized, faces);

    for (int i = 0; i < faces.rows; i++) {
        cv::Rect box(
            faces.at<float>(i, 0) ,
            faces.at<float>(i, 1) ,
            faces.at<float>(i, 2) ,
            faces.at<float>(i, 3)
        );

        cv::GaussianBlur(resized(box), resized(box), cv::Size(55, 55), 0);
    }


    if (!cv::imwrite(output_path, resized)) {
        std::cout << "cannot save image" << std::endl;
        return 2;
    }

    // Create json
    Json::Value root;
    root["input"] = input_path;
    root["output"] = output_path;

    for (int i = 0; i < faces.rows; i++) {
        Json::Value face_json;
        face_json["x"] = faces.at<float>(i, 0) * 2;
        face_json["y"] = faces.at<float>(i, 1) * 2;
        face_json["w"] = faces.at<float>(i, 2) * 2;
        face_json["h"] = faces.at<float>(i, 3) * 2;
        root["faces"].append(face_json);
    }


    Json::StreamWriterBuilder builder;
    std::string json = Json::writeString(builder, root);


    std::cout << "[OK] processed: " << input_path
              << " faces=" << faces.rows << std::endl;

    // write json in out_json_buffer
    *out_json_buffer_sz = json.size() + 1;
    *out_json_buffer = new char[*out_json_buffer_sz];
    memcpy(*out_json_buffer, json.c_str(), *out_json_buffer_sz);
    return 0;
}
