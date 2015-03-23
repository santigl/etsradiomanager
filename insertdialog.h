#ifndef INSERTDIALOG_H
#define INSERTDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>

namespace Ui {
class InsertDialog;
}

class InsertDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InsertDialog(QWidget *parent = 0);
    ~InsertDialog();
    QString getName() const;
    QString getUrl() const;

private slots:
    void on_buttonBox_accepted();

    void on_insertName_textChanged(const QString &arg1);

    void on_insertUrl_textChanged(const QString &arg1);

private:
    Ui::InsertDialog *ui;
    QString name_;
    QString url_;
};

#endif // INSERTDIALOG_H

