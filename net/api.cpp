#include "api.h"
#include "http.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <algorithm>

namespace vcs::api {

#define errOrStatus(e, c, ce)\
    if(e){\
    return e;\
}\
    if(c!=ce){\
    return kBadStatusCode;\
}\



    int repo_list(QList<repo> *repos) {
        const QString url = QString("%1/%2").arg(kBaseUrl).arg(u8"repositories");
        http::status_code_t code{0};
        QByteArray output{};
        int err = http::get(url.toUtf8(), &code, &output);
        errOrStatus(err, code, 200);
        if (!repos) {
            return 0;
        }
        QJsonParseError parse_err{};
        const auto doc = QJsonDocument::fromJson(output, &parse_err);
        if (parse_err.error != QJsonParseError::NoError) {
            return kJSONParseErr;
        }
        const auto arr = doc.array();
        std::transform(arr.cbegin(), arr.cend(), std::back_inserter(*repos),
                       [](const QJsonValue &it) -> repo {
                           repo repo{};
                           QJsonObject obj = it.toObject();
                           repo.name = obj.value("name").toString();
                           repo.desc = obj.value("description").toString();
                           int time_stamp = obj.value("createdAt").toInt();
                           repo.createAt = QDateTime::fromTime_t(uint(time_stamp));
                           return repo;
                       });
        return 0;
    }

    int version_list(const QString &repo_name, QList<version> *versions) {
        const QString url = QString("%1/versions/%2").arg(kBaseUrl).arg(repo_name);
        http::status_code_t code{0};
        QByteArray body{};
        int err = http::get(url.toUtf8(), &code, &body);
        errOrStatus(err, code, 200);
        QJsonParseError parse_err{};
        QJsonDocument doc = QJsonDocument::fromJson(body, &parse_err);
        if (parse_err.error != QJsonParseError::NoError) {
            return kJSONParseErr;
        }
        //
        QJsonArray arr = doc.array();
        std::transform(arr.cbegin(), arr.cend(), std::back_inserter(*versions), [](const QJsonValue &arr_it) {
            version it{};
            QJsonObject jsob = arr_it.toObject();
            it.version = jsob.value("version").toString();
            it.desc = jsob.value("desc").toString();
            it.url = jsob.value("url").toString();
            it.md5 = jsob.value("md5").toString();
            it.is_latest = jsob.value("is_latest").toBool();
            int time_stamp = jsob.value("createdAt").toInt();
            it.createAt = QDateTime::fromTime_t(uint(time_stamp));
            return it;
        });
        return 0;
    }

    int repo_create(const QString &name, const QString &desc) {
        QJsonObject jsob;
        jsob.insert("name", QJsonValue(name));
        jsob.insert("description", QJsonValue(desc));
        QJsonDocument doc(jsob);
        QByteArray req_body = doc.toJson();
        const QString url = QString("%1/repositories").arg(kBaseUrl);
        http::status_code_t code{0};
        QByteArray result;
        int err = http::post(url.toUtf8(),
                             &code,
                             &req_body,
                             &result,
                             {"Content-Type: application/json; charset=utf-8"});
        errOrStatus(err, code, 201);
        return 0;
    }

    int repo_drop(const QString &repo_name) {
        const QString url = QString("%1/%2/%3")
                .arg(kBaseUrl)
                .arg("repositories")
                .arg(repo_name);

        http::status_code_t code{0};
        int err = http::del(url.toUtf8(), &code);
        errOrStatus(err, code, 200);
        return 0;
    }

    int repo_update_desc(const QString &repo_desc) {
        return 0;
    }

    int version_upload(const QString &repo_name,
                       const QString &file_path,
                       const QString &version,
                       const QString &desc) {
        return 0;
    }
}
