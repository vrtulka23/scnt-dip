#ifndef DIP_ENVIRONMENT_H
#define DIP_ENVIRONMENT_H

#include "lists/lists.h"

namespace dip {

  class Environment {
  private:
  public:
    SourceList sources;
    NodeList nodes;
    HierarchyList hierarchy;
    BranchingList branching;
    Environment();
  };

}

#endif
