#ifndef hub_H
#define hub_H

#include "hue.h"
#include "light.h"

namespace hue {

  typedef bool (*hub_auth_callback)();
  
  /** Represents a single Hue hub, allowing query and control of the
   *    bulbs associated with it. */
  class hub {
  public:

    hub(string ip, string device = "hue_cpp", string username = "hue_cpp",
      hub_auth_callback callback = await_hub_auth);
    hub(hub_addr ip, string device = "hue_cpp", string username = "hue_cpp",
      hub_auth_callback callback = await_hub_auth);
    virtual ~hub();

    /** Get a light by index
     *    @return NULL if no such light */
    light* get_light(int index);

    /** Get a light by name
     *    @return NULL if no such light */
    light* get_light(string name);

    /** Discover new lights (this initiates a search, it'll take up to a minute
     *  for the process to complete, and only 15 lights may be discovered in
     *  one call to this). */
    void discover_new_lights();

    /** Get new lights that were discovered last time we ran a scan
     *    @return a pair of: (scan status, list of new lights) */
    std::pair<string, std::list<light*> > query_new_lights();

    /** Refresh status for a single light */
    void refresh_light(int index);

    /** Refresh our view of the hub's state entirely (expensive! use
     *  sparingly!) */
    void refresh_state();

    string get_ip() { return m_ip_str; }
    string get_device() { return m_device; }
    string get_username() { return m_username; }

    /** Static function that discovers all hue bridges on your local
     *  network
     *    @return A list of IP addresses */
    static std::list<hub_addr> Discover();

    /** A default callback for waiting for the hub button to be pressed
     *    @return Whether the user pressed it (hopefully), or cancelled
     *      the action */
    static bool await_hub_auth();

  private:

    friend class light;

    /** Make sure we have a username set up with the hub (this will
     *  potentially stall until you press the hub button thingy...) */
    bool init_username(hub_auth_callback callback);

    /** Get complete hub status, returns whether or not we're authenticated
     *  properly (this happens on startup to ensure we're auth'd) */
    bool get_status();

    /** Try to initialize a new light */
    void init_light(int index, bool fetch = false);

    // IP address of this hub
    hub_addr m_ip;
    string m_ip_str;

    // devicetype/username for hub authentication
    string m_device;
    string m_username;

    // most up to date status of the hub we have (not guaranteed to be
    // up-to-date if other apps are meddling with things...)
    Json::Value m_status;

    // curl context we'll use for operations pertaining to this hub
    CURL* m_ctx;

    std::map<int, light*> m_lights;
    std::map<string, light*> m_light_names;
    
  };
}

#endif

