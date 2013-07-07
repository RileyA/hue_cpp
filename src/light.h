#ifndef light_H
#define light_H

#include "hue.h"

namespace hue {
  class hub;
  class light {
  public:

    light(int index, hub* config);

    /** Get the name of this light */
    string get_name();
    void set_name(string new_name);

    int get_index() { return m_index; }
    string get_url() { return m_url; }

  private:

    /** Get json blob for this light */
    Json::Value& get();

    hub* m_config;
    int m_index;
    string m_url;

  };
}

#endif

