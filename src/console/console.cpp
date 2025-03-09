// #ifdef __RETRO__

#include "console.hpp"
#include "Events.h"
#include "Fonts.h"
#include "MacMemory.h"
#include "MacTypes.h"
#include "MacWindows.h"
#include "Memory.h"
#include "Menus.h"
#include "Processes.h"
#include "QuickDraw.h"
#include "Quickdraw.h"
#include "QuickdrawText.h"
#include "TextEdit.h"
#include "Threads.h"
#include "Windows.h"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <optional>
#include <stdlib.h>
#include <string>
#include <vector>

WindowPtr window = NULL;
void *wStorage;

std::vector<std::string> lineBuffer = std::vector<std::string>();

void console_setup(void) {
  // don't clobber font settings
  short save_font = qd.thePort->txFont;
  short save_font_size = qd.thePort->txSize;
  short save_font_face = qd.thePort->txFace;
  short save_font_fg = qd.thePort->fgColor;
  short save_font_bg = qd.thePort->bkColor;

  BackColor(blackColor);
  ForeColor(whiteColor);

  TextFont(kFontIDMonaco);
  TextSize(10);
  TextFace(normal);

  FontInfo fi = {0};
  GetFontInfo(&fi);

  auto cell_height = fi.ascent + fi.descent + fi.leading + 1;
  auto font_offset = fi.descent;
  auto cell_width = CharWidth(' ');

  Rect initial_window_bounds = qd.screenBits.bounds;

  InsetRect(&initial_window_bounds, 20, 20);
  initial_window_bounds.top += 40;

  initial_window_bounds.bottom = initial_window_bounds.top + cell_height * 60;
  initial_window_bounds.right = initial_window_bounds.left + cell_width * 120;

  Rect qdR = qd.screenBits.bounds;
  qdR.top += 40;

  InsetRect(&qdR, 5, 5);

  ConstStr255Param title = (ConstStr255Param) "\pPowerBoot 1.0.0";

  wStorage = malloc(1024 * 2);
  WindowPtr win = NewWindow(wStorage, &initial_window_bounds, title, true,
                            documentProc, (WindowPtr)-1, true, 0);

  Rect portRect = win->portRect;

  SetPort(win);
  EraseRect(&portRect);

  window = win;

  BackColor(save_font_bg);
  ForeColor(save_font_fg);

  TextFont(save_font);
  TextSize(save_font_size);
  TextFace(save_font_face);
}

void MacMain() {
  InitGraf(&qd);
  InitFonts();
  InitWindows();
  InitMenus();

  DrawMenuBar();

  console_setup();

  InitCursor();

  BeginUpdate(window);
  ScreenDraw(&(window->portRect));
  EndUpdate(window);

  long int sleep_time = GetCaretTime() / 4;
  bool exit_loop = false;

  EventRecord event;
  WindowPtr eventWin;
  ThreadState mainThreadState;
  long data;

  do {
    SystemTask();
    while (!GetNextEvent(everyEvent, &event)) {
      SystemTask();
      BeginUpdate(window);
      ScreenDraw(&(window->portRect));
      EndUpdate(window);
    }

    // handle any GUI events
    switch (event.what) {
    case updateEvt:
      eventWin = (WindowPtr)event.message;

      BeginUpdate(eventWin);
      ScreenDraw(&(eventWin->portRect));
      EndUpdate(eventWin);
      break;
    case activateEvt:
      GetWRefCon(window);
      break;
    case mouseDown:
      switch (FindWindow(event.where, &eventWin)) {
      case inDrag:
        DragWindow(eventWin, event.where, &(*(GetGrayRgn()))->rgnBBox);
        break;
      case inGrow: {
        long growResult =
            GrowWindow(eventWin, event.where, &(*(GetGrayRgn()))->rgnBBox);
        SizeWindow(eventWin, growResult & 0xFFFF, growResult >> 16, false);
      } break;
      case inGoAway: {
        if (TrackGoAway(eventWin, event.where))
          exit(0);
      } break;
      }

    case osEvt:
      break;
    }

    YieldToAnyThread();
  } while (!exit_loop);

  CloseWindow(window);
  DisposePtr((Ptr)window);
}

