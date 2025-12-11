//
//  facelib.cpp
//  FaceDetection
//
//  Created by Pavel Palityka on 10.12.25.
//
#include <gtest/gtest.h>
#include <json/json.h>

/* Json validity test */

TEST(JSON, ValidateStructure)
{
    const char* sample = R"({
        "input": "a.jpg",
        "output": "b.jpg",
        "faces": [{"x":1,"y":2,"w":3,"h":4}]
    })";

    Json::Value r;
    Json::CharReaderBuilder b;
    std::string errs;
    std::stringstream ss(sample);

    ASSERT_TRUE(Json::parseFromStream(b, ss, &r, &errs));

    EXPECT_TRUE(r["faces"].isArray());
    EXPECT_EQ(r["faces"][0]["w"].asInt(), 3);
}
