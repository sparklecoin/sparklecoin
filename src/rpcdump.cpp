// Copyright (c) 2009-2012 Bitcoin Developers
// Copyright (c) 2012-2013 The SPKLoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "init.h" // for pwalletMain
#include "bitcoinrpc.h"
#include "ui_interface.h"
#include "base58.h"
#include "string"

#include <boost/lexical_cast.hpp>

#include "json/json_spirit_reader_template.h"
#include "json/json_spirit_writer_template.h"
#include "json/json_spirit_utils.h"

#define printf OutputDebugStringF

// using namespace boost::asio;
using namespace json_spirit;
using namespace std;

extern Object JSONRPCError(int code, const string& message);

class CTxDump
{
public:
    CBlockIndex *pindex;
    int64 nValue;
    bool fSpent;
    CWalletTx* ptx;
    int nOut;
    CTxDump(CWalletTx* ptx = NULL, int nOut = -1)
    {
        pindex = NULL;
        nValue = 0;
        fSpent = false;
        this->ptx = ptx;
        this->nOut = nOut;
    }
};

void importPrivateKey(string strSecret, string strLabel = "")
{
    CBitcoinSecret vchSecret;
    bool fGood = vchSecret.SetString(strSecret);

    if (!fGood) throw JSONRPCError(-5,"Invalid private key");
    if (pwalletMain->IsLocked())
        throw JSONRPCError(-13, "Error: Please enter the wallet passphrase with walletpassphrase first.");
    if (fWalletUnlockMintOnly) // sparklecoin: no importprivkey in mint-only mode
        throw JSONRPCError(-102, "Wallet is unlocked for minting only.");

    CKey key;
    bool fCompressed;
    CSecret secret = vchSecret.GetSecret(fCompressed);
    key.SetSecret(secret, fCompressed);
    CKeyID vchAddress = key.GetPubKey().GetID();
    {
        LOCK2(cs_main, pwalletMain->cs_wallet);

        pwalletMain->MarkDirty();
        pwalletMain->SetAddressBookName(vchAddress, strLabel);

        if (!pwalletMain->AddKey(key))
            throw JSONRPCError(-4,"Error adding key to wallet. It maybe already imported.");

        pwalletMain->ScanForWalletTransactions(pindexGenesisBlock, true);
        pwalletMain->ReacceptWalletTransactions();
    }

    MainFrameRepaint();
}

Value importprivkey(const Array& params, bool fHelp)
{
    if (fHelp || params.size() < 1 || params.size() > 2)
        throw runtime_error(
            "importprivkey <sparklecoinprivkey> [label]\n"
            "Adds a private key (as returned by dumpprivkey) to your wallet.");

    string strSecret = params[0].get_str();
    string strLabel = "";
    if (params.size() > 1)
        strLabel = params[1].get_str();
    importPrivateKey(strSecret, strLabel);
    /*
    CBitcoinSecret vchSecret;
    bool fGood = vchSecret.SetString(strSecret);

    if (!fGood) throw JSONRPCError(-5,"Invalid private key");
    if (pwalletMain->IsLocked())
        throw JSONRPCError(-13, "Error: Please enter the wallet passphrase with walletpassphrase first.");
    if (fWalletUnlockMintOnly) // sparklecoin: no importprivkey in mint-only mode
        throw JSONRPCError(-102, "Wallet is unlocked for minting only.");

    CKey key;
    bool fCompressed;
    CSecret secret = vchSecret.GetSecret(fCompressed);
    key.SetSecret(secret, fCompressed);
    CKeyID vchAddress = key.GetPubKey().GetID();
    {
        LOCK2(cs_main, pwalletMain->cs_wallet);

        pwalletMain->MarkDirty();
        pwalletMain->SetAddressBookName(vchAddress, strLabel);

        if (!pwalletMain->AddKey(key))
            throw JSONRPCError(-4,"Error adding key to wallet");

        pwalletMain->ScanForWalletTransactions(pindexGenesisBlock, true);
        pwalletMain->ReacceptWalletTransactions();
    }

    MainFrameRepaint();
*/
    return Value::null;
}
Value dumpprivkey(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "dumpprivkey <sparklecoinaddress>\n"
            "Reveals the private key corresponding to <sparklecoinaddress>.");

    string strAddress = params[0].get_str();
    CBitcoinAddress address;
    if (!address.SetString(strAddress))
        throw JSONRPCError(-5, "Invalid Sparklecoin address");
    if (pwalletMain->IsLocked())
        throw JSONRPCError(-13, "Error: Please enter the wallet passphrase with walletpassphrase first.");
    if (fWalletUnlockMintOnly) // sparklecoin: no dumpprivkey in mint-only mode
        throw JSONRPCError(-102, "Wallet is unlocked for minting only.");
    CKeyID keyID;
    if (!address.GetKeyID(keyID))
        throw JSONRPCError(-3, "Address does not refer to a key");
    CSecret vchSecret;
    bool fCompressed;
    if (!pwalletMain->GetSecret(keyID, vchSecret, fCompressed))
        throw JSONRPCError(-4,"Private key for address " + strAddress + " is not known");
    return CBitcoinSecret(vchSecret, fCompressed).ToString();
}

