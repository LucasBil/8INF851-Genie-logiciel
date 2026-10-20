#include "io/UserIOController.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <stdexcept>
#include <string>

TEST(UserIOControllerTest, ReadsAndWritesAMatrix) {
    std::istringstream input("2 2 1 2 3 4");
    std::ostringstream output;
    mss::UserIOController controller(input, output);

    const mss::Matrix matrix = controller.readMatrix();
    controller.writeMatrix(matrix);

    EXPECT_DOUBLE_EQ(matrix.at(1, 1), 4.0);
    EXPECT_NE(output.str().find("2 x 2\n1 2\n3 4\n"), std::string::npos);
}

TEST(UserIOControllerTest, RejectsInvalidDimensionsAndIncompleteInput) {
    std::istringstream invalidDimensions("0 2");
    std::ostringstream firstOutput;
    mss::UserIOController firstController(invalidDimensions, firstOutput);
    EXPECT_THROW(firstController.readMatrix(), std::invalid_argument);

    std::istringstream negativeDimensions("-1 2");
    std::ostringstream negativeOutput;
    mss::UserIOController negativeController(negativeDimensions, negativeOutput);
    EXPECT_THROW(negativeController.readMatrix(), std::invalid_argument);

    std::istringstream incomplete("2 2 1 2");
    std::ostringstream secondOutput;
    mss::UserIOController secondController(incomplete, secondOutput);
    EXPECT_THROW(secondController.readMatrix(), std::runtime_error);
}

TEST(UserIOControllerTest, RejectsInvalidDimensionInput) {
    std::istringstream input("deux trois");
    std::ostringstream output;
    mss::UserIOController controller(input, output);

    EXPECT_THROW(controller.readMatrix(), std::runtime_error);
}
