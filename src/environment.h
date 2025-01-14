#ifndef DIP_ENVIRONMENT_H
#define DIP_ENVIRONMENT_H

#include "lists/nodes.h"
#include "lists/sources.h"

namespace dip {

  class Environment {
  private:
  public:
    SourceList sources;
    NodeList nodes;
    Environment();
  };

}

#endif
