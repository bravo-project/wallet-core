#pragma once

#include "TWBase.h"

#include "TWBitsharesProto.h"
#include "TWCommonProto.h"

TW_EXTERN_C_BEGIN

/// Represents a Bitshares Signer.
TW_EXPORT_CLASS
struct TWBitsharesSigner;

TW_EXPORT_STATIC_METHOD
TW_Proto_Result TWBitsharesSignerSign(TW_Bitshares_Proto_SigningInput input);

TW_EXTERN_C_END