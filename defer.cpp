#include "defer.h"

Defer::Defer(const std::function<void()> &deferFunc) : deferFunc(deferFunc) {}

Defer::~Defer() {
    if (deferFunc)
        deferFunc();
}
