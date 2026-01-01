#pragma once

namespace common {

class ConstMap {
    public:
    ConstMap(int w, int h) {}

    virtual bool isWall(int x, int y) = 0;
};

class MockMap : public ConstMap {
    public:
    MockMap(int w, int h) : ConstMap(w, h) {}

    bool isWall(int x, int y) override {
        return false;
    }
};

}