#include "hub.h"

static int writer(char *data, size_t size, size_t nmemb, std::string *out)
{
  int result = 0;
  if(out)
  {
    out->append(data, size * nmemb);
    result = size * nmemb;
  }
  return result;
}

namespace hue {
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
    curl_easy_setopt(ctx, CURLOPT_WRITEFUNCTION, writer);
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

