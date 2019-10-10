#ifndef NETWORK_H
#define NETWORK_H

#include<QString>
#include<QList>
#include<QDate>

namespace vcs {

    namespace api {

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
            QString version;
            QString desc;
            QString url;
            QString md5;
            QDateTime createAt;
        };

        int version_list(const QString &repo_name, QList<version> *versions);

        int version_upload(const QString &repo_name,
                           const QString &file_path,
                           const QString &version,
                           const QString &desc = "no desc");

    }

}


#endif // NETWORK_H
