#pragma once

#include <functional>

#define defer1(a, b) a##b
#define defer2(a, b) defer1(a, b)
#define defer(deferFunction) Defer defer2(DEFER, __COUNTER__)(deferFunction)

class Defer {
public:
    explicit Defer(const std::function<void()> &deferFunc);

    ~Defer();

private:
    std::function<void()> deferFunc;
};
