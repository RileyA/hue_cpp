#ifndef light_H
#define light_H

#include "hue.h"

namespace hue {
  class hub;
  class light {
  public:
      
    struct state_change {
      Json::Value obj;
      state_change() {}
      void set_on(bool on) { obj["on"] = on; }
      void set_bri(uint8_t bri) { obj["bri"] = bri; }
      void set_sat(uint8_t sat) { obj["sat"] = sat; }
      void set_hue(uint16_t hue) { obj["hue"] = hue; }
      void set_ct(uint16_t ct) { obj["ct"] = ct; }
      void set_transitiontime(uint16_t tt) { obj["transitiontime"] = tt; }
      void set_alert_none() { obj["alert"] = "none"; }
      void set_alert_select() { obj["alert"] = "select"; }
      void set_alert_lselect() { obj["alert"] = "lselect"; }
      void set_alert(string alert) { obj["alert"] = alert; }
      void set_effect_none() { obj["effect"] = "none"; }
      void set_effect_colorloop() { obj["effect"] = "colorloop"; }
      void set_effect(string effect) { obj["effect"] = effect; }
      void set_xy(float x, float y) {
        obj["xy"] = Json::Value(Json::arrayValue);
        obj["xy"][0] = x; obj["xy"][1] = y;
      }
    };

    light(int index, hub* config);

    /** Get the name of this light */
    string get_name();
    void set_name(string new_name);

    /** Set this light's state
     *    @param state What parameters we'll change 
     *    @param deduplicate Whether or not to prune params that are the
     *      same as what we already have (may be out of date!). */
    void set_state(state_change state, bool deduplicate = false);

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

