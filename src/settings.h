#ifndef DIP_SETTINGS_H
#define DIP_SETTINGS_H

#include <string>

namespace dip {

  // Source labels
  constexpr std::string_view ROOT_SOURCE    = "ROOT";
  constexpr std::string_view FILE_SOURCE    = "FILE";
  constexpr std::string_view STRING_SOURCE  = "STRING";

  // Parsing separators
  constexpr char SEPARATOR_NEWLINE   = '\n';
  constexpr char SEPARATOR_DIMENSION = ',';
  constexpr char SEPARATOR_SLICE     = ':';
    
  struct Source {
    std::string name;
    int line_number;
  };

  struct Line {
    std::string code;
    Source source;
  };  

  // Forward declarations
  class Environment;
  
}

#endif // DIP_SETTINGS_H
