#include "lists.h"

namespace dip {

  bool BranchingList::false_case() {
    if (state.size()==0)
      return false;
    //std::string branch = _get_branch_id();
    // TODO: implement branching
    return false;
  }

  void BranchingList::solve_case(std::shared_ptr<BaseNode> node) {
    // TODO: implement
  }
  
  void BranchingList::prepare_node(std::shared_ptr<BaseNode> node) {
    // TODO: implement
  }

}
