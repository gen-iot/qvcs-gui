#pragma once

#include <QDialog>
#include <functional>

namespace Ui {
    class repo_new_view_controller;
}

namespace vcs::form {

    class repo_new_dialog : public QDialog {
    Q_OBJECT

    public:
        explicit repo_new_dialog(QWidget *parent = nullptr);

        ~repo_new_dialog() override;

    Q_SIGNALS:

        void new_repo(const QString &name, const QString &desc);

    private:
        void ui_init();


    private:
        Ui::repo_new_view_controller *vc_;
    };
}

