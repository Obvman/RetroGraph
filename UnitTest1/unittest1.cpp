#include "stdafx.h"
#include "CppUnitTest.h"

#include "../RetroGraph/headers/DriveMeasure.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace rg;

namespace UnitTest1 {        

TEST_CLASS(UnitTest1) {
public:
    TEST_METHOD(DriveMeasure1) {
        DriveMeasure dm{};
        Assert::IsTrue(dm.getNumDrives() > 0); // Should always have at least 1 drives on a PC (C:\)

        const auto& drives{ dm.getDrives() };
    }

    TEST_METHOD(TestMethod2) {
        // TODO: Your test code here
    }

    TEST_METHOD(TestMethod3) {
        // TODO: Your test code here
    }

};

}