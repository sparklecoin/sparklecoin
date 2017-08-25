#ifndef WALLETPASSPHRASEDIALOG_H
#define WALLETPASSPHRASEDIALOG_H

#include <QDialog>

namespace Ui {
    class walletPassPhraseDialog;
}

class walletPassPhraseDialog : public QDialog
{
    Q_OBJECT
    public:
        explicit walletPassPhraseDialog(QWidget *parent = 0);
        ~walletPassPhraseDialog();
        void accept();
    private:
        Ui::walletPassPhraseDialog *ui;
};

#endif // WALLETPASSPHRASEDIALOG_H
