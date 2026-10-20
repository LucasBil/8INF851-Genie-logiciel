#include "core/MonSuperSysteme.hpp"
#include "io/UserIOController.hpp"

#include <exception>
#include <iostream>

int main() {
    try {
        mss::UserIOController controller(std::cin, std::cout);
        mss::MonSuperSysteme system;

        system.loadInitialMatrix(controller.readMatrix());
        controller.writeMatrix(system.initialMatrix());
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "Erreur : " << error.what() << '\n';
        return 1;
    }
}
