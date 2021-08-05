// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The BitcoinBuck developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef _BITCOINBUCKALERT_H_
#define _BITCOINBUCKALERT_H_ 1

#include <set>
#include <string>

#include "uint256.h"
#include "util.h"


static const char* pszMainKey = "04f4d48f5181fd4bcb85ff1b2ae07e94316a6d5d417b00b09c38273622c64b80d5926f4df74b775bc845a628b369a226008a686a75ba515b669c5689aa79b8cb37";

// TestNet alerts pubKey
static const char* pszTestKey = "04e3573d29feb5503d6ae6322120033186f9f6fd52674e5da64bcf35e3a7a313d3246ccdf2a93ef709a2e61af0e3ed457951315cd2ace38afd57b6392e258f9178";


class CNode;

/** Alerts are for notifying old versions if they become too obsolete and
 * need to upgrade.  The message is displayed in the status bar.
 * Alert messages are broadcast as a vector of signed data.  Unserializing may
 * not read the entire buffer if the alert is for a newer version, but older
 * versions can still relay the original data.
 */
class CUnsignedAlert
{
public:
    int nVersion;
    int64 nRelayUntil;      // when newer nodes stop relaying to newer nodes
    int64 nExpiration;
    int nID;
    int nCancel;
    std::set<int> setCancel;
    int nMinVer;            // lowest version inclusive
    int nMaxVer;            // highest version inclusive
    std::set<std::string> setSubVer;  // empty matches all
    int nPriority;

    // Actions
    std::string strComment;
    std::string strStatusBar;
    std::string strReserved;

    IMPLEMENT_SERIALIZE
    (
        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(nRelayUntil);
        READWRITE(nExpiration);
        READWRITE(nID);
        READWRITE(nCancel);
        READWRITE(setCancel);
        READWRITE(nMinVer);
        READWRITE(nMaxVer);
        READWRITE(setSubVer);
        READWRITE(nPriority);

        READWRITE(strComment);
        READWRITE(strStatusBar);
        READWRITE(strReserved);
    )

    void SetNull();

    std::string ToString() const;
    void print() const;
};

/** An alert is a combination of a serialized CUnsignedAlert and a signature. */
class CAlert : public CUnsignedAlert
{
public:
    std::vector<unsigned char> vchMsg;
    std::vector<unsigned char> vchSig;

    CAlert()
    {
        SetNull();
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(vchMsg);
        READWRITE(vchSig);
    )

    void SetNull();
    bool IsNull() const;
    uint256 GetHash() const;
    bool IsInEffect() const;
    bool Cancels(const CAlert& alert) const;
    bool AppliesTo(int nVersion, std::string strSubVerIn) const;
    bool AppliesToMe() const;
    bool RelayTo(CNode* pnode) const;
    bool CheckSignature() const;
    bool ProcessAlert();

    /*
     * Get copy of (active) alert object by hash. Returns a null alert if it is not found.
     */
    static CAlert getAlertByHash(const uint256 &hash);
};

#endif