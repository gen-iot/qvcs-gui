//
// Created by suzhen on 2019/11/15.
//

#pragma once

#include <QDialog>

namespace Ui {
    class settings_view_controller;
}

class QAbstractButton;

namespace vcs::form {
    class settings_dialog : public QDialog {
    Q_OBJECT
    public:
        explicit settings_dialog(QWidget *parent = nullptr,
                                 Qt::WindowFlags f = Qt::WindowFlags());

        ~settings_dialog() override;

    private:
        void ui_init();

        void grouped_button_clicked(QAbstractButton *btn);

    private:
        Ui::settings_view_controller *vc_;
    };
}



