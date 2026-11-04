#pragma once

#include <QMainWindow>

#include "ui_mainwindow.h"
#include "ui/loginpage.h"
#include "ui/signuppage.h"
#include "ui/mainappview.h"
#include "app/UserSession.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* m_ui;

    LoginPage* m_loginPage;
    SignupPage* m_signupPage;
    MainAppView* m_mainAppView;

    void setupPages();
    void setupConnections();
};
