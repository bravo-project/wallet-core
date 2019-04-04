#include "PackedTransaction.h"

#include "../HexCoding.h"

using namespace TW::EOS;
using json = nlohmann::json;

PackedTransaction::PackedTransaction(const Transaction& transaction, CompressionType type) noexcept : compression(type) {
    transaction.serialize(packedTrx);
    const Data& cfd = transaction.contextFreeData;

    if (cfd.size()) {
        packedCFD.push_back(1);
        packedCFD.push_back(cfd.size());
        append(packedCFD, cfd);
    }

    signatures = transaction.signatures;
}

void PackedTransaction::serialize(Data& os) const noexcept {
    using namespace TW::Bravo;
    encodeCollection(signatures, os);
    os.push_back(static_cast<uint8_t>(compression));
    encodeVarInt32(packedCFD.size(), os);
    append(os, packedCFD);
    encodeVarInt32(packedTrx.size(), os);
    append(os, packedTrx);
}

json PackedTransaction::serialize() const noexcept {
    // create a json array of signatures
    json sigs = json::array();
    for (const auto& sig : signatures) {
        sigs.push_back(sig.string());
    }

    json obj;

    using namespace TW::Bravo;
    obj["signatures"] = sigs;
    obj["compression"] = compression;
    obj["packed_context_free_data"] = hex(packedCFD);
    obj["packed_trx"] = hex(packedTrx);

    return obj;
}