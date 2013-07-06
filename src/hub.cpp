#include "hub.h"
#include <sstream>

namespace hue {

  hub::hub(string ip, string device, string username,
    hub_auth_callback callback)
    : hub(str_to_addr(ip), device, username, callback) {
  }

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

  hub::~hub() {
    curl_easy_cleanup(m_ctx);
  }

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
          Json::Value root;
          Json::Reader reader;
          bool parse_success = reader.parse(result, root);
          if (!parse_success || !root.isArray()) {
            throw hue_exception("Bad response.");
          }
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

  bool hub::get_status() {
    string url = "http://" + m_ip_str + "/api/" + m_username;
    string result;

    try {
      result = http_get(url, m_ctx);
    } catch (std::exception e) {
      return false;
    }
    Json::Value root;
    Json::Reader reader;
    bool success = reader.parse(result, root);

    if (!success || root.isArray()) {
      return false;
    }

    m_status = root;
    return true;
  }

  bool hub::await_hub_auth() {
    // default is just to prompt and wait for the user to hit enter
    std::cout << "Please hit the hub button and then press enter" << 
      " within 30 seconds.\n";
    std::cin.get();
    return true;
  }

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

    // parse out the ips
    Json::Value root;
    Json::Reader reader;
    bool success = reader.parse(response, root);

    if (!success) {
      throw hue_exception("Failed to parse discovery service response json.");
    }

    for (int i = 0; i < root.size(); ++i) {
      Json::Value hub = root[i];
      string addr = hub["internalipaddress"].asString();
      out.push_back(str_to_addr(addr));
    }

    return out;
  }
}

