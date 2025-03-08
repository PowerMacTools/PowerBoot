#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <vector>

#ifndef __CONSOLE_HPP
#define __CONSOLE_HPP

extern std::vector<std::string> lineBuffer;

#ifdef __RETRO__
void MacMain();

#include "MacTypes.h"
#include "Quickdraw.h"
#include "Threads.h"
extern WindowPtr window;
extern ThreadID main_thread_id;
extern ThreadID read_thread_id;
#endif

std::runtime_error formatted_error(const char *format, ...);
void error_throw(const char *format, ...);
void read_error_throw(const char *format, ...);

#ifdef __RETRO__
void ScreenDraw(Rect *r);
void handleColorCode(int code);
#endif

#endif