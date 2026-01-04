#include "GameRunner.hpp"
#include "MapGenerator.hpp"
#include <iostream>

int main() {
    map_generator::CombinationsGenerator some_generator(4,4);
    some_generator.generate();
}