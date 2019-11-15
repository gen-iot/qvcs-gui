//
// Created by suzhen on 2019/11/15.
//

#include "settings_dialog.h"
#include "ui_settings_dialog.h"
#include <QDialogButtonBox>
#include <QAbstractButton>
#include <QMessageBox>
#include <QRegExp>
#include "net/api.h"
#include "utils.h"

namespace vcs::form {

    settings_dialog::settings_dialog(QWidget *parent,
                                     Qt::WindowFlags f) :
            QDialog(parent, f),
            vc_(new Ui::settings_view_controller) {
        vc_->setupUi(this);
        utils::dialog_disable_help(this);
        ui_init();
    }

    settings_dialog::~settings_dialog() {
        delete vc_;
    }

    void settings_dialog::ui_init() {
        setWindowTitle("Settings");
        vc_->txt_api_url->setText(api::api_url_get());
        QObject::connect(vc_->button_box, &QDialogButtonBox::clicked,
                         this, &settings_dialog::grouped_button_clicked);
    }

    void settings_dialog::grouped_button_clicked(QAbstractButton *btn) {
        using button_role_t = QDialogButtonBox::ButtonRole;
        button_role_t role = vc_->button_box->buttonRole(btn);
        if (role != button_role_t::AcceptRole) {
            return;
        }
        QString api_url = vc_->txt_api_url->text();
        QRegExp regex("^(http|https):\\/\\/.*");
        if (!regex.exactMatch(api_url)) {
            QMessageBox::critical(this, "Error", "bad api_url");
            return;
        }
        api::api_url_set(api_url);
        close();
    }
}