#ifndef DIP_SETTINGS_H
#define DIP_SETTINGS_H

#include <string>

namespace dip {

  constexpr char SYMBOL_NEWLINE = '\n';
  constexpr std::string_view ROOT_SOURCE = "ROOT";
  constexpr std::string_view FILE_SOURCE = "FILE";
  constexpr std::string_view STRING_SOURCE = "STRING";
  
  struct Source {
    std::string name;
    int line_number;
  };

}

#endif // DIP_SETTINGS_H
