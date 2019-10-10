#ifndef NEW_REPO_DIALOG_H
#define NEW_REPO_DIALOG_H

#include <QDialog>
#include <functional>

namespace Ui {
    class new_repo_view_controller;
}

namespace vcs::form {

    using new_repo_callback =std::function<void(const QString &name, const QString &desc)>;

    class new_repo_dialog : public QDialog {
    Q_OBJECT

    public:
        explicit new_repo_dialog(QWidget *parent = nullptr);

        ~new_repo_dialog() override;

    Q_SIGNALS:

        void new_repo(const QString &name, const QString &desc);

    private:
        void init();


    private:
        Ui::new_repo_view_controller *vc_;
    };
}


#endif // NEW_REPO_DIALOG_H
