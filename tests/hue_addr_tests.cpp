#include "gtest/gtest.h"
#include "hue.h"

namespace {
    // Test simple str -> uint32_t
    TEST(StrToAddrTests, simple) {
        EXPECT_EQ(hue::str_to_addr("128.75.15.32"), static_cast<uint32_t>(
          (128 << 24) | (75 << 16) | (15 << 8) | 32));
    }

    // Test simple str -> uint32_t
    TEST(AddrToStrTests, simple) {
        EXPECT_EQ(hue::string("128.75.15.32"), hue::addr_to_string(
          static_cast<uint32_t>((128 << 24) | (75 << 16) | (15 << 8) | 32)));
    }
}

