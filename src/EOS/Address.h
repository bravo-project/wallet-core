// Copyright © 2017-2019 Trust.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include <string>

#include "../PublicKey.h"
#include "../Data.h"
#include "Prefixes.h"

namespace TW {
namespace EOS {

class Address {
public:

    /// Determines whether a string makes a valid EOS address.
    static bool isValid(const std::string& string);

    /// Determines whether the given byte array is a valid keyBuffer
    static bool isValid(const uint8_t *buffer, size_t size, Type type = Type::Legacy);

    /// Initializes a EOS address from a string representation.
    Address(const std::string& string);

    /// Initializes a EOS address from a public key.
    Address(const PublicKey& publicKey, Type type = Type::Legacy);

    /// Initializes a EOS address from a byte array.
    Address(const Data& data, Type type = Type::Legacy);

    /// Returns a string representation of the EOS address.
    std::string string() const;

    friend bool operator==(const Address& lhs, const Address& rhs);

    inline std::string prefix() const { return pubPrefixForType(type); }



protected:
    // Class constants
    static const size_t PublicKeyDataSize = 33;
    static const size_t ChecksumSize = 4;
    static const size_t KeyDataSize = PublicKeyDataSize + ChecksumSize;
    static const size_t Base58Size = KeyDataSize * 138 / 100 + 2; // a buffer size large enough to fit base58 representation of the key data


    uint8_t keyData[KeyDataSize];
    Type type;

    static uint32_t createChecksum(const uint8_t *buffer, Type type);
    static bool extractKeyData(const std::string& string, Address *address = nullptr);
};


inline bool operator==(const Address& lhs, const Address& rhs) {
    return memcmp(lhs.keyData, rhs.keyData, Address::KeyDataSize) == 0
             && lhs.type == rhs.type;
}

}} // namespace

struct TWEOSAddress {
    TW::EOS::Address impl;
};