#include "MacTypes.h"
#include "Quickdraw.h"
#include "Threads.h"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <vector>

#ifndef __CONSOLE_HPP
#define __CONSOLE_HPP

void MacMain();

extern WindowPtr window;
extern std::vector<std::string> lineBuffer;
extern ThreadID main_thread_id;

std::runtime_error formatted_error(const char *format, ...);
void error_throw(const char *format, ...);

void ScreenDraw(Rect *r);

#endif