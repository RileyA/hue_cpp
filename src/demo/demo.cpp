#include <iostream>
#include "hue.h"
#include "hub.h"

const static char* DEVICE = "Linux PC"; // arbitrary, but true of my testing setup
const static char* USERNAME = "huecppdebug";

int main(int argc, char** argv) {

  std::cout << "Discovering hubs...\n";
  std::list<hue::hub_addr> addrs = hue::hub::Discover();;

  if (addrs.empty()) {
    std::cout << "No hubs found, check your network config.\n";
    return -1;
  }

  std::cout << "Found " << addrs.size() << " hub" 
    << ((addrs.size() > 1) ? "s." : ".") << "\n";
  size_t idx = 0;
  for (std::list<hue::hub_addr>::iterator it = addrs.begin();
    it != addrs.end(); ++it) {
    std::cout << "  " << idx++ << ": " << hue::addr_to_string(*it) << "\n";
  }

  std::cout << "Enter hub index to use: ";
  size_t hub_idx = 0;
  std::cin >> hub_idx;
  std::cin.clear();
  std::cin.ignore(1);

  if (hub_idx > addrs.size() - 1) {
    std::cout << "Invalid hub index!\n";
    return -1;
  }

  hue::hub_addr address = 0;
  std::list<hue::hub_addr>::iterator hubs = addrs.begin();
  for (int i = 0; i < hub_idx; ++i) { ++hubs; }
  address = *hubs;

  std::cout << "Using hub at: " << hue::addr_to_string(address) << "\n";

  // setup our hub...
  hue::hub h = hue::hub(address, DEVICE, USERNAME);

  return 0;
}

