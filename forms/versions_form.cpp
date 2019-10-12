//
// Created by suzhen on 2019/10/10.
//

#include <curl/curl.h>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>
#include <QThread>
#include <QMessageBox>
#include <QAction>
#include "versions_form.h"
#include "ui_versions_form.h"
#include "net/api.h"
#include "utils.h"
#include "versions_new_dialog.h"
#include "net/uploader.h"
#include "lite_progress_dialog.h"

namespace vcs::form {

    versions_form::versions_form(const QString &repo_name, QWidget *parent) :
            QMainWindow(parent),
            vc_(new Ui::versions_view_controller),
            repo_name_(repo_name),
            table_ctx_menu_(new QMenu(this)) {
        vc_->setupUi(this);
        //
        setWindowModality(Qt::ApplicationModal);
        setAttribute(Qt::WA_DeleteOnClose, true);
        //
        ui_init();
    }

    versions_form::~versions_form() {
        delete vc_;
    }

    void versions_form::load_versions() {
        QList<api::version> vers{};
        int err = api::version_list(this->repo_name_, &vers);
        if (err) {
#ifdef Debug
            qDebug() << "list version failed, err=" << err;
#endif
            return;
        }
        vc_->table_versions->clearContents();
        vc_->table_versions->setRowCount(vers.size());
        for (int i = 0; i < vers.size(); ++i) {
            const api::version &it = vers[i];
            vc_->table_versions->setItem(i, 0, new QTableWidgetItem(it.version));
            vc_->table_versions->setItem(i, 1, new QTableWidgetItem(it.desc));
            vc_->table_versions->setItem(i, 2, new QTableWidgetItem(it.is_latest ? "HEAD" : ""));
            vc_->table_versions->setItem(i, 3, new QTableWidgetItem(it.createAt.toString(api::kTimeFormat)));
        }
    }

    void versions_form::ui_setup_table() {
        //
        QStringList ls = {"Version", "Desc", "HEAD", "CreateAt"};
        vc_->table_versions->setColumnCount(ls.size());
        vc_->table_versions->setHorizontalHeaderLabels(ls);
        //
        QHeaderView *hor_headers = vc_->table_versions->horizontalHeader();
        hor_headers->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        hor_headers->setSectionResizeMode(1, QHeaderView::Stretch);
        hor_headers->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        hor_headers->setSectionResizeMode(3, QHeaderView::ResizeToContents);
        vc_->table_versions->setSelectionBehavior(QTableView::SelectionBehavior::SelectRows);
        //
        vc_->table_versions->setShowGrid(true);
        vc_->table_versions->setEditTriggers(QTableWidget::NoEditTriggers);
        vc_->table_versions->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

        QObject::connect(vc_->table_versions, &QTableWidget::customContextMenuRequested,
                         [this](const QPoint &table_pos) {
                             QPoint cur_pos = QCursor::pos();
                             int row_idx = vc_->table_versions->rowAt(table_pos.y());
                             if (row_idx != -1) {
                                 table_ctx_menu_->exec(cur_pos);
                             }
                         });
    }

