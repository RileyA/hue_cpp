#ifndef hub_H
#define hub_H

#include "hue.h"

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

    /** Static function that discovers all hue bridges on your local
     *  network
     *    @return A list of IP addresses */
    static std::list<hub_addr> Discover();

    /** A default callback for waiting for the hub button to be pressed
     *    @return Whether the user pressed it (hopefully), or cancelled
     *      the action */
    static bool await_hub_auth();

  private:

    /** Make sure we have a username set up with the hub (this will
     *  potentially stall until you press the hub button thingy...) */
    bool init_username(hub_auth_callback callback);

    /** Get complete hub status, returns whether or not we're authenticated
     *  properly (this happens on startup to ensure we're auth'd) */
    bool get_status();

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
    
  };
}

#endif

