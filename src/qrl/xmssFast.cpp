// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#include <iostream>
#include <xmss-alt/xmss_params.h>
#include "xmssFast.h"

XmssFast::XmssFast(const TSEED &seed,
                   unsigned char height,
                   eHashFunction hashFunction,
                   eAddrFormatType addrFormatType ) throw(std::invalid_argument)
        : XmssBase(seed, height, hashFunction, addrFormatType) {
//    PK format
//    32 root address
//    32 pub_seed
//
//    SK format
//    4  idx
//    32 sk_seed
//    32 sk_prf
//    32 pub_seed
//    32 root
// TODO: Use a union? to operated on partial fields

    // FIXME: Inconsistency here
    _sk = TKEY(132, 0);
    auto tmp = TKEY(64, 0);

    const uint32_t k = 2;
    const uint32_t w = 16;
    const uint32_t n = 32;

    if (k >= height || (height - k) % 2) {
        throw std::invalid_argument("For BDS traversal, H - K must be even, with H > K >= 2!");
    }

    xmss_set_params(&params, n, height, w, k);

    _stackoffset = 0;
    _stack = std::vector<unsigned char>((height + 1) * n);
    _stacklevels = std::vector<unsigned char>(height + 1);
    _auth = std::vector<unsigned char>(height * n);
    _keep = std::vector<unsigned char>((height >> 1) * n);
    _treehash = std::vector<treehash_inst>(height - k);
    _th_nodes = std::vector<unsigned char>((height - k) * n);
    _retain = std::vector<unsigned char>(((1 << k) - k - 1) * n);

    for (int i = 0; i < height - k; i++) {
        _treehash[i].node = &_th_nodes[n * i];
    }

    xmss_set_bds_state(&_state,
                       _stack.data(),
                       _stackoffset,
                       _stacklevels.data(),
                       _auth.data(),
                       _keep.data(),
                       _treehash.data(),
                       _retain.data(),
                       0);

    xmssfast_Genkeypair(_hashFunction,
                        &params,
                        tmp.data(),
                        _sk.data(),
                        &_state,
                        _seed.data());
}

unsigned int XmssFast::setIndex(unsigned int new_index) {
    xmssfast_update(_hashFunction,
                    &params,
                    _sk.data(),
                    &_state,
                    new_index);

    return new_index;
}

TSIGNATURE XmssFast::sign(const TMESSAGE &message) {
    auto signature = TSIGNATURE(getSignatureSize(), 0);

    auto index = getIndex();
    setIndex(index);

    xmssfast_Signmsg(_hashFunction,
                     &params,
                     _sk.data(),
                     &_state,
                     signature.data(),
                     static_cast<TMESSAGE>(message).data(),
                     message.size());

    return signature;
}
