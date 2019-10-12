#include "repo_new_dialog.h"
#include "ui_repo_new_dialog.h"
#include <QDialogButtonBox>
#include <QMessageBox>
#include "repos_form.h"

namespace vcs::form {

    repo_new_dialog::repo_new_dialog(QWidget *parent) :
            QDialog(parent),
            vc_(new Ui::repo_new_view_controller) {
        vc_->setupUi(this);
        ui_init();
    }

    repo_new_dialog::~repo_new_dialog() {
        delete vc_;
    }

    void repo_new_dialog::ui_init() {
        this->setWindowTitle("Create New Repo");
        this->layout()->setSizeConstraint(QLayout::SetFixedSize);
        QObject::connect(vc_->btn_box,
                         &QDialogButtonBox::accepted,
                         [this]() {
                             QString repo_name = vc_->text_repo_name->text();
                             if (repo_name.length() == 0) {
                                 QMessageBox::critical(this, "error", "repo name is empty ?");
                                 return;
                             }

                             QString repo_desc = vc_->text_repo_desc->toPlainText();
                             if (repo_desc.length() == 0) {
                                 QMessageBox::critical(this, "error", "repo desc is empty ?");
                                 return;
                             }
                             this->new_repo(repo_name, repo_desc);
                             this->close();
                         });

        QObject::connect(vc_->btn_box,
                         &QDialogButtonBox::rejected,
                         this, &repo_new_dialog::close);
    }


}
