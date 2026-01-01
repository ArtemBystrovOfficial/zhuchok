#include "GameRunner.hpp"
#include "MapGenerator.hpp"

int main() {
    map_generator::CombinationsGenerator some_generator(3,3);
    some_generator.generate();
}
