/**
 * @author    Dmitry Selyutin
 * @copyright GNU General Public License v3.0+
 */

#ifndef WINAPPUSAGE_USAGE_HPP
#define WINAPPUSAGE_USAGE_HPP
#include "config.hpp"
#include "stdint.hpp"
#include "WinError.hpp"
namespace winmenu {


/**
 * @brief Singleton class to iterate over most recently used applications.
 * 
 * Since there is no need to create multiple objects of Usage type,
 * it is implemented as singleton. If there is actual need to refresh
 * registry keys and values, update function must be called.
 */
class Usage
{
private: // PRIVATE MEMBERS
  size_t self_count;
  wchar_t** self_name;
  byte** self_buffer;
  size_t* self_buffersize;


public: // STATIC FUNCTIONS
  /**
   * @brief Retrieve singleton as reference.
   */
  static inline Usage*
  instance()
  {
    static Usage* self_instance;
    if (!self_instance)
      self_instance = new Usage;
    return self_instance;
  }


  /**
   * @brief Retrieve Windows version as 32 bit integer.
   */
  static uint32_t
  platform()
  {
    uint16_t major;
    uint16_t minor;
    uint32_t version;
    OSVERSIONINFO osinfo;
    ::memset(&osinfo, 0, sizeof(osinfo));
    osinfo.dwOSVersionInfoSize = sizeof(osinfo);
    ::GetVersionEx(&osinfo);
    major = osinfo.dwMajorVersion;
    minor = osinfo.dwMinorVersion;
    version = static_cast<uint32_t>(major);
    version <<= (sizeof(uint16_t) * CHAR_BIT);
    version |= static_cast<uint32_t>(minor);
    return version;
  }


  /**
   * @brief Retrieve Windows major version as 16 bit integer.
   */
  static inline uint16_t
  platform_major()
  {
    uint32_t version = platform();
    return static_cast<uint16_t>(version >> 16);
  }


  /**
   * @brief Retrieve Windows minor version as 16 bit integer.
   */
  static inline uint16_t
  platform_minor()
  {
    uint32_t version = platform();
    return static_cast<uint16_t>(version);
  }


  /**
   * @brief Perform ROT13 encoding/decoding.
   * 
   * @param str single wide character
   */
  static inline wchar_t
  ROT13(const wchar_t& code)
  {
    if ((code >= 'a') && (code <= 'm'))
      return (code + 13);
    else if ((code >= 'n') && (code <= 'z'))
      return (code - 13);
    else if ((code >= 'A') && (code <= 'M'))
      return (code + 13);
    else if ((code >= 'N') && (code <= 'Z'))
      return (code - 13);
    else return code;
  }


  /**
   * @brief Read binary buffer and import counter and access time.
   * 
   * @param buffer pointer to binary buffer
   * @param size number of byte to read
   * @param counter number of times file was executed
   * @param time last access time in time_t format
   * 
   * If import_data fails, then both counter and time are set to 0.
   */
  static void
  import_data(const byte* buffer,
              const size_t& size,
              uint32_t& counter,
              time_t& time)
  {
    counter = 0;
    time = 0;
    if ((buffer == NULL) || (size == 0))
      return;

    // Determine Windows version.
    size_t offset = 0;
    bool windows7 = false;
    uint16_t major = Usage::platform_major();
    uint16_t minor = Usage::platform_minor();
    windows7 = ((major >= 6) && (minor >= 1));
    offset = (windows7 ? 60 : 8);
    if (size < (offset + 8))
      return;

    // Extract counter and time.
    byte32set set;
    uint64_t hpart;
    uint64_t lpart;
#if (UINT64_MAX == ULONG_MAX)
    const uint64_t ms2ns = 10000000UL;
    const uint64_t diff = 116444736000000000UL;
#else
    const uint64_t ms2ns = 10000000ULL;
    const uint64_t diff = 116444736000000000ULL;
#endif
    buffer += 4;
    for (size_t i = 0; i < 4; ++i)
      set.bytes[i] = buffer[i];
    counter = static_cast<uint32_t>(set.code);
    buffer -= 4;
    buffer += offset;
    for (size_t i = 0; i < 4; ++i)
      set.bytes[i] = buffer[i];
    hpart = static_cast<uint32_t>(set.code);
    buffer += 4;
    for (size_t i = 0; i < 4; ++i)
      set.bytes[i] = buffer[i];
    lpart = static_cast<uint32_t>(set.code);
    time = ((hpart << 32) | lpart);
    // time = static_cast<time_t>((unixtime - diff) / ms2ns); // Unix
  }


