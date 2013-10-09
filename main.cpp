/**
 * @author    Dmitry Selyutin
 * @copyright GNU General Public License v3.0+
 */

#include "winmenu.hpp"

int
main(int argc, const char** argv)
{
  time_t time;
  tm* timeinfo;
  FILETIME ft;
  SYSTEMTIME st;
  wchar_t buffer[255];
  timeinfo = ::localtime(&time);
  winmenu::Usage* usage = winmenu::Usage::instance();
  const size_t size = usage->size();
  for (size_t i = 0; i < size; ++i)
  {
    bool state;
    std::wcout << L"File:    " << usage->name(i) << '\n';
    std::wcout << L"Counter: " << usage->counter(i) << '\n';
    time = usage->time(i, ft);
    ::FileTimeToSystemTime(&ft, &st);
    state = ::GetDateFormatW(
      LOCALE_INVARIANT,
      DATE_LONGDATE,
      &st,
      NULL,
      buffer,
      255);
    if (state)
      std::wcout << L"Time:    " << buffer << '\n'; 
    else
      std::wcout << L"Time:    " << L"INCORRECT TIME STAMP" << '\n';
    std::wcout << std::endl;
  }
  return 0;
}
