#ifndef UTILS_H
#define UTILS_H

#include <curl/curl.h>
#include <QString>
#include <QList>

#define nil nullptr

namespace vcs::http {

    constexpr static int kErrFileNotExist = 1099;

    using status_code_t = long;

    struct curl_global final {
        curl_global() noexcept;

        ~curl_global() noexcept;
    };

    struct curl_raii final {

    public:
        explicit curl_raii(int timeout_seconds = 5) noexcept;

        ~curl_raii() noexcept;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"

        inline operator CURL *() noexcept {
            return handle_;
        }

#pragma clang diagnostic pop

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


    struct form_item {
        enum types {
            file,
            string
        };
        types type;
        QByteArray key; // allow empty
        QByteArray value;
    };

    int post_form(const QByteArray &url,
                  status_code_t *status_code,
                  const QList<form_item> &mimes,
                  QByteArray *input_body = nullptr,
                  QList<QByteArray> const &headers = {}
    );

}


#endif // UTILS_H
