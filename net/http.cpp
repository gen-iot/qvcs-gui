#include "http.h"

namespace vcs::http {

    curl_global::curl_global() noexcept {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    curl_global::~curl_global() noexcept {
        curl_global_cleanup();
    }

    curl_raii::curl_raii(int timeout_seconds) noexcept :
            handle_(curl_easy_init()),
            headers_(nullptr) {
        curl_easy_setopt(handle_, CURLOPT_ACCEPT_ENCODING, "");
        curl_easy_setopt(handle_, CURLOPT_NOPROGRESS, 1L);
        set_timeout(timeout_seconds);
    }

    curl_raii::~curl_raii() noexcept {
        curl_easy_cleanup(handle_);
        curl_slist_free_all(this->headers_);
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

    int curl_raii::perform(const QByteArray &url, status_code_t *status_code) noexcept {
        curl_easy_setopt(handle_, CURLOPT_URL, url.data());
        CURLcode code = curl_easy_perform(this->handle_);
        if (code == CURLE_OK && status_code != nullptr) {
            curl_easy_getinfo(this->handle_, CURLINFO_RESPONSE_CODE, status_code);
        }
        return code;
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

    int get(const QByteArray &url,
            status_code_t *status_code,
            QByteArray *input_body,
            QList<QByteArray> const &headers) {
        curl_raii handle;
        handle.add_header(headers);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, dump_response);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, input_body);
        return handle.perform(url, status_code);
    }

    int post(const QByteArray &url,
             status_code_t *status_code,
             QByteArray *output_body,
             QByteArray *input_body,
             QList<QByteArray> const &headers) {
        curl_raii handle;
        handle.add_header(headers);

        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, dump_response);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, input_body);

        if (output_body) {
            curl_easy_setopt(handle, CURLOPT_POSTFIELDS, output_body->data());
        } else {
            curl_easy_setopt(handle, CURLOPT_POST, 1L);
            curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, 0L);
        }
        return handle.perform(url, status_code);
    }

    int del(const QByteArray &url,
            status_code_t *status_code,
            QByteArray *input_body,
            const QList<QByteArray> &headers) {
        curl_raii handle;
        handle.add_header(headers);
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "DELETE");
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, dump_response);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, input_body);
        return handle.perform(url, status_code);
    }

}


