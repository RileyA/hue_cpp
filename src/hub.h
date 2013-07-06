#ifndef hub_H
#define hub_H

#include "hue.h"

namespace hue {
  
  /** Represents a single Hue hub, allowing query and control of the
   *    bulbs associated with it. */
  class hub {
  public:

    /** Static function that discovers all hue bridges on your local
     *  network
     *    @return A list of IP addresses */
    static std::list<hub_addr> Discover();

    hub(string ip);
    hub(hub_addr ip);
    ~hub();

  private:

    hub_addr ip;
    
  };
}

#endif

