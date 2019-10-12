//
// Created by suzhen on 2019/10/12.
//

#include "lite_progress_dialog.h"
#include <QDebug>
#include <QThread>

namespace vcs::form {
    LiteQProgressDialog::LiteQProgressDialog(QWidget *parent, Qt::WindowFlags flags) :
            QProgressDialog(parent, flags) {
    }

    void LiteQProgressDialog::on_progress(long total, long cur) {
        if (this->maximum() != total) {
            this->setMaximum(int(total));
        }
        this->setValue(int(cur));
    }
}
