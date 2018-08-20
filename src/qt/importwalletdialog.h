#ifndef IMPORTWALLETDIALOG_H
#define IMPORTWALLETDIALOG_H

#include <QDialog>
#include <QThread>
#include <QDebug>
#include "init.h"
#include "bitcoinrpc.h"
#include "ui_interface.h"
#include "base58.h"
#include <string>

#ifndef Q_MOC_RUN
#include <boost/lexical_cast.hpp>
#endif

#include "json/json_spirit_reader_template.h"
#include "json/json_spirit_writer_template.h"
#include "json/json_spirit_utils.h"


namespace Ui {
    class ImportWalletDialog;
}
using namespace std;

class WalletManager;

class ImportWalletDialog : public QDialog
{
    Q_OBJECT
    public:
        QString walletFile;
        QSvgWidget *image;
        WalletManager *wallet;
        explicit ImportWalletDialog(QWidget *parent = 0, QString walletFile = "");
        ~ImportWalletDialog();
        void ImportWalletRequest();
        void processRequest();
        void show();
        void processRequest(QString WalletPassPhrase);
        void postClose();
        void error(QString title, QString message, bool modal = true);
        void error(std::string title, std::string message, bool modal = true);
        void error(const char* title, const char* message, bool modal = true);
    private:
        Ui::ImportWalletDialog *ui;
    public slots:
            void failHandler(QString message)
            {
                this->error("Error", message);
            }
            void passPhraseDialog();
            void successHandler();
    signals:
            void failed(QString message);
            void getPassPhrase();
            void success();
};
class WalletManager
{
private:
    CWallet* tempWallet;
    bool fileExists(string filePath)
    {
        return  boost::filesystem::exists(filePath);
    }
    void importPrivateKey(string strSecret, string strLabel = "")
    {
        CBitcoinSecret vchSecret;
        bool fGood = vchSecret.SetString(strSecret);
        if (!fGood) throw std::runtime_error("Invalid private key");
        CKey key;
        bool fCompressed;
        CSecret secret = vchSecret.GetSecret(fCompressed);
        key.SetSecret(secret, fCompressed);
        CKeyID vchAddress = key.GetPubKey().GetID();
        {
            LOCK2(cs_main, pwalletMain->cs_wallet);

            pwalletMain->MarkDirty();
            pwalletMain->SetAddressBookName(vchAddress, strLabel);

            if(pwalletMain->AddKey(key))
                throw std::runtime_error("Error adding key to wallet. It maybe already imported.");

            pwalletMain->ScanForWalletTransactions(pindexGenesisBlock, true);
            pwalletMain->ReacceptWalletTransactions();
        }

        MainFrameRepaint();
    }

public:
    int nLoadWallet;
    WalletManager(QString walletFile)
    {
        boost::filesystem::path full_path( boost::filesystem::current_path() );
        if(!fileExists(walletFile.toStdString()))
            throw std::runtime_error("File doesn't exists. Please make sure to use correct path.");
        bool fFirstRun;
        tempWallet = new CWallet(walletFile.toStdString());
        nLoadWallet = tempWallet->LoadWallet(fFirstRun);
    }
    bool isMainWalletLocked()
    {
        return pwalletMain->IsLocked();
    }
    void import()
    {
        qDebug() << "Its Running";
//        Object response;
//        Object result;
//        response.push_back(Pair("status", "success"));
//        std::map<CTxDestination, std::string> addressBook = tempWallet->mapAddressBook;
        qDebug() << "Opening Loop";
        BOOST_FOREACH(const PAIRTYPE(CBitcoinAddress, string)& item, tempWallet->mapAddressBook)
        {
            qDebug("Loop is running");
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
                //qDebug("Importing private key");
                try
                {
                    importPrivateKey(keySecret.ToString(), strAccount + "-Old");
                }
                catch(const runtime_error& error)
                {
//                    keySecret.ToString() << "::" << strAccount
                                            //QString::fromStdString("KeySecret: ") + QString::fromStdString(keySecret)
                    //qDebug() << "KeySecret: " << keySecret.ToString().c_str();
                    //qDebug() << "Account: " << strAccount.c_str();
                }
//                importPrivateKey(keySecret.ToString(), strAccount + "-Old");

 //               qDebug("Process Request: Private key Imported Successfully");
  //              result.push_back(Pair(strAccount, address.ToString()));
            }
        }
//        response.push_back(Pair("result", result));
    }
    bool isLocked()
    {
        return tempWallet->IsLocked();
    }
    bool unLockWallet(QString passPhrase)
    {
        SecureString strWalletPass;
        strWalletPass.reserve(100);
        strWalletPass = passPhrase.toStdString().c_str();
        return tempWallet->Unlock(strWalletPass);
    }
};
class Sleeper : public QThread
{
public:
    static void usleep(unsigned long usecs){QThread::usleep(usecs);}
    static void msleep(unsigned long msecs){QThread::msleep(msecs);}
    static void sleep(unsigned long secs){QThread::sleep(secs);}
};
#endif //IMPORTWALLETDIALOG
