#include "GameRunner.hpp"
#include "MapGenerator.hpp"
#include <iostream>

int main() {
    map_generator::CombinationsGenerator some_generator(3,3);
    some_generator.generate();
}