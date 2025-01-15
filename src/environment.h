#ifndef DIP_ENVIRONMENT_H
#define DIP_ENVIRONMENT_H

#include "lists/node_list.h"
#include "lists/source_list.h"
#include "lists/hierarchy_list.h"

namespace dip {

  class Environment {
  private:
  public:
    SourceList sources;
    NodeList nodes;
    HierarchyList hierarchy;
    Environment();
  };

}

#endif