    void versions_form::ui_setup_toolbar() {
        //
        QAction *upload_new = new QAction(
                QIcon(":/default/images/upload_32px.ico"), "Upload", this);
        QObject::connect(upload_new, &QAction::triggered, this, &versions_form::show_create_version);
        //
        QAction *act_reload = new QAction(
                QIcon(":/default/images/reload_32px.ico"), "Reload", this);
        QObject::connect(act_reload, &QAction::triggered, this, &versions_form::load_versions);
        //
        vc_->tool_bar->addActions({upload_new, act_reload});
        //
        // disable toolbar context menu
        vc_->tool_bar->setContextMenuPolicy(Qt::PreventContextMenu);
        // show text under ico
        vc_->tool_bar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);
    }

    void versions_form::ui_init() {
        utils::window_center_screen(this);
        setWindowTitle(QString("Versions [%1]").arg(repo_name_));
        ui_setup_toolbar();
        ui_setup_ctx_menu();
        ui_setup_table();
        load_versions();
    }

    void versions_form::show_create_version() {
        versions_new_dialog dialog(this);
        QObject::connect(&dialog, &versions_new_dialog::new_version_ready, this, &versions_form::version_created);
        dialog.exec();
    }


    void versions_form::version_created(const QString &ver_code,
                                        const QString &ver_file_loc,
                                        const QString &ver_desc,
                                        bool isHead) {
        QString upload_url = QString("%1/versions/%2").arg(api::kBaseUrl).arg(repo_name_);
        QList<http::form_part> parts{
                {
                        http::form_part::string,
                        "version",
                        ver_code.toUtf8()
                },
                {
                        http::form_part::string,
                        "description",
                        ver_desc.toUtf8()
                },
                {
                        http::form_part::file,
                        "file",
                        ver_file_loc.toUtf8()
                },
                {
                        http::form_part::string,
                        "isLatest",
                        isHead ? "true" : "false"
                }
        };
        QThread *thread = new QThread;
        net::uploader *uploader = new net::uploader(nil, upload_url.toUtf8(), parts);
        LiteQProgressDialog progressDialog(this);
        uploader->moveToThread(thread);
        //
        QObject::connect(thread, &QThread::started, uploader, &net::uploader::start);
        QObject::connect(uploader, &net::uploader::finished, thread, &QThread::quit);
        QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        QObject::connect(thread, &QThread::finished, uploader, &net::uploader::deleteLater);
        QObject::connect(thread, &QThread::finished, &progressDialog, &LiteQProgressDialog::close);
        //
        progressDialog.setWindowTitle("Uploading...");
        progressDialog.setCancelButton(nil);
        QObject::connect(uploader, &net::uploader::on_progress,
                         &progressDialog, &LiteQProgressDialog::on_progress);
        QObject::connect(uploader, &net::uploader::on_result,
                         this, &versions_form::version_create_result);
        thread->start();
        progressDialog.exec();
    }

    void versions_form::version_create_result(
            int err,
            http::status_code_t status_code,
            const QByteArray &server_response) {
        if (err) {
            QMessageBox::critical(this, "Error",
                                  QString("Send Request Failed:%1")
                                          .arg(curl_easy_strerror(CURLcode(err))));
            return;
        }
        if (status_code != 201) {
            QMessageBox::critical(this, "Error",
                                  QString("Version Create Failed, Bad Status Code:%1")
                                          .arg(status_code));
            return;
        }
#ifdef Debug
        qDebug() << "upload on result"
                 << "err" << curl_easy_strerror(CURLcode(err))
                 << "status_code" << status_code
                 << "response body:" << QString::fromUtf8(server_response);
#endif
        load_versions();
    }

    void versions_form::ui_setup_ctx_menu() {
        QAction *act_ver_del = new QAction("Delete");
        QObject::connect(act_ver_del, &QAction::triggered, this, &versions_form::delete_selected_versions);
        QAction *act_ver_set_head = new QAction("HEAD");
        QObject::connect(act_ver_set_head, &QAction::triggered, this, &versions_form::set_selected_head);
        table_ctx_menu_->addActions({act_ver_set_head, act_ver_del});
    }

    void versions_form::delete_selected_versions() {
        QModelIndexList selected_rows = vc_->table_versions->selectionModel()->selectedRows();
        QStringList ver_will_delete{};
        for (const QModelIndex &it : selected_rows) {
            int row_idx = it.row();
            QString ver_name = vc_->table_versions->item(row_idx, 0)->text();
            if (!vc_->table_versions->item(row_idx, 2)->text().isEmpty()) {
                QMessageBox::critical(this, "Error",
                                      QString("abort: %1 is HEAD").arg(ver_name));
                return;
            }
            ver_will_delete.append(ver_name);
        }
        QMessageBox::StandardButton chose = QMessageBox::question(this, "Question",
                                                                  QString("are u sure delete such versions?\n%1")
                                                                          .arg(ver_will_delete.join('\n')));
        if (chose != QMessageBox::StandardButton::Yes) {
#ifdef Debug
            qDebug() << "user cancel delete versions:" << ver_will_delete.join(',');
#endif
            return;
        }
        QStringList del_failed_ls{};
        for (const QString &ver_name : ver_will_delete) {
            if (api::version_delete(repo_name_, ver_name)) {
                del_failed_ls.append(ver_name);
            }
        }
        if (del_failed_ls.empty()) {
            QMessageBox::information(this, "Information", "Delete Success");
        } else {
            QMessageBox::critical(this, "Error",
                                  del_failed_ls.size() == ver_will_delete.size()
                                  ? "all deletion failed!"
                                  : QString("some version delete failed:\n%1").arg(del_failed_ls.join('\n')));
        }
        if (del_failed_ls.size() != ver_will_delete.size()) {
            load_versions();
        }
    }

    void versions_form::set_selected_head() {
        QModelIndexList selected_rows = vc_->table_versions->selectionModel()->selectedRows();
        if (selected_rows.size() != 1) {
            QMessageBox::critical(this, "Error", "too many version selected");
            return;
        }
        int row_idx = selected_rows.first().row();
        QString selected_ver_name = vc_->table_versions->item(row_idx, 0)->text();
        QMessageBox::StandardButton chose = QMessageBox::question(this, "Question",
                                                                  QString("set [%1] as HEAD?")
                                                                          .arg(selected_ver_name));
        if (chose != QMessageBox::StandardButton::Yes) {
#ifdef Debug
            qDebug() << "user cancel set " << selected_ver_name << " as HEAD versions";
#endif
            return;
        }
        int err = api::version_head(repo_name_, selected_ver_name);
        if (err) {
            QMessageBox::critical(this, "Error",
                                  QString("set [%1] as HEAD failed").arg(selected_ver_name));
            return;
        }
        QMessageBox::information(this, "Information",
                                 QString("set [%1] as HEAD success").arg(selected_ver_name));
        load_versions();
    }

}

