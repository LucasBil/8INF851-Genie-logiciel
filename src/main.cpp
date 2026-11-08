#include "core/MonSuperSysteme.hpp"
#include "io/UserInputController.hpp"
#include "io/UserOutputController.hpp"
#include "serializer/SerializerTool.hpp"

#include <exception>
#include <iostream>

int main() {
    try {
        mss::SerializerTool serializer;
        mss::MonSuperSysteme system;
        mss::UserOutputController output({}, std::cin, std::cout);
        mss::UserInputController input(serializer, output, system, std::cin, std::cout);

        input.run();
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "Erreur : " << error.what() << '\n';
        return 1;
    }
}
