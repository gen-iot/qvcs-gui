//
// Created by suzhen on 2019/10/12.
//

#include "uploader.h"


namespace vcs::net {

    int progress_callback(void *user_data,
                          curl_off_t,
                          curl_off_t,
                          curl_off_t ultotal,
                          curl_off_t ulnow) {
        uploader *uploader = static_cast<class uploader *>(user_data);
        Q_ASSERT(uploader);
        uploader->on_progress(ultotal, ulnow);
        return 0;
    }

    void uploader::start() noexcept {
        // enable progress
        curl_easy_setopt(handle_, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(handle_, CURLOPT_XFERINFOFUNCTION, progress_callback);
        curl_easy_setopt(handle_, CURLOPT_XFERINFODATA, this);
        http::status_code_t status_code{0};
        int err = handle_.perform(url_, &status_code);
        QByteArray input_body{};
        on_result(err, status_code, input_body);
        finished();
    }

    uploader::uploader(QObject *parent,
                       const QByteArray &url,
                       const QList<http::form_part> &mimes,
                       const QList<QByteArray> &headers) noexcept:
            QObject(parent),
            handle_(),
            url_(url) {
        handle_.add_header(headers);
        handle_.set_mimes(mimes);
    }

}
