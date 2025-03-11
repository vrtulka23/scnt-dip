#ifndef H_HELPERS
#define H_HELPERS

namespace dip {
  
  // trim whitespace from the beginning and the end of a string
  void trim(std::string& str) {
    auto start = std::find_if_not(str.begin(), str.end(), ::isspace);
    auto end = std::find_if_not(str.rbegin(), str.rend(), ::isspace).base();
    str = (start < end) ? std::string(start, end) : "";
  }

}

#endif // H_HELPERS
