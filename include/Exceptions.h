#pragma once

#include <stdexcept>

class FramebufferException : public std::runtime_error {
public:
    FramebufferException() : std::runtime_error("FramebufferException") {};
};
