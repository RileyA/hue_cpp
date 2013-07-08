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
    curl_easy_setopt(ctx, CURLOPT_PUT, 0);
    curl_easy_setopt(ctx, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(ctx, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(ctx, CURLOPT_WRITEFUNCTION, curl_string_writer);
    curl_easy_setopt(ctx, CURLOPT_WRITEDATA, (void *)&response);
    if (curl_easy_perform(ctx) != CURLE_OK) {
      throw hue_exception("HTTP POST failed.");
    }
    return response;
  }

  string http_put(string url, string body, CURL* ctx) {
    string response;
    curl_easy_setopt(ctx, CURLOPT_URL, url.c_str());
    curl_easy_setopt(ctx, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(ctx, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(ctx, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(ctx, CURLOPT_WRITEFUNCTION, curl_string_writer);
    curl_easy_setopt(ctx, CURLOPT_WRITEDATA, (void *)&response);
    CURLcode res = curl_easy_perform(ctx);
    if (res != CURLE_OK) {
      throw hue_exception("HTTP PUT failed.");
    }
    curl_easy_setopt(ctx, CURLOPT_CUSTOMREQUEST, NULL);
    return response;
  }

  string http_get(string url, CURL* ctx) {
    string response;
    curl_easy_setopt(ctx, CURLOPT_URL, url.c_str());
    curl_easy_setopt(ctx, CURLOPT_PUT, 0);
    curl_easy_setopt(ctx, CURLOPT_HTTPGET, 1);
    curl_easy_setopt(ctx, CURLOPT_POST, 0);
    curl_easy_setopt(ctx, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(ctx, CURLOPT_WRITEFUNCTION, curl_string_writer);
    curl_easy_setopt(ctx, CURLOPT_WRITEDATA, (void *)&response);
    if (curl_easy_perform(ctx) != CURLE_OK) {
      throw hue_exception("HTTP GET failed.");
    }
    return response;
  }

  Json::Value parse_json(string json_str) {
    Json::Value root;
    Json::Reader reader;
    bool parse_success = reader.parse(json_str, root);
    if (!parse_success) {
      throw hue_exception("Failed to parse JSON.");
    }
    return root;
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

  int curl_string_reader(void *ptr, size_t size, size_t nmemb, void *in) {
    std::string* str = (std::string*)in;
    size_t len = str->size();
    if (len <= size * nmemb) {
      memcpy(ptr, str->c_str(), len);
      return len / nmemb;
    } else if (len != 0) {
      memcpy(ptr, str->c_str(), size * nmemb);
      *str = str->substr(size * nmemb);
      return size;
    } else {
      return 0;
    }
  }
}

