#pragma once

namespace rg {

class Widget {
public:
    virtual ~Widget() = default;
    virtual void draw() const = 0;
};

}
