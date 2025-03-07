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
#include <string>

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
  TextSize(12);
  TextFace(normal);

  FontInfo fi = {0};
  GetFontInfo(&fi);

  auto cell_height = fi.ascent + fi.descent + fi.leading + 1;
  auto font_offset = fi.descent;
  auto cell_width = CharWidth(' ');

  Rect initial_window_bounds = qd.screenBits.bounds;

  InsetRect(&initial_window_bounds, 20, 20);
  initial_window_bounds.top += 40;

  initial_window_bounds.bottom = initial_window_bounds.top + cell_height * 16;
  initial_window_bounds.right = initial_window_bounds.left + cell_width * 44;

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
      printf("%ld\n", event.message & 0xFFFF);
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
  TextSize(12);
  TextFace(normal);
  BackColor(blackColor);
  ForeColor(whiteColor);
  TextMode(srcOr); // or mode is faster for some common cases

  int select_start = -1;
  int select_end = -1;
  int i = 0;

  EraseRect(rec);

  short y = rec->top + 12;
  ThreadBeginCritical();
  for (auto line = lineBuffer.begin(); line != lineBuffer.end(); line++) {
    short x = rec->left + 12;
    for (int ch = 0; ch < line->size(); ch++) {
      char c = line->at(ch);
      switch (c) {
      case '\n':
        x = rec->left + 12;
        y += 12;
        break;
      default:
        MoveTo(x, y);
        DrawChar(c);
        break;
      }
      x += 12;
    }
  }
  ThreadEndCritical();

  InvalRect(rec);

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