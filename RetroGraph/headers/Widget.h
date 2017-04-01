#pragma once

namespace rg {

class Widget {
public:
    Widget() = default;
    virtual ~Widget() = default;
    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;

    virtual void draw() const = 0;
};

}
