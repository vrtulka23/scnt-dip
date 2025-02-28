#include <sstream>
#include <tuple>

#include "nodes.h"

namespace dip {
  
  bool Node::has_dtype(NodeDtype dt) {
    return dtype==dt;
  }
  
  std::string Node::to_string() {
    std::stringstream ss;
    ss << "indent(" << indent << ") ";
    ss << "name(" << name << ") ";
    ss << "value_raw(" << value_raw << ") ";
    ss << "dtype(" << dtype << ") ";
    ss << "dimension(";
    for (auto slice: dimension)
      ss << std::get<0>(slice) << SEPARATOR_SLICE << std::get<1>(slice) << SEPARATOR_DIMENSION;
    if (dimension.size()>0)
      ss.seekp(-1, std::ios_base::end);
    ss << ") ";
    return ss.str();
  }

}
