// Copyright � 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "../Bravo/Address.h"

using namespace TW::Bravo;

// Steem address class is similar to Bravo. 
// Just creating TWSteemAddress struct for C interface 

struct TWSteemAddress {
	TW::Bravo::Address impl;
};