bool fileExists(string filePath)
{
    return  boost::filesystem::exists(filePath);
}

Value importwallet(const Array& params, bool fHelp)
{
    if (fHelp || params.size() < 1)
        throw runtime_error(
            "importwallet <file_path> wallet-pass-phrase \n"
            "Adds wallet dat file of your previous wallet to import.");

    string walletFile = params[0].get_str();
    string passPhrase = "";
    if(params.size() > 1)
    	passPhrase = params[1].get_str();
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    if(!fileExists(walletFile))
    	throw runtime_error("File doesn't exists. Please make sure to use correct path. \n");
    bool fFirstRun;
    CWallet* tempWallet = new CWallet(walletFile);
    int nLoadWallet = tempWallet->LoadWallet(fFirstRun);
    if (nLoadWallet != DB_LOAD_OK)
    {
        if (nLoadWallet == DB_CORRUPT)
            throw runtime_error("Error loading "+walletFile+": Wallet corrupted \n");
        else if (nLoadWallet == DB_TOO_NEW)
        	throw runtime_error("Error loading "+walletFile+": Wallet requires newer version of Sparkleunity\n");
        else if (nLoadWallet == DB_NEED_REWRITE)
            throw runtime_error(walletFile+" file is incomplete \n");
        else
            throw runtime_error("Error Loading wallet file "+walletFile+" \n");
    }
    if (tempWallet->IsLocked() && params.size() == 1)
        throw JSONRPCError(-13, "Error: The wallet you are importing is encrypted. Please provide valid passphrase to unlock wallet");
    else if(tempWallet->IsLocked())
    {
        SecureString strWalletPass;
        strWalletPass.reserve(100);
        strWalletPass = passPhrase.c_str();
        if (!tempWallet->Unlock(strWalletPass))
        	throw JSONRPCError(-14, "Error: The wallet passphrase entered was incorrect.");
   }

    Object response;
    Object result;
    response.push_back(Pair("status", "success"));
    BOOST_FOREACH(const PAIRTYPE(CBitcoinAddress, string)& item, tempWallet->mapAddressBook)
    {
    	const CBitcoinAddress& address = item.first;
    	CSecret vchSecret;
    	bool fCompressed;
    	CKeyID keyID;
    	address.GetKeyID(keyID);
    	CScript scriptKey;
    	scriptKey.SetDestination(address.Get());
    	if(address.IsValid() && IsMine(*tempWallet, scriptKey))
    	{
        	tempWallet->GetSecret(keyID, vchSecret, fCompressed);
        	stringstream Size;
        	Size << vchSecret.size();
        	CBitcoinSecret keySecret = CBitcoinSecret(vchSecret, fCompressed);
        	string strAccount;
        	strAccount = item.second;
        	importPrivateKey(keySecret.ToString(), strAccount + "-Old");
        	result.push_back(Pair(strAccount, address.ToString()));
    	}
    }
    response.push_back(Pair("result", result));
    return response;//"Wallet Successfully Imported \n" + str;
}
