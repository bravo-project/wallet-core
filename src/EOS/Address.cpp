#include "Address.h"

#include <stdexcept>
#include <TrezorCrypto/base58.h>
#include <TrezorCrypto/ripemd160.h>
#include <BinaryCoding.h>

using namespace TW::EOS;

bool Address::isValid(const std::string& string) {
    return extractKeyData(string);
}

/// Determines whether the given byte array is a valid keyBuffer
/// Verifies the buffer's size and it's checksum bytes
bool Address::isValid(const uint8_t *buffer, size_t size, EOS::Address::Type type) {
    // last Address::ChecksumSize bytes are a checksum
    uint32_t checksum = decode32LE(buffer + PublicKeyDataSize);

    if (size != KeyDataSize) return false;
    if (createChecksum(buffer, type) != checksum) return false;
    return true;
}

/// Creates a checksum of PublicKeyDataSize bytes at the buffer
/// IMPORTANT: THERE ARE NO SIZE CHECKS. THE BUFFER IS ASSUMED
///             TO HAVE AT LEAST PublicKeyDataSize bytes.
uint32_t Address::createChecksum(const uint8_t *buffer, Address::Type type) {
    // create our own checksum and compare the two
    uint8_t hash[RIPEMD160_DIGEST_LENGTH];
    RIPEMD160_CTX ctx;
    ripemd160_Init(&ctx);

    // add the buffer bytes to the hash input
    ripemd160_Update(&ctx, buffer, PublicKeyDataSize);

    //  append the prefix to the hash input as well in case of modern types
    switch (type) {
    case Type::Legacy: // no extra input
        break;

    case Type::ModernK1:
        ripemd160_Update(&ctx, (const uint8_t *) Modern::K1::subPrefix, Modern::K1::subPrefixSize);
        break;

    case Type::ModernR1:
        ripemd160_Update(&ctx, (const uint8_t *) Modern::R1::subPrefix, Modern::R1::subPrefixSize);
        break;
    }

    // finalize the hash
    ripemd160_Final(&ctx, hash);

    return decode32LE(hash);
}

/// Extracts and verifies the key data from a base58 string.
/// If the second arg is provided, the keyData and isTestNet 
/// properties of that object are set from the extracted data.
bool Address::extractKeyData(const std::string& string, Address *address) {
    // verify if the string has one of the valid prefixes
    Type type;
    size_t prefixSize;
    // std::cout << "Checking prefix for " + string + ".\n";
    // std::cout << "Prefix: " + string.substr(0, Legacy::prefixSize) + "\n";
    // std::cout << "Legacy Prefix: " + Legacy::prefix + " (" << sizeof(Legacy::prefix) << "\n";
    if (string.substr(0, Legacy::prefixSize) == Legacy::prefix) {
        type = Type::Legacy;
        prefixSize = Legacy::prefixSize;
    } else if (string.substr(0, Modern::R1::fullPrefixSize) == Modern::R1::fullPrefix) {
        type = Type::ModernR1;
        prefixSize = Modern::R1::fullPrefixSize;
    } else if (string.substr(0, Modern::K1::fullPrefixSize) == Modern::K1::fullPrefix) {
        type = Type::ModernK1;
        prefixSize = Modern::K1::fullPrefixSize;
    } else {
        return false;
    }

    // the binary data will easily fit in a buffer of the size of base58 data
    size_t size = string.size();

    uint8_t buffer[size];
    if (!b58tobin(buffer, &size, string.c_str() + prefixSize)) {
        return false;
    }

    if (size != KeyDataSize) {
        return false;
    }

    // get a pointer past the padding
    const uint8_t *bufferP = buffer + string.size() - size;
    if (isValid(bufferP, size, type)) {
        if (address) {
            std::memcpy(address->keyData, bufferP, KeyDataSize);
            address->type = type;
        }

        return true;
    }

    return false;
}

/// Initializes a EOS address from a string representation.
Address::Address(const std::string& string) {
    if (!Address::extractKeyData(string, this)) {
        throw std::invalid_argument("Invalid address string!");
    }
}

/// Initializes a EOS address from raw bytes
Address::Address(const Data& data, Address::Type type) : type(type) {
    if (!isValid(data.data(), data.size(), type)) {
        throw std::invalid_argument("Invalid byte size!");
    }

    std::memcpy(keyData, data.data(), KeyDataSize);
}

/// Initializes a EOS address from a public key.
Address::Address(const PublicKey& publicKey, Address::Type type) : type(type) {
    assert(PublicKeyDataSize == TW::PublicKey::secp256k1Size);

    // copy the raw, compressed key data
    std::memcpy(keyData, 
                reinterpret_cast<void *>(publicKey.compressed().bytes.data()), 
                PublicKeyDataSize);

    // append the checksum
    uint32_t checksum = createChecksum(keyData, type);
    Data bytes;
    encode32LE(checksum, bytes);

    for (int i = 0; i < ChecksumSize; ++i) {
        keyData[PublicKeyDataSize + i] = bytes[i];
    } 
}

/// Returns a string representation of the EOS address.
std::string Address::string() const {
    std::string prefix = prefixForType(type);

    size_t b58Size = Base58Size;
    char b58[b58Size];
    b58enc(b58, &b58Size, keyData, KeyDataSize);

    return prefix + std::string(b58);
}