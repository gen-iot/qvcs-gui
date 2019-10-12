//
// Created by suzhen on 2019/10/10.
//

#include <curl/curl.h>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>
#include <QThread>
#include <QMessageBox>
#include <QProgressDialog>
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
            repo_name_(repo_name) {
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
        //
        vc_->table_versions->setShowGrid(true);
        vc_->table_versions->setEditTriggers(QTableWidget::NoEditTriggers);
        //vc_->table_versions->setContextMenuPolicy()
    }

    void versions_form::ui_setup_toolbar() {
        //
        QAction *upload_new = new QAction(QIcon(":/default/images/upload_32px.ico"), "Upload", this);
        QObject::connect(upload_new, &QAction::triggered, this, &versions_form::show_create_version);
        //
        QAction *act_reload = new QAction(QIcon(":/default/images/reload_32px.ico"), "Reload", this);
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
        utils::window_center_screen(this->window());
        setWindowTitle(QString("Versions [%1]").arg(repo_name_));
        ui_setup_toolbar();
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

}

