//
//  facelib.cpp
//  FaceDetection
//
//  Created by Pavel Palityka on 10.12.25.
//
#include <gtest/gtest.h>
#include <cstdlib>
#include <array>
#include <memory>

/* Testing the app's operation with an incorrect folder path */

static std::string execCmd(const std::string& cmd)
{
    std::array<char, 128> buffer;
    std::string result;

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe)
        return result;

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        result += buffer.data();

    return result;
}

TEST(FacesApp, WrongPath)
{
    
    std::string cmd = "./facesapp /temp///temp/temp";

    std::string output = execCmd(cmd);
    int ret = system(cmd.c_str());

    EXPECT_NE(ret, 0);
}
