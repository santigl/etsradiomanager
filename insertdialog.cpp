#include "insertdialog.h"
#include "ui_insertdialog.h"

InsertDialog::InsertDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InsertDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

InsertDialog::~InsertDialog()
{
    delete ui;
}

QString InsertDialog::getName() const
{
    return name_;
}

QString InsertDialog::getUrl() const
{
    return url_;
}

void InsertDialog::on_buttonBox_accepted()
{
    url_    = ui->insertUrl->text();
    name_   = ui->insertName->text();
}

void InsertDialog::on_insertName_textChanged(const QString &arg1)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(
                !(arg1.isEmpty() || ui->insertUrl->text().isEmpty())
                );
}

void InsertDialog::on_insertUrl_textChanged(const QString &arg1)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(
                !(arg1.isEmpty() || ui->insertName->text().isEmpty())
                );
}
