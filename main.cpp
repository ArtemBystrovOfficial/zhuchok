#include "GameRunner.hpp"
#include "MapGenerator.hpp"
#include <iostream>

int main() {
    std::unique_ptr<common::BaseMap> mp = std::make_unique<common::GenericMap>(2, 2);
    mp->cell(0,1) = common::CellType::Wall;

    int proc = 42;
    int steps = 10'000;
    int steps_mut = 100'000;

    //for(int i=0;i<100;i++) {
    //    map_generator::EvoltuionFlatGenerator ev_gen(std::move(mp),steps); //i>16
    //    mp = ev_gen.generate();
    //}
    //mp->print(true);

    //for(int i=0;i<13;i++) {
    //    map_generator::EvoltuionGenerator ev_gen(std::move(mp),steps,proc,i>8,3); //i>16
    //    mp = ev_gen.generate();
    //}
    //mp->print(true);

    for(int i=0;i<27;i++) {
        map_generator::EvoltuionGenerator ev_gen(std::move(mp),steps,proc, i>16); //i>16
        mp = ev_gen.generate();
    }
    mp->print(true);

    map_generator::MutationGenerator mut_gen(std::move(mp), steps_mut);
    auto out_mp = mut_gen.generate();
    out_mp->print(true);

    //for(int i=0;i<27;i++) {
    //    map_generator::EvoltuionGenerator ev_gen(std::move(mp),steps,proc,i<10); //<10
    //    mp = ev_gen.generate();
    //}
    //mp->print(true);
}