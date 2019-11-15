#include <QApplication>
#include "forms/repos_form.h"
#include "net/http.h"
#include "net/api.h"

using namespace vcs;

int main(int argc, char *argv[]) {
    qRegisterMetaType<http::status_code_t>("http::status_code_t");
    http::curl_global global{};
    QApplication::setOrganizationName("GEN-IOT");
    QApplication::setOrganizationDomain("gen-iot.com");
    QApplication app(argc, argv);
    vcs::form::repos_form window;
    window.show();
    return app.exec();
}
