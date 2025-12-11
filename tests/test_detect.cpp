#include <gtest/gtest.h>
#include <filesystem>
#include <json/json.h>
#include "../faceslib/include/facelib.h"

 /* A test to identify three faces in a photo. You need a photo with three faces called person3.jpg.  */

TEST(FaceDetect, DetectThreeFaces)
{
    std::filesystem::path exeDir = std::filesystem::current_path();
    std::filesystem::path img = exeDir / "person3.jpg";
    std::filesystem::path out = exeDir / "person3_out.jpg";

    ASSERT_TRUE(std::filesystem::exists(img));

    char* json_buf = nullptr;
    size_t json_sz = 0;

    int rc = detect_faces(
        img.string().c_str(),
        out.string().c_str(),
        exeDir.string().c_str(),
        &json_buf,
        &json_sz
    );

    ASSERT_EQ(rc, 0);
    ASSERT_NE(json_buf, nullptr);
    ASSERT_GT(json_sz, 0);

    // --- Parse JSON ---
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;

    {
        std::stringstream ss(std::string(json_buf, json_sz));
        bool ok = Json::parseFromStream(builder, ss, &root, &errs);
        ASSERT_TRUE(ok) << "JSON parse error: " << errs;
    }

    ASSERT_TRUE(root.isMember("faces"));
    ASSERT_TRUE(root["faces"].isArray());

    // -------- HERE: check number of faces --------
    EXPECT_EQ(root["faces"].size(), 3)
        << "Expected exactly 3 faces in test image";

    // cleanup
    delete[] json_buf;
    if (std::filesystem::exists(out))
        std::filesystem::remove(out);
}

