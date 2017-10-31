#pragma once

#include "boost/variant.hpp"

#include <vector>
#include <string>

struct ClearHistory {};
struct AddLine {
    std::vector<std::string> vec;
    AddLine(std::vector<std::string> vec) : vec(vec) {};
};

struct ChangePrompt {
    std::string new_prompt;
    ChangePrompt(std::string prompt) : new_prompt(prompt) {};
};

struct InsertChar {
     char c;
    InsertChar(char c) : c(c) {};
};

struct SendInput {
    bool dummy;
    SendInput(bool dummy = false) : dummy(dummy) {};
};


using UIEvent = boost::variant<ClearHistory,AddLine,ChangePrompt,InsertChar,SendInput>;
