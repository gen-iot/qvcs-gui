#include "utils.h"
#include <QWidget>
#include <QStyle>
#include <QApplication>
#include <QScreen>
#include <QDialog>

namespace vcs::form::utils {

    void widget_center_screen(QWidget *window) {
        window->setGeometry(
                QStyle::alignedRect(
                        Qt::LeftToRight,
                        Qt::AlignCenter,
                        window->size(),
                        QGuiApplication::screens().first()->availableGeometry()
                )
        );
    }

    void dialog_disable_help(QDialog *dialog) {
        dialog->setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    }

}