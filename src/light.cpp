#include "light.h"
#include "hub.h"

namespace hue {
  light::light(int index, hub* config)
    : m_config(config)
    , m_index(index) {
    m_url = "http://" + config->get_ip() + "/api/"
      + config->get_username() + "/lights/" + to_str(index);
  }
  //-------------------------------------------------------------------------

  string light::get_name() {
    return get()["name"].asString();
  }
  //-------------------------------------------------------------------------

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
  //-------------------------------------------------------------------------

  void light::set_state(state_change state, bool deduplicate) {
    Json::Value& cur_state = get()["state"];
    if (deduplicate) {
      std::vector<string> members = state.obj.getMemberNames();
      for (int i = 0; i < members.size(); ++i) {
        if (cur_state[members[i]] == state.obj[members[i]]) {
          state.obj.removeMember(members[i]);
        }
      }
      if (state.obj.getMemberNames().empty()) {
        // this is a no-op (as far as out prior knowledge suggests...)
        return;
      }
    }
    Json::Value result = parse_json(http_put(get_url() + "/state",
      to_str(state.obj), m_config->m_ctx));
    
    // get response (will be an array of failures or successes)
    for (Json::ValueIterator it = result.begin(); it != result.end(); ++it) {
      Json::Value v = *it;
      if (v.isMember("success")) {
        if (v["success"].getMemberNames().size() > 1) {
          throw hue_exception("Unexpected response fields.");
        } else {
          string prop = v["success"].begin().memberName();
          int pos = prop.find_last_of('/');
          prop = prop.substr(pos + 1);
          cur_state[prop] = *(v["success"].begin()); // keep our view of the world up to date
        }
      }
    }
  }
  //-------------------------------------------------------------------------

  Json::Value& light::get() {
    if (m_config->m_status["lights"][to_str(m_index)] != Json::Value()) {
      return m_config->m_status["lights"][to_str(m_index)];
    } else {
      throw hue_exception("Light doesn't exist!");
    }
  }
  //-------------------------------------------------------------------------
}

