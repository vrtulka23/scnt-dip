#include <sstream>
#include <iostream>

#include "hierarchy_list.h"

namespace dip {

  void HierarchyList::record(std::shared_ptr<BaseNode> node, const std::vector<Node::NodeKeyword>& excluded) {
    if (node->name=="")
      return;
    for (auto kwd: excluded)
      if (node->keyword==kwd)
	return;

    // closed children nodes and register new parent
    while (parents.size()>0 and node->indent<=parents.back().indent) {
      parents.pop_back();
    }
    parents.push_back({node->indent, node->name});
    
    // set node name according to the hierarchy
    if (parents.size()>1) {
      std::stringstream ss;    
      for (auto parent: parents)
	ss << parent.name << SIGN_SEPARATOR;
      node->name = ss.str();
      node->name = node->name.substr(0, node->name.size()-1);
    }
  }
  
}
