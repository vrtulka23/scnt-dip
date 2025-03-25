#include "nodes.h"

namespace dip {

  void QuantityNode::set_units(Quantity::PointerType units_input) {
    units = nullptr;
    if (units_input==nullptr and !units_raw.empty()) {
      units = std::make_unique<puq::Quantity>(units_raw);
    } else if (units_input!=nullptr) {
      units = std::move(units_input);
    }
  }
  
}
