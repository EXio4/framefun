#pragma once

#include <stdexcept>

namespace Internal {

class FramebufferException : public std::runtime_error {
public:
    FramebufferException() : std::runtime_error("FramebufferException") {};
};

}