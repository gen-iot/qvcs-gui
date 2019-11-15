//
// Created by suzhen on 2019/10/12.
//

#pragma once

#include <QDialog>

namespace Ui {
    class versions_new_view_controller;
}

class QAbstractButton;

namespace vcs::form {

    class versions_new_dialog : public QDialog {

    Q_OBJECT

    public:
        explicit versions_new_dialog(QWidget *parent = nullptr,
                                     Qt::WindowFlags f = Qt::WindowFlags());

        ~versions_new_dialog() override;

    Q_SIGNALS:

        void new_version_ready(const QString &ver_code,
                               const QString &ver_file_loc,
                               const QString &ver_desc,
                               bool isHead);

    private:

        void ui_init();

        void open_chose_file();

        void grouped_button_clicked(QAbstractButton *btn);

        bool validate_inputs();

    private:
        Ui::versions_new_view_controller *vc_;
    };

}



