#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class main_view_controller;
}

namespace vcs::form {
class main_window : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit main_window(QWidget *parent = nullptr);
    ~main_window();
    
private:
    void init_ui();
    void ui_setup_toolbar();
    void ui_setup_table();
    void ui_setup_context_menu();
    //
    void new_repo();    
    void load_repos();
    void del_selected_repo();
   
    
private:
    Ui::main_view_controller *ui_;
    QMenu* table_menu_;
    
};
}



#endif // MAINWINDOW_H
