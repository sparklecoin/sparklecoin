#ifndef OVERVIEWPAGE_H
#define OVERVIEWPAGE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

namespace Ui {
    class OverviewPage;
}
class WalletModel;
class TxViewDelegate;

/** Overview ("home") page widget */
class OverviewPage : public QWidget
{
    Q_OBJECT

public:
    explicit OverviewPage(QWidget *parent = 0);
    ~OverviewPage();

    void setModel(WalletModel *model);

public slots:
    void setBalance(qint64 available, qint64 maturing, qint64 unconfirmed);
    void setNumTransactions(int count);

signals:
    void transactionClicked(const QModelIndex &index);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    Ui::OverviewPage *ui;
    WalletModel *model;
    qint64 currentAvailable;
    qint64 currentMaturing;
    qint64 currentUnconfirmed;

    TxViewDelegate *txdelegate;
    unsigned int numDisplayedTransactions();

private slots:
    void displayUnitChanged();
};

#endif // OVERVIEWPAGE_H
