//
//  BRChainParams.h
//
//  Created by Aaron Voisine on 1/10/18.
//  Copyright (c) 2019 breadwallet LLC
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.

#ifndef BRChainParams_h
#define BRChainParams_h

#include "BRMerkleBlock.h"
#include "BRPeer.h"
#include "BRSet.h"
#include "BRPeer.h"
#include <assert.h>

typedef struct {
    uint32_t height;
    UInt256 hash;
    uint32_t timestamp;
    uint32_t target;
} BRCheckPoint;

typedef struct {
    const char * const *dnsSeeds; // NULL terminated array of dns seeds
    uint16_t standardPort;
    uint32_t magicNumber;
    uint64_t services;
    int (*verifyDifficulty)(const BRMerkleBlock *block, const BRSet *blockSet); // blockSet must have last 2016 blocks
    const BRCheckPoint *checkpoints;
    size_t checkpointsCount;
} BRChainParams;

static const char *BRMainNetDNSSeeds[] = {
    "mazacoin.org", NULL
};

static const char *BRTestNetDNSSeeds[] = {
  "mazatest.cryptoadhd.com", NULL
};

// blockchain checkpoints - these are also used as starting points for partial chain downloads, so they must be at
// difficulty transition boundaries in order to verify the block difficulty at the immediately following transition
static const BRCheckPoint BRMainNetCheckpoints[] = {
    {      0, uint256("00000c7c73d8ce604178dae13f0fc6ec0be3275614366d44b1b4b5c6e238c60c"), 1390747675, 0x1e0ffff0u },
    {   91800, uint256("00000000000000f35417a67ff0bb5cec6a1c64d13bb1359ae4a03d2c9d44d900"), 1403364920, 0x1a2083d6u },
    {   183600, uint256("0000000000000787f10fa4a547822f8170f1f182ca0de60ecd2de189471da885"), 1415237368, 0x1a259a07u },
    {   700000, uint256("000000000000018674cd89025fc8190e5fc1a558dce38392e43f3603cb1cb192"), 1480586887, 0x1a026de3u },
    {   750000, uint256("0000000000000024a619312835504165c91b817a50ee724fc3f2a48565fdb555"), 1486902764, 0x1a0256ddu },
    {   800000, uint256("000000000000010c0245a794d16023ffb7a0e5f0fceb991e9f15706e711272de"), 1493405361, 0x1a01bc8eu },
    {   850000, uint256("000000000000025553ea305539a442cfa620d5224252f641f5250a52b53cdea7"), 1499985075, 0x1a0285c4u },
    {   870000, uint256("00000000000004386593649e6ad9a2ed3153710d94a55bf8dfa630baf53ec5ec"), 1502541642, 0x1a04a3aau }
};

static const BRCheckPoint BRTestNetCheckpoints[] = {
  {      261, uint256("00000c26026d0815a7e2ce4fa270775f61403c040647ff2c3091f99e894a4618"), 1423410572,  0x1e0ffff0u },
  {     3000, uint256("0000013ee65b53f805d6d7e9384a4cba59bdc97f2de0d216f40aba9aac7c5809"), 1462856602,  0x1e015555u },
  {     6000, uint256("0000002f95bdd3f7f85fceeb9091886dea8e1f6cb189765fb828b9c64064fd6c"), 1463327199,  0x1d4a220eu }
};


static int BRMainNetVerifyDifficulty(const BRMerkleBlock *block, const BRSet *blockSet)
{
    const BRMerkleBlock *previous, *b = NULL;
    uint32_t i;
    
    assert(block != NULL);
    assert(blockSet != NULL);
    
    // check if we hit a difficulty transition, and find previous transition block
    if ((block->height % BLOCK_DIFFICULTY_INTERVAL) == 0) {
        for (i = 0, b = block; b && i < BLOCK_DIFFICULTY_INTERVAL; i++) {
            b = BRSetGet(blockSet, &b->prevBlock);
        }
    }
    
    previous = BRSetGet(blockSet, &block->prevBlock);
    return BRMerkleBlockVerifyDifficulty(block, previous, (b) ? b->timestamp : 0);
}

static int BRTestNetVerifyDifficulty(const BRMerkleBlock *block, const BRSet *blockSet)
{
    return 1; // XXX skip testnet difficulty check for now
}

static const BRChainParams BRMainNetParams = {
    BRMainNetDNSSeeds,
    12835,                  // standardPort
    0xdf03b5f8,            // magicNumber
    SERVICES_NODE_NETWORK, // services
    BRMainNetVerifyDifficulty,
    BRMainNetCheckpoints,
    sizeof(BRMainNetCheckpoints)/sizeof(*BRMainNetCheckpoints)
};

static const BRChainParams BRTestNetParams = {
    BRTestNetDNSSeeds,
    11835,                 // standardPort
    0x01a9fe05,            // magicNumber
    SERVICES_NODE_NETWORK, // services
    BRTestNetVerifyDifficulty,
    BRTestNetCheckpoints,
    sizeof(BRTestNetCheckpoints)/sizeof(*BRTestNetCheckpoints)
};

#endif // BRChainParams_h
