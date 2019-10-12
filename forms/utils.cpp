#include "utils.h"
#include <QWidget>
#include <QStyle>
#include <QApplication>
#include <QScreen>

namespace vcs::form::utils {

    void window_center_screen(QWidget *window) {
        window->setGeometry(
                QStyle::alignedRect(
                        Qt::LeftToRight,
                        Qt::AlignCenter,
                        window->size(),
                        QGuiApplication::screens().first()->availableGeometry()
                )
        );
    }

}