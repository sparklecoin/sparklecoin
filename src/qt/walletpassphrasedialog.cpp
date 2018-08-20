#include "walletpassphrasedialog.h"
#include "ui_walletpassphrasedialog.h"

walletPassPhraseDialog::walletPassPhraseDialog(QWidget *parent) : QDialog(parent), ui(new Ui::walletPassPhraseDialog)
{
    ui->setupUi(this);
}
walletPassPhraseDialog::~walletPassPhraseDialog()
{
    delete ui;
}
void walletPassPhraseDialog::accept()
{
    //ImportWalletDialog *parent = (ImportWalletDialog*)parentWidget();
//    parent->passPhrase = ui->walletPassphrase->text();
    //parent->processRequest(ui->walletPassphrase->text());
    this->close();
}
