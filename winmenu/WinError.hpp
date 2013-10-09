/**
 * @author    Dmitry Selyutin
 * @copyright GNU General Public License v3.0+
 */


#ifndef WINAPPUSAGE_WINERROR_HPP
#define WINAPPUSAGE_WINERROR_HPP
namespace winmenu {


class WinError: public std::runtime_error
{
private:
  DWORD self_code;
  char* self_message;


public:
  virtual ~WinError() throw()
  {
    ::free(self_message);
  }


  WinError(const char* message)
  : std::runtime_error(NULL)
  {
    self_code = 0;
    if (message)
      self_message = ::strdup(message);
    else
      self_message = NULL;
  }


  WinError(const DWORD code)
  : std::runtime_error(NULL)
  {
    self_code = code;
    self_message = NULL;
  }


  virtual const char*
  what() const throw()
  {
    std::string stack;
    char* buffer = NULL;
    DWORD code = self_code;
    DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER;
    flags |= FORMAT_MESSAGE_IGNORE_INSERTS;
    flags |= FORMAT_MESSAGE_FROM_SYSTEM;
    WORD lang = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
    size_t len = ::FormatMessageA(
      flags,     // formatting options
      NULL,      // message source
      code,      // error code
      lang,      // language id
      buffer,    // pointer to buffer
      0,         // size of buffer
      NULL);     // special parameters
    if (len == 0)
      stack += "WindowsError";
    else
    {
      while ((len > 0) && ((buffer[len-1] <= ' ') || (buffer[len-1] == '.')))
        buffer[--len] = '\0';
      stack += buffer;
      ::LocalFree(buffer);
    }
    return stack.c_str();
  }
};


} // namespace winmenu
#endif // WINAPPUSAGE_WINERROR_HPP
