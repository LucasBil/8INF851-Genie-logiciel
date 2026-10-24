#include "core/MonSuperSysteme.hpp"
#include "io/UserIOController.hpp"

#include <exception>
#include <iostream>

#include <vector>
#include "core/Matrix.hpp"
#include "core/Cpu.hpp"

int main() {
    /*try {
        mss::UserIOController controller(std::cin, std::cout);
        mss::MonSuperSysteme system;

        system.loadInitialMatrix(controller.readMatrix());
        controller.writeMatrix(system.initialMatrix());
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "Erreur : " << error.what() << '\n';
        return 1;
    }*/
    std::vector<std::vector<double>> matrix = {
    {100,10},
    {45,13}};
    mss::Cpu cpu;
    mss::Matrix matrice(matrix);

    cpu.permuteColumn(matrice);
    cpu.printMatrix(matrice);

    cpu.permuteLine(matrice);
    cpu.printMatrix(matrice);

    cpu.rotate(matrice);
    cpu.printMatrix(matrice);

    cpu.calcDeterminant(matrice);
    cpu.printMatrix(matrice);
    std::cout << "test" << std::endl;
}
