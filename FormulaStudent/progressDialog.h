#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include "ui_progressdialog.h"

namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(QWidget *parent = nullptr);
    ~ProgressDialog();

public:
    void setProgress(int value);
    bool wasCanceled() const;

private:
    bool canceled;

private:
    Ui::ProgressDialog *ui;
};

#endif // PROGRESSDIALOG_H
