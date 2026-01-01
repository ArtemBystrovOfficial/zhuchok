#include "GameRunner.hpp"
#include <MapGenerator.hpp>
#include <iostream>

int main() {
    map_generator::SomeGenerator generator;
    generator.generate(5,6);
}