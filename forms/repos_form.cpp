#include "repos_form.h"
#include "ui_repo_form.h"
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

    repos_form::repos_form(QWidget *parent) :
            QMainWindow(parent),
            vc_(new Ui::repos_view_controller) {
        vc_->setupUi(this);
    init_ui();
    load_repos();
}

    repos_form::~repos_form() {
        delete vc_;
    }

    void repos_form::init_ui()
{
    this->setWindowTitle("Repo list");
    ui_setup_context_menu();
    ui_setup_toolbar();
    ui_setup_table();
}

    void repos_form::ui_setup_toolbar()
{
    //
    QAction *act_new = new QAction(QIcon(":/default/images/add_32px.ico"),"New",this);
    QObject::connect(act_new, &QAction::triggered, this, &repos_form::new_repo);
    //
    QAction* act_reload = new QAction(QIcon(":/default/images/reload_32px.ico"),"Reload", this);
    QObject::connect(act_reload, &QAction::triggered, this, &repos_form::load_repos);
    //
    vc_->tool_bar->addActions({act_new, act_reload});
    //
    // disbale toolbar context menu
    vc_->tool_bar->setContextMenuPolicy(Qt::PreventContextMenu);
}

    void repos_form::ui_setup_table()
{
    QStringList ls={"Name","Desc","CreateAt"};
    //    
    vc_->table_repos->setColumnCount(ls.size());
    vc_->table_repos->setHorizontalHeaderLabels(ls);
    vc_->table_repos->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::ResizeToContents);
    vc_->table_repos->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Stretch);
    vc_->table_repos->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::ResizeToContents);
    vc_->table_repos->setSelectionBehavior(QTableView::SelectionBehavior::SelectRows);
    //
    vc_->table_repos->setShowGrid(true);
    vc_->table_repos->setEditTriggers(QTableWidget::NoEditTriggers);
    vc_->table_repos->setContextMenuPolicy(Qt::CustomContextMenu);
    //
    QObject::connect(vc_->table_repos,
                     &QTableWidget::customContextMenuRequested,
                     [this](const QPoint& table_pos){
        QPoint cur_pos = QCursor::pos();
                         int row_idx = vc_->table_repos->rowAt(table_pos.y());
        if (row_idx!=-1){
            table_menu_->exec(cur_pos);
        }
    });
    QObject::connect(vc_->table_repos, &QTableWidget::itemDoubleClicked,
                     [this](QTableWidgetItem *item) {
                         qDebug() << "row double-clicked:" << item->row();
                     });
}

    void repos_form::ui_setup_context_menu()
{
    table_menu_ = new QMenu(this);
    QAction* act_del_repo = new QAction("Delete",table_menu_);
    QObject::connect(act_del_repo, &QAction::triggered, this, &repos_form::del_selected_repo);
    table_menu_->addAction(act_del_repo);
}

    void repos_form::new_repo()
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

    void repos_form::load_repos()
{   
    QList<api::repo> repos{};
    int err = api::repo_list(&repos);
    if(err){
        return;       
    }
    vc_->table_repos->clearContents();
    vc_->table_repos->setRowCount(repos.size());
    for(int i =0;i<repos.size();++i){
        const api::repo& repo = repos[i];
        vc_->table_repos->setItem(i, 0, new QTableWidgetItem(repo.name));
        vc_->table_repos->setItem(i, 1, new QTableWidgetItem(repo.desc));
        vc_->table_repos->setItem(i, 2, new QTableWidgetItem(repo.createAt.toString(kTimeFormat)));
    }
}

    void repos_form::del_selected_repo() {
        QModelIndexList rows = vc_->table_repos->selectionModel()->selectedRows();
    QStringList del_repos;
    std::transform(rows.cbegin(),rows.cend(),std::back_inserter(del_repos),[this](const QModelIndex& idx){
        QTableWidgetItem *item = vc_->table_repos->item(idx.row(), 0);
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
