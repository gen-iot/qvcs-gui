//
// Created by suzhen on 2019/10/10.
//

#include "versions_form.h"
#include "ui_versions_form.h"

namespace vcs::forms {

    versions_form::versions_form(QWidget *parent) :
            QMainWindow(parent),
            vc_(new Ui::versions_view_controller) {
        vc_->setupUi(this);
    }

    versions_form::~versions_form() {
        delete vc_;
    }

}

