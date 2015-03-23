#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QLabel>
#include <QDir>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QCloseEvent>

#include "aboutdialog.h"
#include "insertdialog.h"
#include "parser.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionAbout_triggered();

    void on_actionOpen_triggered();

    void on_dataTable_itemSelectionChanged();

    void resizeEvent(QResizeEvent *);

    void on_moveUp_clicked();

    void on_moveDown_clicked();

    void on_actionSave_triggered();

    void on_actionSave_As_triggered();

    void entryEdited();

    void closeEvent(QCloseEvent*);

    void on_saveEdit_clicked();

    void on_insertNew_clicked();

    void on_remove_clicked();


private:
    Ui::MainWindow *ui;
    Parser* parser_;

    // Right-hand side message.
    QLabel* status_message;
    // Last directory from which a file was opened/saved.
    QDir last_directory_;
    // Currently opened file.
    QString opened_file_;

    bool changes_made_;

    void setChangesMade();
    void clearChangesMade();
    int saveChangesPrompt();
    enum ColumnInfo {DESC_COL=0, URL_COL=1};
    void swapItems(unsigned int, unsigned int);

};

#endif // MAINWINDOW_H
