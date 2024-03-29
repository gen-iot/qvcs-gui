#include "repos_form.h"
#include "ui_repo_form.h"
#include "net/api.h"
#include <algorithm>
#include <QAction>
#include <QIcon>
#include "repo_new_dialog.h"
#include <QDebug>
#include <QMessageBox>
#include <QMenu>
#include <QAbstractButton>
#include "versions_form.h"
#include "settings_dialog.h"
#include "utils.h"

Q_DECLARE_METATYPE(vcs::api::repo)

namespace vcs::form {

    repos_form::repos_form(QWidget *parent) :
            QMainWindow(parent),
            vc_(new Ui::repos_view_controller),
            table_ctx_menu_(new QMenu(this)) {
        vc_->setupUi(this);
        ui_init();
        load_repos();
    }

    repos_form::~repos_form() {
        delete vc_;
    }

    void repos_form::ui_init() {
        utils::widget_center_screen(this);
        this->setWindowTitle("Repo list");
        ui_setup_ctx_menu();
        ui_setup_toolbar();
        ui_setup_table();
    }

    void repos_form::ui_setup_toolbar() {
        //
        QAction *act_new = new QAction(QIcon(":/default/images/add_32px.ico"), "New", this);
        QObject::connect(act_new, &QAction::triggered, this, &repos_form::new_repo);
        //
        QAction *act_reload = new QAction(QIcon(":/default/images/reload_32px.ico"), "Reload", this);
        QObject::connect(act_reload, &QAction::triggered, this, &repos_form::load_repos);
        //
        QAction *act_settings = new QAction(QIcon(":/default/images/settings_32px.ico"), "Settings", this);
        QObject::connect(act_settings, &QAction::triggered, this, &repos_form::open_settings);
        //
        vc_->tool_bar->addActions({act_new, act_reload, act_settings});
        //
        // disbale toolbar context menu
        vc_->tool_bar->setContextMenuPolicy(Qt::PreventContextMenu);
        // show text under ico
        vc_->tool_bar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);
    }

    void repos_form::ui_setup_table() {
        //
        QStringList ls = {"Name", "Desc", "CreateAt"};
        vc_->table_repos->setColumnCount(ls.size());
        vc_->table_repos->setHorizontalHeaderLabels(ls);
        //
        QHeaderView *hor_headers = vc_->table_repos->horizontalHeader();
        hor_headers->setSectionResizeMode(0, QHeaderView::ResizeMode::ResizeToContents);
        hor_headers->setSectionResizeMode(1, QHeaderView::ResizeMode::Stretch);
        hor_headers->setSectionResizeMode(2, QHeaderView::ResizeMode::ResizeToContents);
        vc_->table_repos->setSelectionBehavior(QTableView::SelectionBehavior::SelectRows);
        //
        vc_->table_repos->setShowGrid(true);
        vc_->table_repos->setEditTriggers(QTableWidget::NoEditTriggers);
        vc_->table_repos->setContextMenuPolicy(Qt::CustomContextMenu);
        //
        QObject::connect(vc_->table_repos, &QTableWidget::itemSelectionChanged,
                         this, &repos_form::item_selection_changed);
        //
        QObject::connect(vc_->table_repos,
                         &QTableWidget::customContextMenuRequested,
                         [this](const QPoint &table_pos) {
                             QPoint cur_pos = QCursor::pos();
                             int row_idx = vc_->table_repos->rowAt(table_pos.y());
                             if (row_idx != -1) {
                                 table_ctx_menu_->exec(cur_pos);
                             }
                         });
        QObject::connect(vc_->table_repos, &QTableWidget::itemDoubleClicked,
                         [this](QTableWidgetItem *item) {
                             QString repo_name = vc_->table_repos->item(item->row(), 0)->text();
#ifdef Debug
                             qDebug() << "row double-clicked:" << item->row() << "repo name=" << repo_name;
#endif
                             versions_form *form = new versions_form(repo_name, this);
                             form->setWindowModality(Qt::ApplicationModal);
                             form->setAttribute(Qt::WA_DeleteOnClose, true);
                             form->show();
                         });
    }

    void repos_form::ui_setup_ctx_menu() {
        QAction *act_del_repo = new QAction("Delete");
        QObject::connect(act_del_repo, &QAction::triggered, this, &repos_form::del_selected_repo);
        table_ctx_menu_->addAction(act_del_repo);
    }

    void repos_form::new_repo() {
        repo_new_dialog dig(this);
        QObject::connect(&dig, &repo_new_dialog::new_repo,
                         [this](const QString &name, const QString &desc) {
                             int err = api::repo_create(name, desc);
                             if (err) {
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

    void repos_form::load_repos() {
        QList<api::repo> repos{};
        int err = api::repo_list(&repos);
        if (err) {
            return;
        }
        vc_->table_repos->clearContents();
        vc_->table_repos->setRowCount(repos.size());
        for (int i = 0; i < repos.size(); ++i) {
            const api::repo &repo = repos[i];
            auto *repo_name_item = new QTableWidgetItem(repo.name);
            repo_name_item->setData(Qt::UserRole, QVariant::fromValue(repo));
            vc_->table_repos->setItem(i, 0, repo_name_item);
            vc_->table_repos->setItem(i, 1, new QTableWidgetItem(repo.desc));
            vc_->table_repos->setItem(i, 2, new QTableWidgetItem(repo.createAt.toString(api::kTimeFormat)));
        }
    }

    void repos_form::open_settings() {
        settings_dialog *dialog = new settings_dialog;
        dialog->setAttribute(Qt::WA_DeleteOnClose, true);
        dialog->exec();
    }

    void repos_form::del_selected_repo() {
        QModelIndexList rows = vc_->table_repos->selectionModel()->selectedRows();
        QStringList del_repos;
        std::transform(rows.cbegin(), rows.cend(), std::back_inserter(del_repos), [this](const QModelIndex &idx) {
            QTableWidgetItem *item = vc_->table_repos->item(idx.row(), 0);
            return item->text();
        });
        QMessageBox box(QMessageBox::Icon::Question,
                        "are u sure?",
                        QString("those repos will be deleted:\n%1").arg(del_repos.join(",\n")),
                        QMessageBox::Ok | QMessageBox::Cancel, this);

        QObject::connect(&box, &QMessageBox::buttonClicked, [&, this](QAbstractButton *button) {
            if (box.standardButton(button) != QMessageBox::Ok) {
                return;
            }
            // do delete
            std::for_each(del_repos.cbegin(), del_repos.cend(), [](const QString &repo_name) {
                api::repo_drop(repo_name);
            });
            this->load_repos();
        });
        box.exec();
    }

    void repos_form::item_selection_changed() {
        QModelIndexList selected_rows = vc_->table_repos->selectionModel()->selectedRows();
        if (selected_rows.size() != 1) {
            return;
        }
        int row = selected_rows.first().row();
        QVariant data = vc_->table_repos->item(row, 0)->data(Qt::UserRole);
        if (data.isNull()) {
            return;
        }
        const api::repo &r = data.value<api::repo>();
        vc_->status_bar->showMessage(r.desc);
    }

}
