#include "host.hpp"
#include <iostream>
#include <stdexcept>

int main() {
    std::cout << "Игра начинается" << std::endl << std::endl;
    Host host;
    try {
        host.start_game();
    } catch (const std::runtime_error &err) {
        std::cout << "ERROR: " << err.what() << std::endl;
        return 1;
    }
    std::cout << "Конец игры" << std::endl;
    return 0;
}