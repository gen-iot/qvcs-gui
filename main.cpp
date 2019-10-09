#include <QApplication>
#include "forms/main_window.h"
#include "net/http.h"

using namespace vcs;

int main(int argc, char *argv[])
{
    http::curl_global global{};
    QApplication app(argc, argv);
    vcs::form::main_window window;
    window.show();
    return app.exec();
}
