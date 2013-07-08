#include "hub.h"
#include <sstream>

namespace hue {

  hub::hub(string ip, string device, string username,
    hub_auth_callback callback)
    : hub(str_to_addr(ip), device, username, callback) {
  }
  //-------------------------------------------------------------------------

  hub::hub(hub_addr ip, string device, string username,
   hub_auth_callback callback)
    : m_ip(ip)
    , m_username(username)
    , m_device(device)
    , m_ctx(0) {
    m_ctx = curl_easy_init();
    m_ip_str = addr_to_string(m_ip);
    if (!m_ctx) {
      throw hue_exception("curl init failed.");
    }
    if (!init_username(callback)) {
      throw hue_exception("Failed authentication with hub.");
    }
    std::cout << "Hub authentication success!\n";
  }
  //-------------------------------------------------------------------------

  hub::~hub() {
    curl_easy_cleanup(m_ctx);
  }
  //-------------------------------------------------------------------------

  light* hub::get_light(int index) {
    if (m_lights.count(index)) {
      return m_lights[index];
    } else {
      return 0;
    }
  }
  //-------------------------------------------------------------------------

  light* hub::get_light(string name) {
    if (m_light_names.count(name)) {
      return m_light_names[name];
    } else {
      return 0;
    }
  }
  //-------------------------------------------------------------------------

  void hub::discover_new_lights() {
    string url = "http://" + m_ip_str + "/api/" + m_username + "/lights";
    string body = "";
    string response = http_post(url, body, m_ctx);
    Json::Value val = parse_json(response);
    if (val.isArray() && val[0].isMember("success")) {
    } else {
      throw hue_exception("Failed to initiate light discovery.");
    }
  }
  //-------------------------------------------------------------------------

  std::pair<string, std::list<light*> > hub::query_new_lights() {
    std::list<light*> out;
    string url = "http://" + m_ip_str + "/api/" + m_username + "/lights/new";
    string response = http_get(url, m_ctx);
    Json::Value val = parse_json(response);
    std::string status = "unknown";

    std::vector<string> members = val.getMemberNames();

    for (int i = 0; i < members.size(); ++i) {
      if (members[i] == "lastscan") {
        status = val[members[i]].asString();
      } else {
        int idx = atoi(members[i].c_str());
        init_light(idx, true);
        if (m_lights.count(idx)) {
          out.push_back(get_light(idx));
        }
      }
    }
    return std::make_pair(status, out);
  }
  //-------------------------------------------------------------------------

  void hub::refresh_light(int index) {
    string url = "http://" + m_ip_str + "/api/" + m_username + "/lights/"
      + to_str(index);
    Json::Value attrs = parse_json(http_get(url, m_ctx));
    if (attrs.isArray()) {
      throw hue_exception(attrs[0]["error"]["description"].asString());
    }
    m_status["lights"][to_str(index)] = attrs;
  }
  //-------------------------------------------------------------------------

  void hub::refresh_state() {
    if (!get_status()) {
      throw hue_exception("Failed to refresh hub status.");
    }
  }
  //-------------------------------------------------------------------------

  bool hub::init_username(hub_auth_callback callback) {
    std::cout << "Authenticating with hub...\n";
    string url = "http://" + m_ip_str + "/api";

    if (!get_status()) {
      // no dice getting the status, we need to register ourself

      // contents:
      Json::Value request;
      request["devicetype"] = Json::Value(m_device.c_str());
      request["username"] = Json::Value(m_username.c_str());
      std::stringstream request_strstr;
      request_strstr << request;
      string request_str = request_strstr.str();
      
      while(true) {
        try {
          string result = http_post(url, request_str, m_ctx);
          // check for success
          Json::Value root = parse_json(result);
          if (root[0].isMember("error")) {
            if (root[0]["error"].isMember("type")
              && root[0]["error"]["type"].asInt() == 101) {
              if (!callback()) {
                std::cout << "Hub authentication fail: link button not pressed\n";
                return false;
              }
            } else {
              throw hue_exception("Unknown error");
            }
          } else if (root[0].isMember("success")) {
            std::cout << "Authentication succes, new username: " << m_username 
              << " registered\n";
            break;
          } else {
            throw hue_exception("Unknown auth response");
          }
        } catch (hue_exception e) {
          std::cout << "Hub authentication fail: " << e.what() << "\n";
          return false;
        }
      }
      return true;
    } else {
      return true;
    }
  }
  //-------------------------------------------------------------------------

  bool hub::get_status() {
    string url = "http://" + m_ip_str + "/api/" + m_username;
    string result;

    try {
      result = http_get(url, m_ctx);
    } catch (std::exception e) {
      return false;
    }
    Json::Value root = parse_json(result);

    if (root.isArray()) {
      return false;
    }

    m_status = root;

    // initial setup from status blob:
    Json::Value lights = m_status["lights"];
    std::vector<string> light_indices = lights.getMemberNames();

    for (int i = 0; i < light_indices.size(); ++i) {
      int idx = atoi(light_indices[i].c_str());
      init_light(idx);
    }

    return true;
  }
  //-------------------------------------------------------------------------

  void hub::init_light(int index, bool fetch) {
    if (m_lights.count(index)) {
      // already exists, we're done
      return;
    }

    if (fetch) {
      refresh_light(index);
    }

    light* l = new light(index, this);
    m_lights[index] = l;
    m_light_names[l->get_name()] = l;

    std::cout << "Initializing light: " << index << " with name " << l->get_name() << "\n";
  }
  //-------------------------------------------------------------------------

  bool hub::await_hub_auth() {
    // default is just to prompt and wait for the user to hit enter
    std::cout << "Please hit the hub button and then press enter" << 
      " within 30 seconds.\n";
    std::cin.get();
    return true;
  }
  //-------------------------------------------------------------------------

  std::list<hub_addr> hub::Discover() {
    std::list<hub_addr> out;

    // create a one-off curl context...
    CURL* ctx = curl_easy_init();

    if (!ctx) {
      throw hue_exception("Curl init failure!");
    }

    // convenient discovery service (TODO: manually scan for upnp devices, 
    // so we don't need a 'net connection to run this).
    curl_easy_setopt(ctx, CURLOPT_URL, "http://www.meethue.com/api/nupnp");
    curl_easy_setopt(ctx, CURLOPT_FOLLOWLOCATION, 1L);

    string response = "";
    curl_easy_setopt(ctx, CURLOPT_WRITEFUNCTION, curl_string_writer);
    curl_easy_setopt(ctx, CURLOPT_WRITEDATA, (void *)&response);

    CURLcode res = curl_easy_perform(ctx);

    curl_easy_cleanup(ctx);
    if(res != CURLE_OK) {
      throw hue_exception("Failed to reach discovery service. Check your internet connection.");
    }

    Json::Value root = parse_json(response);

    for (int i = 0; i < root.size(); ++i) {
      Json::Value hub = root[i];
      string addr = hub["internalipaddress"].asString();
      out.push_back(str_to_addr(addr));
    }

    return out;
  }
  //-------------------------------------------------------------------------
}

