#include <stdexcept>
#include <iostream>

#include "Bitshares/Asset.h"
#include "HexCoding.h"

#include <gtest/gtest.h>

using namespace TW;
using namespace TW::Bitshares;

TEST(BitsharesAsset, Serialization) {
    Data buf;
    Asset(2, 0).serialize(buf);
    ASSERT_EQ(
        hex(buf),
        "020000000000000000"
    );

    buf.clear();
    Asset(511, 256).serialize(buf);
    ASSERT_EQ(
        hex(buf),
        "ff010000000000008002"
    );

    ASSERT_EQ(
        Asset(511, 256).serialize().dump(),
        "{\"amount\":511,\"asset_id\":\"1.3.256\"}"
    );
}