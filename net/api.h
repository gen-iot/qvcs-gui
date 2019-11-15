#ifndef NETWORK_H
#define NETWORK_H

#include<QString>
#include<QList>
#include<QDate>

namespace vcs::api {

    static constexpr char kTimeFormat[]{"yyyy-MM-dd HH:mm:ss"};
    static constexpr int kBadStatusCode{-5000};
    static constexpr int kJSONParseErr{-5001};

    void api_url_set(const QString &s) noexcept;

    QString api_url_get() noexcept;


    struct repo {
        QString name;
        QString desc;
        QDateTime createAt;
    };

    int repo_list(QList<repo> *repos);

    int repo_create(const QString &name, const QString &desc);

    int repo_drop(const QString &repo_name);

    int repo_update_desc(const QString &repo_desc);

    struct version {
        QString name;
        QString desc;
        QString url;
        QString md5;
        bool is_latest;
        QDateTime createAt;
    };

    int version_list(const QString &repo_name, QList<version> *versions);


    int version_delete(const QString &repo_name, const QString &ver_name);

    int version_head(const QString &repo_name, const QString &ver_name);

}


#endif // NETWORK_H