void ScreenDraw(Rect *rec) {

  // don't clobber font settings
  short save_font = qd.thePort->txFont;
  short save_font_size = qd.thePort->txSize;
  short save_font_face = qd.thePort->txFace;
  short save_font_fg = qd.thePort->fgColor;
  short save_font_bg = qd.thePort->bkColor;

  TextFont(kFontIDMonaco);
  TextSize(9);
  TextFace(normal);
  BackColor(blackColor);
  ForeColor(whiteColor);
  TextMode(srcOr); // or mode is faster for some common cases

  int select_start = -1;
  int select_end = -1;
  int i = 0;

  EraseRect(rec);

  short y = rec->top + 9;
  std::optional<std::vector<std::string>::iterator> lastLine = {};
  ThreadBeginCritical();
  bool makingColor = false;
  std::string colorCodes = std::string();
  for (auto line = lineBuffer.begin(); line != lineBuffer.end(); line++) {
    short x = rec->left + 9;
    for (int ch = 0; ch < line->size(); ch++) {
      char c = line->at(ch);
      switch (c) {
      case '\e':
        makingColor = true;
        colorCodes.erase(colorCodes.begin());
        break;
      case '\n':
        x = rec->left + 9;
        y += 12;
        lastLine = line;
        break;
      case '\t':
        x += 16;
        x = (x + 8) & (x - 7);
        break;
      case '\r':
        x = rec->left + 9;
        break;
      case 'm':
        if (makingColor) {
          makingColor = false;
          printf("%s\n", colorCodes.c_str());
          handleColorCode(atoi(colorCodes.c_str()));
          break;
        }
        // intentionally don't break here because if we're not in color mode
        // we wanna do default behavior.
      default:
        if (makingColor) {
          if (isdigit(c)) {
            colorCodes.push_back(c);
          }
        } else {
          MoveTo(x, y);
          DrawChar(c);

          Rect r = (Rect){
              .top = y,
              .left = x,
              .bottom = (short)(y + 12),
              .right = (short)(x + CharWidth(ch) + 9),
          };
        }
        break;
      }
      x += CharWidth(ch) + 9;
    }
    lastLine = {};
  }
  InvalRect(rec);

  ThreadEndCritical();

  TextFont(save_font);
  TextSize(save_font_size);
  TextFace(save_font_face);
  qd.thePort->fgColor = save_font_fg;
  qd.thePort->bkColor = save_font_bg;

  // draw the grow icon in the bottom right corner, but not the scroll bars
  // yes, this is really awkward
  MacRegion bottom_right_corner = {10, window->portRect};
  MacRegion *brc = &bottom_right_corner;
  MacRegion **old = window->clipRgn;

  bottom_right_corner.rgnBBox.top = bottom_right_corner.rgnBBox.bottom - 15;
  bottom_right_corner.rgnBBox.left = bottom_right_corner.rgnBBox.right - 15;

  window->clipRgn = &brc;
  DrawGrowIcon(window);
  window->clipRgn = old;
}

void handleColorCode(int code) {
  switch (code) {
  case 30:
    ForeColor(blackColor);
    break;
  case 31:
    ForeColor(redColor);
    break;
  case 32:
    ForeColor(greenColor);
    break;
  case 33:
    ForeColor(yellowColor);
    break;
  case 34:
    ForeColor(blueColor);
    break;
  case 35:
    ForeColor(magentaColor);
    break;
  case 36:
    ForeColor(cyanColor);
    break;
  case 37:
    ForeColor(whiteColor);
    break;
  case 40:
    BackColor(blackColor);
    break;
  case 41:
    BackColor(redColor);
    break;
  case 42:
    BackColor(greenColor);
    break;
  case 43:
    BackColor(yellowColor);
    break;
  case 44:
    BackColor(blueColor);
    break;
  case 45:
    BackColor(magentaColor);
    break;
  case 46:
    BackColor(cyanColor);
    break;
  case 47:
    BackColor(whiteColor);
    break;
  }
}

// #endif