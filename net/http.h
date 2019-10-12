#ifndef UTILS_H
#define UTILS_H

#include <curl/curl.h>
#include <QString>
#include <QList>

#define nil nullptr

namespace vcs::http {

    constexpr static int kErrFileNotExist = 1099;

    using status_code_t = long;

    struct form_part {
        enum types {
            file,
            string
        };
        types type;
        QByteArray key; // allow empty
        QByteArray value;
    };

    struct curl_global final {
        curl_global() noexcept;

        ~curl_global() noexcept;
    };

    struct curl_raii final {

    public:
        ~curl_raii() noexcept;

        explicit curl_raii(int timeout_seconds = 5) noexcept;

        curl_raii(const curl_raii &) = delete;

        curl_raii &operator=(const curl_raii &) = delete;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"

        inline operator CURL *() noexcept {
            return handle_;
        }

#pragma clang diagnostic pop

        void set_mimes(const QList<form_part> &parts) noexcept;

        void add_header(const QByteArray &header) noexcept;

        void add_header(const QList<QByteArray> &header) noexcept;

        void set_timeout(int seconds) noexcept;

        int perform(const QByteArray &url, status_code_t *code = nullptr, QByteArray *input_body = nullptr) noexcept;

    private:
        CURL *handle_;
        curl_slist *headers_;
        curl_mime *mimes_;
    };


    int get(const QByteArray &url,
            status_code_t *status_code,
            QByteArray *input_body = nullptr,
            QList<QByteArray> const &headers = {});

    int del(const QByteArray &url,
            status_code_t *status_code,
            QByteArray *input_body = nullptr,
            QList<QByteArray> const &headers = {});

    int patch(const QByteArray &url,
              status_code_t *status_code,
              QByteArray *input_body = nullptr,
              QList<QByteArray> const &headers = {});

    int post(const QByteArray &url,
             status_code_t *status_code,
             QByteArray *output_body,
             QByteArray *input_body = nullptr,
             QList<QByteArray> const &headers = {});


    int post_form(const QByteArray &url,
                  status_code_t *status_code,
                  const QList<form_part> &mimes,
                  QByteArray *input_body = nullptr,
                  QList<QByteArray> const &headers = {}
    );

}


#endif // UTILS_H
