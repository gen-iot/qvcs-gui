#include <QApplication>
#include "forms/repos_form.h"
#include "net/http.h"

using namespace vcs;

int main(int argc, char *argv[])
{
    http::curl_global global{};
    QApplication app(argc, argv);
    vcs::form::repos_form window;
    window.show();
    return app.exec();
}
