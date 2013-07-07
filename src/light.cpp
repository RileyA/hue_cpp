#include "light.h"
#include "hub.h"

namespace hue {
  light::light(int index, hub* config)
    : m_config(config)
    , m_index(index) {
    m_url = "http://" + config->get_ip() + "/api/"
      + config->get_username() + "/lights/" + to_str(index);
  }

  string light::get_name() {
    return get()["name"].asString();
  }

  void light::set_name(string new_name) {
    Json::Value val;
    val["name"] = new_name;
    string body = to_str(val);
    Json::Value result = parse_json(http_put(get_url(), body, m_config->m_ctx));
    if (!result.isArray() || !result[0].isMember("success")) {
      throw hue_exception("Failed to rename light '" + get_name()
        + "' to '" + new_name + "'");
    }
    // keep our json up to date...
    get()["name"] = new_name;
  }

  Json::Value& light::get() {
    if (m_config->m_status["lights"][to_str(m_index)] != Json::Value()) {
      return m_config->m_status["lights"][to_str(m_index)];
    } else {
      throw hue_exception("Light doesn't exist!");
    }
  }
}

