#include "hue.h"
#include <sstream>

namespace hue {
  // convert an ip address string to a quadword, sorta ugly...
  hub_addr str_to_addr(string ip) {
    size_t pos = 0;
    hub_addr out = 0;
    for (int i = 0; i < 4; ++i) {
      size_t separator_pos = ip.find_first_of(".");
      if (separator_pos == string::npos) {
        if (i == 3) {
          separator_pos = ip.size();
        } else {
          throw hue_exception("Invalid IP address.");
        }
      }
      size_t val = atoi(ip.substr(0, separator_pos).c_str());
      if (val > 255) {
        throw hue_exception("Invalid IP address.");
      }
      out |= (val << (8 * (3 - i)));

      if (i != 3) {
        ip = ip.substr(separator_pos + 1);
      }
    }
    return out;
  }

  // convert a quadword to a string ip
  string addr_to_string(hub_addr ip) {
    std::stringstream ss;
    for (int i = 0; i < 4; ++i) {
      uint32_t octet = (ip >> ((3 - i) * 8)) & 0xff;
      ss << octet;
      if (i != 3) {
        ss << ".";
      }
    }
    return ss.str();
  }
}

