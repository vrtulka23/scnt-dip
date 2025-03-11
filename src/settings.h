#ifndef DIP_SETTINGS_H
#define DIP_SETTINGS_H

#include <string>
#include <sstream>

namespace dip {

  // Source labels
  constexpr std::string_view ROOT_SOURCE    = "ROOT";
  constexpr std::string_view FILE_SOURCE    = "FILE";
  constexpr std::string_view STRING_SOURCE  = "STRING";

  // Parsing separators
  constexpr char SEPARATOR_NEWLINE                  = '\n';
  constexpr char SEPARATOR_DIMENSION                = ',';
  constexpr char SEPARATOR_SLICE                    = ':';
  constexpr char SEPARATOR_TABLE_COLUMNS            = ' ';
  constexpr std::string_view SEPARATOR_TABLE_HEADER = "---";
				        
  constexpr char SIGN_QUERY             = '?';
  constexpr char SIGN_WILDCARD          = '*';
  constexpr char SIGN_NEGATE            = '~';
  constexpr char SIGN_DEFINED           = '!';
  constexpr char SIGN_SEPARATOR         = '.';
  constexpr char SIGN_CONDITION         = '@';
  constexpr char SIGN_VARIABLE          = '$';
  constexpr char SIGN_VALIDATION        = '!';
  constexpr char SIGN_NEWLINE           = '\n';
  constexpr std::string_view SIGN_BLOCK = "\"\"\"";

  // Keywords
  constexpr std::string_view KEYWORD_TRUE        = "true";
  constexpr std::string_view KEYWORD_FALSE       = "false";
  constexpr std::string_view KEYWORD_CONSTANT    = "constant";
  constexpr std::string_view KEYWORD_DESCRIPTION = "description";
  constexpr std::string_view KEYWORD_FORMAT      = "format";
  constexpr std::string_view KEYWORD_TAGS        = "tags";
  constexpr std::string_view KEYWORD_OPTIONS     = "options";
  constexpr std::string_view KEYWORD_CASE        = "case";
  constexpr std::string_view KEYWORD_ELSE        = "else";
  constexpr std::string_view KEYWORD_END         = "end";
  
  // Various settings
  constexpr int DISPLAY_FLOAT_PRECISION = 4;
  
  struct Source {
    std::string name;
    int line_number;
  };

  struct Line {
    std::string code;
    Source source;
    std::string to_string() {
      std::ostringstream oss;
      oss << "[" << source.name << ":" << source.line_number << "] " << code;
      return oss.str();
    };
  };  

  // Forward declarations
  class Environment;
  
}

#endif // DIP_SETTINGS_H
