#include "api.h"
#include "http.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <algorithm>
#include <mutex>
#include <QSettings>

namespace vcs::api {

#define errOrStatus(e, c, ce)\
    if(e){\
    return e;\
}\
    if(c!=ce){\
    return kBadStatusCode;\
}

    void api_url_set(const QString &s) noexcept {
        QSettings settings{};
        settings.setValue("network/api_url", s);
    }

    QString api_url_get() noexcept {
        QSettings settings{};
        QString ret = settings.value("network/api_url").toString();
        if (ret.isEmpty()) {
            ret = "http://localhost";
        }
        return ret;
    }


    int repo_list(QList<repo> *repos) {
        const QString url = QString("%1/%2").arg(api_url_get()).arg(u8"repositories");
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
        QJsonArray arr = doc.array();
        std::transform(arr.constBegin(), arr.constEnd(), std::back_inserter(*repos),
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
        const QString url = QString("%1/versions/%2").arg(api_url_get()).arg(repo_name);
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
        std::transform(arr.constBegin(), arr.constEnd(), std::back_inserter(*versions),
                       [](const QJsonValue &arr_it) {
                           version it{};
                           QJsonObject jsob = arr_it.toObject();
                           it.name = jsob.value("version").toString();
                           it.desc = jsob.value("description").toString();
                           it.url = jsob.value("url").toString();
                           it.md5 = jsob.value("md5").toString();
                           it.is_latest = jsob.value("isLatest").toBool();
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
        const QString url = QString("%1/repositories").arg(api_url_get());
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
        const QString url = QString("%1/repositories/%2")
                .arg(api_url_get())
                .arg(repo_name);
        http::status_code_t code{0};
        int err = http::del(url.toUtf8(), &code);
        errOrStatus(err, code, 200);
        return 0;
    }

    int repo_update_desc(const QString &repo_desc) {
        return 0;
    }

    int version_delete(const QString &repo_name, const QString &ver_name) {
        const QString url = QString("%1/versions/%2/%3")
                .arg(api_url_get())
                .arg(repo_name)
                .arg(ver_name);
        http::status_code_t code{0};
        int err = http::del(url.toUtf8(), &code);
        errOrStatus(err, code, 200);
        return 0;
    }

    int version_head(const QString &repo_name, const QString &ver_name) {
        const QString url = QString("%1/versions/HEAD/%2/%3")
                .arg(api_url_get())
                .arg(repo_name)
                .arg(ver_name);
        http::status_code_t code{0};
        int err = http::patch(url.toUtf8(), &code, "{}");
        errOrStatus(err, code, 200);
        return 0;
    }


}
