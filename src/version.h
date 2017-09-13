// Copyright (c) 2012 The Bitcoin developers
// Copyright (c) 2012-2013 The Peercoin developers
// Copyright (c) 2016-2017 The Sparklecoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_VERSION_H
#define BITCOIN_VERSION_H

#include <string>

//
// client versioning
//

// These need to be macro's, as version.cpp's voodoo requires it

// sparklecoin version - intended for display purpose only
#define SPARKLEUNITY_VERSION_MAJOR       1
#define SPARKLEUNITY_VERSION_MINOR       0
#define SPARKLEUNITY_VERSION_REVISION    1
#define SPARKLEUNITY_VERSION_BUILD       0

static const int SPARKLEUNITY_VERSION =
                           1000000 * SPARKLEUNITY_VERSION_MAJOR
                         +   10000 * SPARKLEUNITY_VERSION_MINOR
                         +     100 * SPARKLEUNITY_VERSION_REVISION
                         +       1 * SPARKLEUNITY_VERSION_BUILD;

// sparklecoin version - reference for code tracking
#define SPRKOIN_VERSION_MAJOR       0
#define SPRKOIN_VERSION_MINOR       5
#define SPRKOIN_VERSION_REVISION    0
#define SPRKOIN_VERSION_BUILD       0

static const int SPRKOIN_VERSION =
                           1000000 * SPRKOIN_VERSION_MAJOR
                         +   10000 * SPRKOIN_VERSION_MINOR
                         +     100 * SPRKOIN_VERSION_REVISION
                         +       1 * SPRKOIN_VERSION_BUILD;

// bitcoin version - reference for code tracking
#define BITCOIN_VERSION_MAJOR       0
#define BITCOIN_VERSION_MINOR       6
#define BITCOIN_VERSION_REVISION    3
#define BITCOIN_VERSION_BUILD       0

static const int BITCOIN_VERSION =
                           1000000 * BITCOIN_VERSION_MAJOR
                         +   10000 * BITCOIN_VERSION_MINOR 
                         +     100 * BITCOIN_VERSION_REVISION
                         +       1 * BITCOIN_VERSION_BUILD;

static const int CLIENT_VERSION = BITCOIN_VERSION;

extern const std::string CLIENT_NAME;
extern const std::string CLIENT_BUILD;
extern const std::string CLIENT_DATE;


//
// network protocol versioning
//

static const int PROTOCOL_VERSION = 60006;

// earlier versions not supported as of Feb 2012, and are disconnected
// NOTE: as of bitcoin v0.6 message serialization (vSend, vRecv) still
// uses MIN_PROTO_VERSION(209), where message format uses PROTOCOL_VERSION
static const int MIN_PROTO_VERSION = 209;

// nTime field added to CAddress, starting with this version;
// if possible, avoid requesting addresses nodes older than this
static const int CADDR_TIME_VERSION = 31402;

// only request blocks from nodes outside this range of versions
static const int NOBLKS_VERSION_START = 32000;
static const int NOBLKS_VERSION_END = 32400;

// BIP 0031, pong message, is enabled for all versions AFTER this one
static const int BIP0031_VERSION = 60000;

#endif
