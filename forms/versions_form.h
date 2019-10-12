#pragma once

#include <QMainWindow>
#include "net/http.h"

namespace Ui {
    class versions_view_controller;
}

namespace vcs::form {

    class versions_form : public QMainWindow {
    Q_OBJECT

    public:
        explicit versions_form(const QString &repo_name, QWidget *parent = nullptr);

        ~versions_form() override;

    private:
        void ui_init();

        void ui_setup_table();

        void ui_setup_toolbar();

        void load_versions();

        void show_create_version();

        void version_created(const QString &ver_code,
                             const QString &ver_file_loc,
                             const QString &ver_desc,
                             bool isHead);

        void version_create_result(int err, http::status_code_t status_code, const QByteArray &server_response);

    private :
        Ui::versions_view_controller *vc_;
        QString repo_name_;

    };

}


