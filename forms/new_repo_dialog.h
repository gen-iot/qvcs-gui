#ifndef NEW_REPO_DIALOG_H
#define NEW_REPO_DIALOG_H

#include <QDialog>
#include <functional>

namespace Ui {
class new_repo_dialog;
}

namespace vcs::form {

using new_repo_callback =std::function<void(const QString& name , const QString& desc)>;

class new_repo_dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit new_repo_dialog(QWidget *parent=nullptr);
    ~new_repo_dialog();
    
    Q_SIGNALS:
    void new_repo(const QString& name , const QString& desc);
    
private:
    void init();
    
    
private:
    Ui::new_repo_dialog *ui;
};
}


#endif // NEW_REPO_DIALOG_H
