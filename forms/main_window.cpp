#include "main_window.h"
#include "ui_main_window.h"
#include "net/api.h"
#include "net/http.h"
#include <algorithm>
#include <QAction>
#include <QIcon>
#include "new_repo_dialog.h"
#include <QDebug>
#include <QMessageBox>
#include <QMenu>
#include <QAbstractButton>

namespace vcs::form {

static const QString kTimeFormat("yyyy-MM-dd HH:mm:ss");

main_window::main_window(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::main_view_controller)
{
    ui_->setupUi(this);
    init_ui();
    load_repos();
}

main_window::~main_window()
{
    delete ui_;
}

void main_window::init_ui()
{
    this->setWindowTitle("Repo list");
    ui_setup_context_menu();
    ui_setup_toolbar();
    ui_setup_table();
}

void main_window::ui_setup_toolbar()
{
    //
    QAction *act_new = new QAction(QIcon(":/default/images/add_32px.ico"),"New",this);
    QObject::connect(act_new,&QAction::triggered,this,&main_window::new_repo);
    //
    QAction* act_reload = new QAction(QIcon(":/default/images/reload_32px.ico"),"Reload", this);
    QObject::connect(act_reload,&QAction::triggered,this,&main_window::load_repos);
    //
    ui_->tool_bar->addActions({act_new,act_reload});
    //
    // disbale toolbar context menu
    ui_->tool_bar->setContextMenuPolicy(Qt::PreventContextMenu);
}

void main_window::ui_setup_table()
{
    QStringList ls={"Name","Desc","CreateAt"};
    //    
    ui_->table_repos->setColumnCount(ls.size());
    ui_->table_repos->setHorizontalHeaderLabels(ls);
    ui_->table_repos->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeMode::ResizeToContents);    
    ui_->table_repos->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeMode::Stretch);    
    ui_->table_repos->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeMode::ResizeToContents);    
    ui_->table_repos->setSelectionBehavior(QTableView::SelectionBehavior::SelectRows);
    //
    ui_->table_repos->setShowGrid(true);
    ui_->table_repos->setEditTriggers(QTableWidget::NoEditTriggers);
    ui_->table_repos->setContextMenuPolicy(Qt::CustomContextMenu);
    //
    QObject::connect(ui_->table_repos,
                     &QTableWidget::customContextMenuRequested,
                     [this](const QPoint& table_pos){
        QPoint cur_pos = QCursor::pos();
        int row_idx = ui_->table_repos->rowAt(table_pos.y());  
        if (row_idx!=-1){
            table_menu_->exec(cur_pos);
        }
    });
}

void main_window::ui_setup_context_menu()
{
    table_menu_ = new QMenu(this);
    QAction* act_del_repo = new QAction("Delete",table_menu_);
    QObject::connect(act_del_repo,&QAction::triggered,this, &main_window::del_selected_repo);
    table_menu_->addAction(act_del_repo);
}

void main_window::new_repo()
{
    new_repo_dialog dig(this);
    QObject ::connect(&dig,&new_repo_dialog::new_repo,
                      [this](const QString& name, const QString& desc){
        int err = api::repo_create(name,desc);
        if(err){
            QMessageBox::critical(this,
                                  "error",
                                  QString("create repo \"%1\" failed err=%2")
                                  .arg(name).arg(err));
            return;
        }
        this->load_repos();
    });
    dig.exec();
    
}

void main_window::load_repos()
{   
    QList<api::repo> repos{};
    int err = api::repo_list(&repos);
    if(err){
        return;       
    }
    ui_->table_repos->clearContents();
    ui_->table_repos->setRowCount(repos.size());
    for(int i =0;i<repos.size();++i){
        const api::repo& repo = repos[i];
        ui_->table_repos->setItem(i,0,new QTableWidgetItem(repo.name));
        ui_->table_repos->setItem(i,1,new QTableWidgetItem(repo.desc));
        ui_->table_repos->setItem(i,2,new QTableWidgetItem(repo.createAt.toString(kTimeFormat)));
    }
}

void main_window::del_selected_repo()
{
    QModelIndexList rows = ui_->table_repos->selectionModel()->selectedRows();
    QStringList del_repos;
    std::transform(rows.cbegin(),rows.cend(),std::back_inserter(del_repos),[this](const QModelIndex& idx){
       QTableWidgetItem* item=  ui_->table_repos->item(idx.row(),0);
       return item->text();
    });
    QMessageBox box(QMessageBox::Icon::Question,
                    "are u sure?",
                    QString("those repos will be deleted:\n%1").arg(del_repos.join(",\n")),
                    QMessageBox::Ok| QMessageBox::Cancel,this);
    
    QObject::connect(&box,&QMessageBox::buttonClicked,[&,this](QAbstractButton *button){
        if ( box.standardButton(button) != QMessageBox::Ok){
            return;
        }
        // do delete
        std::for_each(del_repos.cbegin(),del_repos.cend(),[](const QString& repo_name){
            api::repo_drop(repo_name);
        });
        this->load_repos();
    });
    box.exec();    
}
}
