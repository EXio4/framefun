#include <iostream>
#include <atomic>
#include "conc.h"
#include <Internal/UIEvents.h>

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "Internal/Color.h"
#include "Internal/Drawer.h"
#include "Internal/RawFB.h"
#include "Internal/DoubleBuffering.h"
#include "Internal/FontManager.h"
#include "Internal/Toolkit.h"
#include "Internal/TextScreen.h"

#include "libs/inline_variant.hpp"

using namespace Internal;

void render_thread(Conc::Chan<UIEvent> &evs,Conc::Chan<std::string> &input, std::atomic<bool> &stop) {

    std::shared_ptr<RawFB> rawfb = std::shared_ptr<RawFB>(new RawFB("/dev/fb0"));
    DoubleBuffering fb(rawfb);
    FontManager fonts(fb);
    Toolkit tk(fb);
    TextScreen tm(fb, 120);
    tm.setInputPrompt("def> ");
    while (!stop) {
        fb.clear();
        tm.render();
        fb.refreshScreen();
        UIEvent ev = evs.pop();
        match(ev,
          [&fb,&tm](ClearHistory ch) {
                tm.clear();
          },
          [&fb,&tm](AddLine line) {
                for (auto& l : line.vec) {
                    tm.addLine(l);
                }
          },
          [&fb,&tm](ChangePrompt prompt) {
                tm.setInputPrompt(prompt.new_prompt);
          },
          [&fb,&tm](InsertChar nchar) {
                tm.addInput(nchar.c);
          },
          [&fb,&tm,&input](SendInput sn) {
              if (sn.dummy) {
                  tm.getInput();
              } else {
                  input.push(tm.getInput());
              }
          }
        );
    };
};


void setup_echo(bool enable) {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    if (enable) {
        t.c_lflag |= ICANON;
        t.c_lflag |= ECHO;
        fputs("\e[?25h", stdout); // ?? blinking
    } else {
        t.c_lflag &= ~ICANON;
        t.c_lflag &= ~ECHO;
        fputs("\e[?25l", stdout); // ?? blinking
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}


void process_thread(Conc::Chan<UIEvent> &evs,Conc::Chan<std::string> &input, std::atomic<bool> &stop) {

    setup_echo(false);
    while (!stop) {

        fd_set rfds;
        struct timeval tv;
        int retval;

        /* Watch stdin (fd 0) to see when it has input. */
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);

        /* Wait up to 2 seconds. */
        tv.tv_sec = 0;
        tv.tv_usec = 500000;

        retval = select(1, &rfds, NULL, NULL, &tv);

        if (retval == -1) {
            evs.push(UIEvent(AddLine({"error on process_thread's select"})));
        } else if (retval) {
            char x = getchar();
            if (x == 10 || x == 13) {
                evs.push(UIEvent(SendInput()));
            } else if (x == 3) {
                evs.push(UIEvent(InsertChar('\\')));
                evs.push(UIEvent(InsertChar('C')));
                evs.push(UIEvent(SendInput(true)));
            } else {
                evs.push(UIEvent(InsertChar(x)));
            }
        }
    }
    
    setup_echo(true);

}

int main(int argc, char ** argv){


    Conc::Chan<UIEvent> evs;
    Conc::Chan<std::string> inputs;
    std::atomic<bool> stop;
    stop = false;

    std::thread rendering(render_thread, std::ref(evs), std::ref(inputs), std::ref(stop));
    std::thread processing(process_thread, std::ref(evs), std::ref(inputs), std::ref(stop));

    {
        while (1) {
            std::string cmd = inputs.pop();

            if (cmd == "") continue;

            std::vector<std::string> tx;
            tx.push_back("$ " + cmd);
            if (cmd == "exit") {
                stop = true;
                evs.push(UIEvent(AddLine(tx)));
                break;
            } else if (cmd == "something") {
                tx.push_back("something something");
            } else {
                tx.push_back("invalid command");
            }
            evs.push(UIEvent(AddLine(tx)));
        }
    }
    rendering.join();
    processing.join();
    return 0;
}
