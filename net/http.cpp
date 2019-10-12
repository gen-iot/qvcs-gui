#include "http.h"
#include <cstdio>
#include <QDebug>

namespace vcs::http {

    size_t dump_response(const char *data, size_t size,
                         size_t n, void *user_data);

    curl_global::curl_global() noexcept {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    curl_global::~curl_global() noexcept {
        curl_global_cleanup();
    }

    curl_raii::curl_raii(int timeout_seconds) noexcept :
            handle_(curl_easy_init()),
            headers_(nullptr),
            mimes_(nullptr) {
#ifdef CURL_VERBOSE
        curl_easy_setopt(handle_, CURLOPT_VERBOSE, 1L);
#endif
        curl_easy_setopt(handle_, CURLOPT_ACCEPT_ENCODING, "");
        curl_easy_setopt(handle_, CURLOPT_NOPROGRESS, 1L);
        set_timeout(timeout_seconds);
    }

    curl_raii::~curl_raii() noexcept {
        curl_slist_free_all(headers_);
        curl_mime_free(mimes_);
        curl_easy_cleanup(handle_);
    }

    void curl_raii::add_header(const QByteArray &header) noexcept {
        this->headers_ = curl_slist_append(this->headers_, header.data());
        Q_ASSERT(this->headers_);
        curl_easy_setopt(this->handle_, CURLOPT_HTTPHEADER, this->headers_);
    }

    void curl_raii::add_header(const QList<QByteArray> &header) noexcept {
        std::for_each(header.cbegin(), header.cend(), [this](const QByteArray &it) {
            add_header(it);
        });
    }

    void curl_raii::set_timeout(int seconds) noexcept {
        curl_easy_setopt(handle_, CURLOPT_TIMEOUT, seconds);
    }

    int curl_raii::perform(const QByteArray &url, status_code_t *status_code, QByteArray *input_body) noexcept {
        curl_easy_setopt(handle_, CURLOPT_URL, url.data());
        curl_easy_setopt(handle_, CURLOPT_WRITEDATA, input_body);
        curl_easy_setopt(handle_, CURLOPT_WRITEFUNCTION, dump_response);
        CURLcode code = curl_easy_perform(this->handle_);
        if (code == CURLE_OK && status_code != nullptr) {
            curl_easy_getinfo(handle_, CURLINFO_RESPONSE_CODE, status_code);
        }
        return code;
    }

    void curl_raii::set_mimes(const QList<form_part> &parts) noexcept {
        if (mimes_) {
            curl_mime_free(mimes_);
            curl_easy_setopt(handle_, CURLOPT_MIMEPOST, nullptr);
            mimes_ = nullptr;
        }
        if (parts.empty()) {
            return;
        }
        mimes_ = curl_mime_init(handle_);
        std::for_each(parts.cbegin(), parts.cend(), [this](const form_part &it) {
            curl_mimepart *part = nullptr;
            switch (it.type) {
                case form_part::file: {
                    part = curl_mime_addpart(mimes_);
                    // file part
                    curl_mime_filedata(part, it.value.data());
                }
                    break;
                case form_part::string: {
                    part = curl_mime_addpart(mimes_);
                    curl_mime_data(part, it.value.data(), CURL_ZERO_TERMINATED);
                }
                    break;
            }
            if (part && !it.key.isEmpty()) {
                curl_mime_name(part, it.key.data());
            }
        });
        //
        curl_easy_setopt(handle_, CURLOPT_MIMEPOST, mimes_);
    }

    int get(const QByteArray &url,
            status_code_t *status_code,
            QByteArray *input_body,
            QList<QByteArray> const &headers) {
        curl_raii handle;
        handle.add_header(headers);
        return handle.perform(url, status_code, input_body);
    }

    int post(const QByteArray &url,
             status_code_t *status_code,
             QByteArray *output_body,
             QByteArray *input_body,
             QList<QByteArray> const &headers) {
        curl_raii handle;
        handle.add_header(headers);
        if (output_body) {
            curl_easy_setopt(handle, CURLOPT_POSTFIELDS, output_body->data());
        } else {
            curl_easy_setopt(handle, CURLOPT_POST, 1L);
            curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, 0L);
        }
        return handle.perform(url, status_code, input_body);
    }

    int del(const QByteArray &url,
            status_code_t *status_code,
            QByteArray *input_body,
            const QList<QByteArray> &headers) {
        curl_raii handle;
        handle.add_header(headers);
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "DELETE");
        return handle.perform(url, status_code, input_body);
    }

    int patch(const QByteArray &url,
              status_code_t *status_code,
              QByteArray *input_body,
              const QList<QByteArray> &headers) {
        curl_raii handle;
        handle.add_header(headers);
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "PATCH");
        return handle.perform(url, status_code, input_body);
    }

    int post_form(const QByteArray &url,
                  status_code_t *status_code,
                  const QList<form_part> &mimes,
                  QByteArray *input_body,
                  const QList<QByteArray> &headers) {
        curl_raii handle;
        handle.add_header(headers);
        handle.set_mimes(mimes);
        return handle.perform(url, status_code, input_body);
    }

    size_t dump_response(const char *data, size_t size,
                         size_t n, void *user_data) {
        size_t total_len = size * n;
        if (!user_data) {
            return total_len;
        }
        QByteArray *sb = static_cast<QByteArray *>(user_data);
        sb->append(data, int(total_len));
        return total_len;
    }
}


