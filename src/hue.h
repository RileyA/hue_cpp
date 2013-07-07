#ifndef hue_H
#define hue_H

#include <cstdlib>
#include <stdint.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <list>
#include <sstream>

// 3rd party dependencies:
#include <curl/curl.h>
#include <jsoncpp/json/json.h>

/** Establish the namespace, and typedef/declare some common things */
namespace hue {
    typedef std::string string;
    typedef uint32_t hub_addr; // an IPv4 hub address

    hub_addr str_to_addr(string ip);
    string addr_to_string(hub_addr ip);

    string http_post(string url, string body, CURL* ctx);
    string http_put(string url, string body, CURL* ctx);
    string http_get(string url, CURL* ctx);
    int curl_string_writer(char *data, size_t size, size_t nmemb, std::string *out);
    int curl_string_reader(void *ptr, size_t size, size_t nmemb, void *in);

    Json::Value parse_json(string json_str);

    /** Turn something that can be dumped into a stringstream into a string */
    template<typename T> string to_str(T arg) {
      std::stringstream ss;
      ss << arg;
      return ss.str();
    }

    class hue_exception : public std::exception {
    public:
        hue_exception(string what) : m_what(what) {}
        virtual ~hue_exception() throw() {}
        virtual const char* what() { return m_what.c_str(); }
    private:
        string m_what;
    };
};


#endif

