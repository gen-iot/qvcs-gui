//
// Created by suzhen on 2019/10/12.
//

#pragma once

#include <QProgressDialog>

namespace vcs::form {
    class LiteQProgressDialog : public QProgressDialog {
    Q_OBJECT

    public:
        explicit LiteQProgressDialog(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    public:
        Q_SLOT
        void on_progress(long total, long cur);
    };
}


