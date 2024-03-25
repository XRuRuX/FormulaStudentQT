#include "ProgressDialog.h"

ProgressDialog::ProgressDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ProgressDialog), canceled(false) // Initialize 'canceled' with false
{
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this, [this]() {
        // Set 'canceled' to true when the user clicks 'Cancel'
        canceled = true;
        reject();
    });
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::setProgress(int value)
{
    ui->progressBar->setValue(value);
}

bool ProgressDialog::wasCanceled() const // Implement the method
{
    return canceled;
}
