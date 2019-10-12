//
// Created by suzhen on 2019/10/12.
//

#include "versions_new_dialog.h"
#include "ui_versions_new_dialog.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>
#include <QPushButton>
#include <QMessageBox>
#include <QDialogButtonBox>

namespace vcs::form {

    versions_new_dialog::versions_new_dialog(QWidget *parent, Qt::WindowFlags wflag) :
            QDialog(parent, wflag),
            vc_(new Ui::versions_new_view_controller) {
        vc_->setupUi(this);
        ui_init();
    }

    versions_new_dialog::~versions_new_dialog() {
        delete vc_;
    }

    void versions_new_dialog::open_chose_file() {
        QFileDialog dialog(this);
        dialog.setAcceptMode(QFileDialog::AcceptOpen);
        dialog.setWindowTitle("Chose Upload File");
        dialog.setFileMode(QFileDialog::ExistingFile);
        QStringList desktop_loc = QStandardPaths::standardLocations(QStandardPaths::StandardLocation::DesktopLocation);
        dialog.setDirectory(!desktop_loc.empty() ? desktop_loc.first() : "");
        QObject::connect(&dialog, &QFileDialog::fileSelected, [this](const QString &file_selected) {
            if (file_selected.isEmpty()) {
                return;
            }
            vc_->txt_file->setText(file_selected);
        });
        dialog.exec();
    }

    void versions_new_dialog::ui_init() {
        setWindowTitle("Create New Version");
        QObject::connect(vc_->btn_chose_file, &QPushButton::clicked,
                         this, &versions_new_dialog::open_chose_file);
        QObject::connect(vc_->button_box, &QDialogButtonBox::clicked,
                         this, &versions_new_dialog::grouped_button_clicked);
    }

    void versions_new_dialog::grouped_button_clicked(QAbstractButton *btn) {
        using button_role_t = QDialogButtonBox::ButtonRole;
        button_role_t role = vc_->button_box->buttonRole(btn);
        switch (role) {
            case button_role_t::AcceptRole: {
                if (validate_inputs()) {
                    close();
                }
            }
                break;
            case button_role_t::RejectRole: {
                close();
            }
                break;
            case button_role_t::ResetRole: {
                // restore ui to default
                vc_->txt_ver_name->clear();
                vc_->txt_file->clear();
                vc_->txt_ver_desc->clear();
                vc_->cb_head->setChecked(true);
            }
                break;
            default:
                break;
        }
    }

    bool versions_new_dialog::validate_inputs() {
        QString ver_name = vc_->txt_ver_name->text();
        if (ver_name.isEmpty()) {
            QMessageBox::critical(this, "Error", "version name is required");
            return false;
        }
        QString ver_file_loc = vc_->txt_file->text();
        if (ver_file_loc.isEmpty()) {
            QMessageBox::critical(this, "Error", "version file is required");
            return false;
        }
        if (!QFile::exists(ver_file_loc)) {
            QMessageBox::critical(this, "Error", "version file not exist");
            return false;
        }
        QString ver_desc = vc_->txt_ver_desc->toPlainText();
        if (ver_desc.isEmpty()) {
            QMessageBox::critical(this, "Error", "version desc is required");
            return false;
        }
        new_version_ready(ver_name, ver_file_loc, ver_desc, vc_->cb_head->isChecked());
        return true;
    }

}