  /**
   * @brief Initialize binary buffer with the given counter and time.
   * 
   * @param counter number of times file was executed
   * @param time last access time in Unix format
   * @param buffer pointer to binary buffer
   * @param size number of bytes written
   * 
   * If export_data fails, then both buffer and size are set to 0.
   */
  static void
  export_data(const uint32_t& counter,
              const time_t& time,
              byte** buffer,
              size_t& size)
  {
    buffer = NULL;
    size = 0;
    if ((counter == 0) || (time == 0))
      return;

    // Determine Windows version.
    size_t offset = 0;
    bool windows7 = false;
    const byte* byteptr = NULL;
    uint16_t major = Usage::platform_major();
    uint16_t minor = Usage::platform_minor();
    windows7 = ((major >= 6) && (minor >= 1));
    offset = (windows7 ? 60 : 8);

    //
    byte* iter = NULL;
    size = (offset + 8);
    *buffer = new byte[size];
    const byte* data = NULL;
    ::memset(buffer, 0, size);
    byteptr = reinterpret_cast<const byte*>(counter);
    iter += 4;
    for (size_t i = 0; i < 4; ++i)
      iter[i] = byteptr[i];
    byteptr = reinterpret_cast<const byte*>(time);
    iter -= 4;
    iter += (offset);
    for (size_t i = 0; i < 8; ++i)
      iter[i] = byteptr[i];
  }


public: // CLASS FUNCTIONS
  /**
   * @brief Refresh all data from registry.
   */
  void
  update()
  {
    self_count = 0;
    delete[] self_name;
    delete[] self_buffer;
    delete[] self_buffersize;
    DWORD state = ERROR_SUCCESS;
    
    // Determine registry keys.
    bool windows7 = false;
    uint16_t major = platform_major();
    uint16_t minor = platform_minor();
    windows7 = ((major >= 6) && (minor >= 1));
    const wchar_t* keys[3];
    if (!windows7)
    {
      keys[0] = L"{0D6D4F41-2994-4BA0-8FEF-620E43CD2812}";
      keys[1] = L"{5E6AB780-7743-11CF-A12B-00AA004AE837}";
      keys[2] = L"{75048700-EF1F-11D0-9888-006097DEACF9}";
    }
    else
    {
      keys[0] = L"{CEBFF5CD-ACE2-4F4F-9178-9926F41749EA}";
      keys[1] = L"{F4E57C4B-2036-45F0-A9AB-443BCFE33D9F}";
      keys[2] = NULL;
    }

    // Calculate registry paths.
    std::vector<std::wstring> paths;
    const wchar_t* headstr = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\UserAssist\\";
    const wchar_t* tailstr = L"\\Count\\";
    for (size_t i = 0; i < 3; ++i)
    {
      if (!keys[i])
        break;
      std::wstring str;
      str += headstr;
      str += keys[i];
      str += tailstr;
      paths.push_back(str);
    }

    // Iterate over registry paths.
    std::vector<std::wstring>::const_iterator iter = paths.begin();
    std::vector<std::wstring>::const_iterator tail = paths.end();
    while (iter < tail)
    {
      HKEY handle;
      std::wstring key = *iter;

      // Open registry handle.
      DWORD access = (KEY_READ | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE);
      state = ::RegOpenKeyExW(
        HKEY_CURRENT_USER,
        &key[0],    // address of key to open
        0,          // reserved parameter
        access,     // desired access rights
        &handle);   // address of handle of open key

      // Query registry information.
      DWORD values;
      DWORD maxvaluelen;
      DWORD maxdatalen;
      state = ::RegQueryInfoKey(
        handle,
        NULL,           // buffer for class name
        NULL,           // size of class string
        NULL,           // reserved parameter
        NULL,           // number of subkeys
        NULL,           // longest subkey length
        NULL,           // longest class string 
        &values,        // number of values for this key 
        &maxvaluelen,   // longest value name 
        &maxdatalen,    // longest value data 
        NULL,           // security descriptor 
        NULL);          // last write time
      if (state != ERROR_SUCCESS)
        throw WinError(state);

      // Enumerate registry values.
      self_count = values;
      self_name = new wchar_t*[values];
      self_buffer = new byte*[values];
      self_buffersize = new size_t[values];
      for (DWORD index = 0; index < self_count; ++index)
      {
        // Retrieve necessary data.
        DWORD datalen = 128;
        DWORD valuelen = 64;
        DWORD datatype = REG_BINARY;
        byte* data = NULL;
        wchar_t* value = NULL;
        state = ERROR_MORE_DATA;
        DWORD maxdata;
        while (state == ERROR_MORE_DATA)
        {
          data = new byte[datalen];
          value = new wchar_t[valuelen];
          state = ::RegEnumValueW(
            handle,
            index,         // current index
            value,         // pointer to name
            &valuelen,     // maximal name length
            NULL,          // reserved parameter
            &datatype,     // type of data
            data,          // pointer to buffer
            &datalen);     // maximal buffer length
          if (state == ERROR_SUCCESS)
            break;
          ++datalen;
          ++valuelen;
          if (datalen > maxdata)
            maxdata = datalen;
          delete[] data;
          delete[] value;
        }
        if (state != ERROR_SUCCESS)
          throw WinError(state);

        // Append data to lists.
        self_name[index] = new wchar_t[++valuelen];
        self_name[index][--valuelen] = 0;
        for (DWORD i = 0; i < valuelen; ++i)
          self_name[index][i] = Usage::ROT13(value[i]);
        self_buffer[index] = new byte[datalen];
        ::memcpy(self_buffer[index], data, datalen);
        self_buffersize[index] = datalen;
        delete[] data;
        delete[] value;
      }
      ::RegCloseKey(handle);
      ++iter;
    }
  }


