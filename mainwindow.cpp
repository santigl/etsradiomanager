#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    parser_(NULL),
    status_message(new QLabel(this)),
    last_directory_(QDir::homePath()),
    changes_made_(false)
{
    ui->setupUi(this);

    // Window size.
    this->resize(QDesktopWidget().availableGeometry(this).size() * 0.60);

    // Displaying status message (no file is opened yet).
    status_message->setText(tr("No file opened."));
    ui->statusBar->addPermanentWidget(status_message);

    // Setting column labels.
    ui->dataTable->setColumnCount(2);
    QTableWidgetItem *name  = new QTableWidgetItem(tr("Description"));
    QTableWidgetItem *url   = new QTableWidgetItem(tr("URL"));
    ui->dataTable->setHorizontalHeaderItem(DESC_COL, name);
    ui->dataTable->setHorizontalHeaderItem(URL_COL, url);
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionAbout_triggered()
{
    AboutDialog a(this);
    a.exec();
}


void MainWindow::resizeEvent(QResizeEvent *) {
  ui->dataTable->setColumnWidth(DESC_COL, this->width()/2);
  ui->dataTable->setColumnWidth(URL_COL,  this->width()/2);
}


void MainWindow::on_actionOpen_triggered()
{
    // WARN USER IF CHANGES WERE MADE.
    if (this->changes_made_) {
        int ret = saveChangesPrompt();
        if (ret == QMessageBox::Cancel) {
            return;
        }
        if (ret == QMessageBox::Yes) {
                this->parser_->saveStreams();
                clearChangesMade();
        }
    }

    // Checking if we already visited a directory.
    QString starting_directory;
    if (this->last_directory_ != QDir::homePath()) {
        starting_directory = this->last_directory_.path();
    }

    // Creating a file dialog and getting the path to a valid file.
    QString file_name =
        QFileDialog::getOpenFileName(this,
                                     tr("Open Image"),
                                     starting_directory,
                                     tr(".sii files (*.sii)"));

    // Unsuccessful opening?
    if (file_name == "") {
        return;
    }

    this->opened_file_      = file_name;
    this->last_directory_   = QDir(file_name); // Saving directory for future accesses.
    clearChangesMade();


    // Checking if a file was already loaded.
    // If that is the case, we clean up the current data:
    if (this->parser_ != NULL) {
        delete this->parser_;
    }
    // Creating new parser:
    this->parser_ = new Parser(file_name);


    // Populating the table...
    ui->dataTable->clearContents();

    StreamList::const_iterator it;
    unsigned int row = 0;
    for (it = this->parser_->streamsBegin(); it != this->parser_->streamsEnd(); it++) {
        ui->dataTable->insertRow(row);

        QTableWidgetItem *name = new QTableWidgetItem(it->name);
        ui->dataTable->setItem(row, DESC_COL, name);

        QTableWidgetItem *url = new QTableWidgetItem(it->url);
        ui->dataTable->setItem(row, URL_COL, url);
        row++;
    }

    ui->dataTable->setRowCount(row);
    ui->dataTable->setEnabled(true);
    // Status message.
    ui->statusBar->showMessage(QString(tr("Loaded "))+QString::number(row)+QString(tr(" URLs.")));
    // Enabling buttons:
    ui->actionSave_As->setEnabled(true);
    ui->insertNew->setEnabled(true);
}


void MainWindow::on_dataTable_itemSelectionChanged()
{ // Update line edits with selected content.
    // Read selected info:
    QList<QTableWidgetItem*> items  = ui->dataTable->selectedItems();

    if (!items.empty()) {
        QTableWidgetItem* url       = items.at(URL_COL);
        QTableWidgetItem* name      = items.at(DESC_COL);
        // Updating line edits...
        ui->urlEdit->setText(url->text());
        ui->nameEdit->setText(name->text());
        // ... and enabling them:
        ui->urlEdit->setEnabled(true);
        ui->nameEdit->setEnabled(true);

        // Disable save button (until a change is made):
        ui->saveEdit->setEnabled(false);
        // Enable remove button:
        ui->remove->setEnabled(true);

        // Enable move icons:
        ui->moveUp->setEnabled(true);
        ui->moveDown->setEnabled(true);
    }
    else { // There is nothing to edit.
        ui->urlEdit->setEnabled(false);
        ui->nameEdit->setEnabled(false);
        ui->nameEdit->clear();
        ui->urlEdit->clear();
        // Disable move icons:
        ui->moveUp->setEnabled(false);
        ui->moveDown->setEnabled(false);
        // Disable remove button:
        ui->remove->setEnabled(false);
    }
}


void MainWindow::entryEdited(){
// Called by nameEdit and urlEdit when textChanged.
    ui->saveEdit->setEnabled(true);
}


void MainWindow::on_moveUp_clicked()
{
    QTableWidgetSelectionRange selected = ui->dataTable->selectedRanges().first();
    unsigned int selected_row = selected.topRow();

    if (selected_row > 0) {
        swapItems(selected_row-1, selected_row);
        // Changing selection to "chase" the item:
        ui->dataTable->selectRow(selected_row-1);
    }
}


void MainWindow::on_moveDown_clicked()
{
    QTableWidgetSelectionRange selected = ui->dataTable->selectedRanges().first();
    int selected_row = selected.topRow();

    if (selected_row < ui->dataTable->rowCount()-1) {
        swapItems(selected_row, selected_row+1);
        // Changing selection to "chase" the item:
        ui->dataTable->selectRow(selected_row+1);
    }
}


void MainWindow::swapItems(unsigned int a, unsigned int b)
{// This function will call parser.swap() and update the view.

    // Swapping elements in the structure.
    this->parser_->swapStreams(a, b);

    // Updating the view:
    StreamList::iterator it(parser_->streamsBegin());
    ui->dataTable->item(a, URL_COL)->setText(it[a].url);
    ui->dataTable->item(a, DESC_COL)->setText(it[a].name);
    ui->dataTable->item(b, URL_COL)->setText(it[b].url);
    ui->dataTable->item(b, DESC_COL)->setText(it[b].name);

    setChangesMade();
}


void MainWindow::setChangesMade()
{
    if (!changes_made_) {
        this->changes_made_ = true;
        // Add a symbol next to the path to show there are unsaved changes:
        QString path = status_message->text();
        status_message->setText(path + " *");
        // Enable save button:
        ui->actionSave->setEnabled(true);
    }
}


void MainWindow::clearChangesMade()
{
    this->changes_made_ = false;
    // Clear the pending-changes symbol:
    status_message->setText(this->opened_file_);
    // Disable save button:
    ui->actionSave->setEnabled(false);

}


void MainWindow::on_actionSave_triggered()
{
    bool saved = this->parser_->saveStreams();
    if (!saved) {
        QMessageBox error;
        error.setIcon(QMessageBox::Warning);
        error.setText(tr("Error saving file."));
        error.exec();
        return;
    }
    clearChangesMade();
}


void MainWindow::on_actionSave_As_triggered()
{

    // Checking if we already visited a directory.
    QString starting_directory;
    if (this->last_directory_ != QDir::homePath()) {
        starting_directory = this->last_directory_.path();
    }

    // Creating a file dialog and getting the path to a valid file.
    QString file_name =
        QFileDialog::getSaveFileName(this,
                                     tr("Open Image"),
                                     starting_directory,
                                     tr(".sii files (*.sii)"));

    if (file_name != "") {
        bool saved = this->parser_->saveStreams(file_name);
        if (!saved) {
            QMessageBox error;
            error.setIcon(QMessageBox::Warning);
            error.setWindowTitle(tr("ETS Radio Manager"));
            error.setText(tr("Error saving file."));
            error.exec();
            return;
        }
        this->opened_file_ = file_name;
        this->last_directory_ = QDir(file_name); // Saving directory for future accesses.
        clearChangesMade();
    }
}


int MainWindow::saveChangesPrompt() {
    QMessageBox mBox;
    mBox.setWindowTitle(tr("ETS Radio Manager"));
    mBox.setText(tr("The file has been modified."));
    mBox.setInformativeText(tr("Do you want to save your changes?"));
    mBox.setIcon(QMessageBox::Warning);
    mBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    mBox.setDefaultButton(QMessageBox::Yes);

    return mBox.exec();
}


void MainWindow::closeEvent(QCloseEvent* event)
{
    // No changes made => exit.
    if (!this->changes_made_) {
        event->accept();
        return;
    }

    // Ask to save changes:
    int res = saveChangesPrompt();
    if (res == QMessageBox::Cancel) {   // Do not exit yet.
        event->ignore();
        return;
    }

    if (res == QMessageBox::Yes) {
        if (!this->parser_->saveStreams()) {
            QMessageBox error;
            error.setWindowTitle(tr("ETS Radio Manager"));
            error.setIcon(QMessageBox::Critical);
            error.setText(tr("Error saving file."));
            error.exec();
        }
    }

    event->accept();
}


void MainWindow::on_saveEdit_clicked()
{
    QTableWidgetSelectionRange selected = ui->dataTable->selectedRanges().first();
    int row = selected.topRow();
    // Getting iterator to the item:
    StreamList::iterator it = this->parser_->streamsBegin();
    // Modifying data:
    it[row].name    = ui->nameEdit->text();
    it[row].url     = ui->urlEdit->text();
    // Updating view:
    ui->dataTable->item(row, URL_COL)->setText(it[row].url);
    ui->dataTable->item(row, DESC_COL)->setText(it[row].name);
    // Disabling confirm-edit button:
    ui->saveEdit->setEnabled(false);
    setChangesMade();

}


void MainWindow::on_insertNew_clicked()
{
    InsertDialog i(this);
    int res = i.exec();
    if (res == QDialog::Accepted ) {
        Stream ns (i.getUrl(), i.getName());
        this->parser_->insertStream(ns);

        // Creating table item:
        QTableWidgetItem* url_i     = new QTableWidgetItem(ns.url);
        QTableWidgetItem* name_i    = new QTableWidgetItem(ns.name);

        // Inserting into table:
        int row = ui->dataTable->rowCount();
        ui->dataTable->insertRow(row);
        ui->dataTable->setItem(row, URL_COL, url_i);
        ui->dataTable->setItem(row, DESC_COL, name_i);
        this->setChangesMade();
    }
}


void MainWindow::on_remove_clicked()
{ // PRE: A row is selected.

    QTableWidgetSelectionRange selected = ui->dataTable->selectedRanges().first();
    int row = selected.topRow();

    this->parser_->deleteStream(row);
    ui->dataTable->removeRow(row);
    setChangesMade();
}
