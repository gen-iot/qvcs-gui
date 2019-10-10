#ifndef UTILS_H
#define UTILS_H

#include <curl/curl.h>
#include <QString>
#include <QList>

#define nil nullptr

namespace vcs::http {

    using status_code_t = long;

    struct curl_global final {
        curl_global() noexcept;

        ~curl_global() noexcept;
    };

    struct curl_raii final {

    public:
        explicit curl_raii(int timeout_seconds = 5) noexcept;

        ~curl_raii() noexcept;

        inline operator CURL *() noexcept {
            return handle_;
        }

        void add_header(const QByteArray &header) noexcept;

        void add_header(const QList<QByteArray> &header) noexcept;

        void set_timeout(int seconds) noexcept;

        int perform(const QByteArray &url, status_code_t *code = nullptr) noexcept;

    private:
        CURL *handle_;
        curl_slist *headers_;
    };


    int get(const QByteArray &url,
            status_code_t *status_code,
            QByteArray *input_body = nullptr,
            QList<QByteArray> const &headers = {});

    int del(const QByteArray &url,
            status_code_t *status_code,
            QByteArray *input_body = nullptr,
            QList<QByteArray> const &headers = {});

    int post(const QByteArray &url,
             status_code_t *status_code,
             QByteArray *output_body,
             QByteArray *input_body = nullptr,
             QList<QByteArray> const &headers = {});

}


#endif // UTILS_H
