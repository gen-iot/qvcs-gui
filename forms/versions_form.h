#pragma once

#include <QMainWindow>

namespace Ui {
    class versions_view_controller;
}

namespace vcs::forms {

    class versions_form : public QMainWindow {
    Q_OBJECT

    public:
        explicit versions_form(QWidget *parent = nullptr);

        ~versions_form() override;

    private :
        Ui::versions_view_controller *vc_;

    };

}


