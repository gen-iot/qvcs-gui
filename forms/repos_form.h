#pragma once

#include <QMainWindow>

namespace Ui {
    class repos_view_controller;
}

namespace vcs::form {
    class repos_form : public QMainWindow {
    Q_OBJECT

    public:
        explicit repos_form(QWidget *parent = nullptr);

        ~repos_form() override;

    private:
        void ui_init();

        void ui_setup_toolbar();

        void ui_setup_table();

        void ui_setup_ctx_menu();

        //
        void new_repo();

        void load_repos();

        void open_settings();

        void del_selected_repo();

        //
        void item_selection_changed();

    private:
        Ui::repos_view_controller *vc_;
        QMenu *table_ctx_menu_;

    };
}