  /**
   * @brief Retrieve count of elements inside registry.
   */
  inline size_t
  size() const
  {
    return self_count;
  }


  /**
   * @brief Retrieve name for the given index.
   * 
   * @WARNING This function doesn't check index leaving it up to user.
   */
  inline const wchar_t*
  name(const size_t& index) const
  {
    return self_name[index];
  }


  /**
   * @brief Retrieve buffer for the given index.
   * 
   * @WARNING This function doesn't check index leaving it up to user.
   */
  inline const byte*
  buffer(const size_t& index) const
  {
    return self_buffer[index];
  }

  /**
   * @brief Retrieve buffer size for the given index.
   * 
   * @WARNING This function doesn't check index leaving it up to user.
   */
  inline size_t
  buffersize(const size_t& index) const
  {
    return self_buffersize[index];
  }


  /**
   * @brief Retrieve counter for the given index.
   * 
   * @WARNING This function doesn't check index leaving it up to user.
   */
  inline int32_t
  counter(const size_t& index) const
  {
    time_t time;
    uint32_t counter;
    const byte* buffer = self_buffer[index];
    const size_t size = self_buffersize[index];
    Usage::import_data(buffer, size, counter, time);
    return counter;
  }


  /**
   * @brief Retrieve time stamp for the given index.
   * 
   * @WARNING This function doesn't check index leaving it up to user.
   */
  inline time_t
  time(const size_t& index,
       FILETIME& filetime) const
  {
    time_t time;
    uint32_t counter;
    const byte* buffer = self_buffer[index];
    const size_t size = self_buffersize[index];
    Usage::import_data(buffer, size, counter, time);
    filetime.dwLowDateTime = static_cast<DWORD>(time >> 16);
    filetime.dwHighDateTime = static_cast<DWORD>(time);
    return time;
  }


public:
  ~Usage()
  {
    delete[] self_name;
    delete[] self_buffer;
    delete[] self_buffersize;
  }
private:
  Usage()
  {
    self_count = 0;
    self_name = NULL;
    self_buffer = NULL;
    self_buffersize = NULL;
    this->update();
  }
  Usage(const Usage&);
  Usage& operator=(const Usage&);
};


} // namespace winmenu
#endif // WINAPPUSAGE_USAGE_HPP
