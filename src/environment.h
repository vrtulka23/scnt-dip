#ifndef DIP_ENVIRONMENT_H
#define DIP_ENVIRONMENT_H

#import "lists/sources.h"

namespace dip {

  class Environment {
  private:
  public:
    SourceList sources;
    Environment();
  };

}

#endif
