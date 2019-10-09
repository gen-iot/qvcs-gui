#include "new_repo_dialog.h"
#include "ui_new_repo_dialog.h"
#include <QDialogButtonBox>
#include <QMessageBox>
#include "main_window.h"

namespace vcs::form {

new_repo_dialog::new_repo_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::new_repo_dialog)
{
    ui->setupUi(this);
    init();
}

new_repo_dialog::~new_repo_dialog()
{
    delete ui;
}

void new_repo_dialog::init()
{
    this->setWindowTitle("new repo");
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    QObject::connect(ui->btn_box,
                     &QDialogButtonBox::accepted,
                     [this](){ 
        QString repo_name =ui->text_repo_name->text();
        if (repo_name.length() == 0){
            QMessageBox::critical(this,"error","repo name is empty ?");
            return;
        }
        
        QString repo_desc = ui->text_repo_desc->toPlainText();
        if (repo_desc.length() == 0){
            QMessageBox::critical(this,"error","repo desc is empty ?");
            return;
        }
        this->new_repo(repo_name,repo_desc);
        this->close();
    });
    
    QObject::connect(ui->btn_box,
                     &QDialogButtonBox::rejected,
                     this,&new_repo_dialog::close);
}



}
