#include "defer.h"

Defer::Defer(std::function<void()> deferFunc) : deferFunc(std::move(deferFunc)) {}

Defer::~Defer() {
    if (deferFunc)
        deferFunc();
}
