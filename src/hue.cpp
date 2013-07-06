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

  string http_post(string url, string body, CURL* ctx) {
    string response;
    curl_easy_setopt(ctx, CURLOPT_URL, url.c_str());
    curl_easy_setopt(ctx, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(ctx, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(ctx, CURLOPT_WRITEFUNCTION, curl_string_writer);
    curl_easy_setopt(ctx, CURLOPT_WRITEDATA, (void *)&response);
    if (curl_easy_perform(ctx) != CURLE_OK) {
      throw hue_exception("HTTP POST failed.");
    }
    return response;
  }

  string http_get(string url, CURL* ctx) {
    string response;
    curl_easy_setopt(ctx, CURLOPT_URL, url.c_str());
    curl_easy_setopt(ctx, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(ctx, CURLOPT_WRITEFUNCTION, curl_string_writer);
    curl_easy_setopt(ctx, CURLOPT_WRITEDATA, (void *)&response);
    if (curl_easy_perform(ctx) != CURLE_OK) {
      throw hue_exception("HTTP GET failed.");
    }
    return response;
  }

  int curl_string_writer(char *data, size_t size, size_t nmemb, std::string *out)
  {
    int result = 0;
    if(out)
    {
      out->append(data, size * nmemb);
      result = size * nmemb;
    }
    return result;
  }
}

