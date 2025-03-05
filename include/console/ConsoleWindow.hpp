#ifdef __RETRO__
/*
    Copyright 2012-2020 Wolfgang Thaller, Davide Bucci

    This file is part of Retro68.

    Retro68 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Retro68 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    Under Section 7 of GPL version 3, you are granted additional
    permissions described in the GCC Runtime Library Exception, version
    3.1, as published by the Free Software Foundation.

    You should have received a copy of the GNU General Public License
    along with Retro68.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Windows.h>
#include <string>
#include <vector>

#include "Console.hpp"

namespace retro {
class ConsoleWindow : public Console {
public:
  ConsoleWindow(Rect r, ConstStr255Param title);
  ~ConsoleWindow();
  void setWindowName(std::string newName);

private:
  WindowPtr win;

  virtual char WaitNextChar();
};
} // namespace retro
#endif