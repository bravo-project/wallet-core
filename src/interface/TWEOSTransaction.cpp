#include <TrustWalletCore/TWEOSTransaction.h>

#include "../EOS/Transaction.h"

#include<string>

using namespace TW;
using namespace TW::EOS;

struct TWEOSTransaction *_Nullable TWEOSTransactionCreate(TWString *_Nonnull referenceBlockId, uint32_t referenceBlockTime) {
    auto refBlockId = reinterpret_cast<const std::string *>(referenceBlockId);
    try {
        return new TWEOSTransaction { Transaction(*refBlockId, referenceBlockTime) };
    } catch (...) {
        return nullptr;
    }
}

void TWEOSTransactionAddAction(struct TWEOSTransaction *_Nonnull transaction, struct TWEOSAction *_Nonnull action) {
    transaction->impl.actions.push_back(action->impl);
}

void TWEOSTransactionAddContextFreeAction(struct TWEOSTransaction *_Nonnull transaction, struct TWEOSAction *_Nonnull action) {
    transaction->impl.contextFreeActions.push_back(action->impl);
}

TWString *_Nonnull TWEOSTransactionDescription(struct TWEOSTransaction *_Nonnull transaction) {
    return TWStringCreateWithUTF8Bytes(transaction->impl.serialize().dump().c_str());
}

bool TWEOSTransactionIsValid(struct TWEOSTransaction *_Nonnull transaction) {
    return transaction->impl.isValid();
}

void TWEOSTransactionDelete(struct TWEOSTransaction *_Nonnull transaction) {
    delete transaction;
}