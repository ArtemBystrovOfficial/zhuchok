#pragma once

#include <memory>

namespace common {

class ConstMap {
    public:
    ConstMap(int w, int h) {}

    virtual bool isWall(int x, int y) = 0;
};

using const_map_ptr = std::unique_ptr<ConstMap>;

class MockMap : public ConstMap {
    public:
    MockMap(int w, int h) : ConstMap(w, h) {}

    bool isWall(int x, int y) override {
        return false;
    }
};

}