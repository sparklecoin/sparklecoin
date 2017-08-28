#include "importwalletdialog.h"
#include "ui_importwalletdialog.h"
#include <QMovie>
#include <QtSvg/QSvgRenderer>
#include <QtSvg/QtSvg>
#include <QWidget>
#include <QtSvg/QGraphicsSvgItem>
#include <QLayout>
#include <walletpassphrasedialog.h>
#include <QtConcurrentRun>
#include <bitcoingui.h>

ImportWalletDialog::ImportWalletDialog(QWidget *parent, QString walletFilePath) : QDialog(parent), ui(new Ui::ImportWalletDialog)
{
    walletFile = walletFilePath;
    ui->setupUi(this);
    ui->successlabel->setVisible(false);
    ui->finishbutton->setVisible(false);
    ui->finishbutton->setStyleSheet("padding: 10px 20px; font-weight: 600;");
    connect(this, SIGNAL(failed(QString)), this, SLOT(failHandler(QString)));
    connect(this, SIGNAL(getPassPhrase()), this, SLOT(passPhraseDialog()));
    connect(this, SIGNAL(success()), this, SLOT(successHandler()));
    setWindowFlags(Qt::Dialog | Qt::Desktop | Qt::FramelessWindowHint);
    QString imagePath = ":/icons/svg-loader";
    image = new QSvgWidget(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignCenter);
    image->load(imagePath);
    image->setGeometry(160, 90, 80, 80);
    image->setLayout(layout);
//    setWindowModality(Qt::WindowModal);
//    this->show();
//    this->exec();
//    ImportWalletRequest();
    QFuture<void> future = QtConcurrent::run(this, &ImportWalletDialog::ImportWalletRequest);
}
void ImportWalletDialog::successHandler()
{
    ui->importWalletLabel->setVisible(false);
    ui->successlabel->setVisible(true);
    QString path = ":/icons/success-check";
    image->load(path);
    ui->finishbutton->setVisible(true);
}
void ImportWalletDialog::show()
{
    QDialog::show();
//    ImportWalletRequest();
    //    QtConcurrent::run(new ImportWalletDialog((BitcoinGUI*)this->parentWidget(), walletFile), &ImportWalletDialog::ImportWalletRequest);

//    QtConcurrent::run(this, &ImportWalletDialog::ImportWalletRequest);
//    QMetaObject::invokeMethod(this, &QDialog::exec);
//    QApplication::postEvent( this, new QEvent(QEvent::e) );
//    ImportWalletRequest();
//    return 3;
}

void ImportWalletDialog::postClose()
{
    QApplication::postEvent( this, new QCloseEvent() );
}
void ImportWalletDialog::passPhraseDialog()
{
    walletPassPhraseDialog dlg(this);
    dlg.exec();
}
void ImportWalletDialog::ImportWalletRequest()
{
    try
    {
        wallet =  new WalletManager(walletFile);
        if(wallet->isMainWalletLocked())
        {
            throw runtime_error("Your Sparklecoin wallet is Locked. Please unlock wallet to continue");
        }
        else if(fWalletUnlockMintOnly)
        {
            throw runtime_error("Your Sparklecoin wallet is unlocked for minting only. Please unlock wallet completely to import wallet file.");
        }
        if(wallet->isLocked())
        {
            emit getPassPhrase();
//            walletPassPhraseDialog dlg(this);
//            dlg.exec();
        }
        else
        {
            this->processRequest();
//            QFuture<void> future = QtConcurrent::run(this, &ImportWalletDialog::processRequest);
            /*
            try
            {
                future.waitForFinished();
            }
            catch(const std::exception& error)
            {
                this->error("Error", error.what(), true);
                this->postClose();
            }*/
        }
    }
    catch(const runtime_error& error)
    {
        emit failed(QString::fromStdString(error.what()));
//        this->error("Error", error.what(), true);
        this->postClose();
    }
    catch(const DbException& error)
    {
        emit failed(QString::fromStdString("Invalid Wallet File"));
//        this->error("Error","Invalid Wallet File", true);
        this->postClose();
    }
}
void ImportWalletDialog::error(const char* title, const char* message, bool modal)
{
    std::string title_str(title);
    std::string message_str(message);
    error(title_str, message_str, modal);
}
void ImportWalletDialog::error(std::string title, std::string message, bool modal)
{
    error(QString::fromStdString(title), QString::fromStdString(message), modal);
}
void ImportWalletDialog::error(QString title, QString message, bool modal)
{
    ((BitcoinGUI*) this->parentWidget())->error(title, message, modal);
}

void ImportWalletDialog::processRequest(QString passPhrase_str)
{
    if(wallet->unLockWallet(passPhrase_str))
    {
        QFuture<void> future = QtConcurrent::run(this, &ImportWalletDialog::processRequest);
        /*

        try
        {
            future.waitForFinished();
        }
        catch(const std::exception& exec)
        {
            error("Error", exec.what(), true);
            this->postClose();
        }*/
    }
    else
    {
        error("Error","Invalid Passphrase for the wallet.", true);
        this->postClose();
    }
}
void ImportWalletDialog::processRequest()
{
    try
    {
        Sleeper::sleep(2);
        if (wallet->nLoadWallet != DB_LOAD_OK)
        {
            if (wallet->nLoadWallet == DB_CORRUPT)
                throw runtime_error("Error loading "+(walletFile.toStdString())+": Wallet corrupted \n");
            else if (wallet->nLoadWallet == DB_TOO_NEW)
                throw runtime_error("Error loading "+(walletFile.toStdString())+": Wallet requires newer version of Sparkleunity\n");
            else if (wallet->nLoadWallet == DB_NEED_REWRITE)
                throw runtime_error((walletFile.toStdString())+" file is incomplete \n");
            else
                throw runtime_error("Error Loading wallet file "+(walletFile.toStdString())+" \n");
        }
        wallet->import();
        emit success();
    }
    catch(const std::exception& error)
    {
        emit failed(QString::fromStdString(error.what()));
//        this->error("Error",error.what(), true);
    }

/*
 *
 *
    }
   catch(const runtime_error& error)
    {
        QString title = "Error";
        QString message = error.what();
        qDebug(message.toStdString().c_str());
//        ((BitcoinGUI*)this->parentWidget())->error(title, message, true);
        this->postClose();
    }
    catch(const std::exception& exec)
    {
        QString title = "Error";
        QString message = exec.what();
        qDebug(message.toStdString().c_str());
//        ((BitcoinGUI*)this->parentWidget())->error(title, message, true);
        this->postClose();
    }*/

}

ImportWalletDialog::~ImportWalletDialog()
{
    delete ui;
}
