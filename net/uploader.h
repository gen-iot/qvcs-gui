//
// Created by suzhen on 2019/10/12.
//

#pragma once

#include <QObject>
#include "http.h"

namespace vcs::net {

    class uploader final : public QObject {

    Q_OBJECT

    public:
        ~uploader() noexcept override = default;

        uploader(QObject *parent,
                 const QByteArray &url,
                 const QList<http::form_part> &mimes,
                 const QList<QByteArray> &headers = {}) noexcept;

        void start() noexcept;

    Q_SIGNALS:

        void on_progress(long total, long cur);

        void on_result(int err, http::status_code_t status_code, const QByteArray &server_response);

        void finished();

    private:
        http::curl_raii handle_;
        QByteArray url_;
    };

}



