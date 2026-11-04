#include <QDebug>
#include <QByteArray>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_loginPage(new LoginPage(this)),
    m_signupPage(new SignupPage(this)),
    m_mainAppView(new MainAppView(this))
{
    m_ui->setupUi(this);
    setWindowTitle("SecureVault");

    setupPages();
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::setupPages()
{
    m_ui->stackedWidget->addWidget(m_loginPage);
    m_ui->stackedWidget->addWidget(m_signupPage);
    m_ui->stackedWidget->addWidget(m_mainAppView);

    m_ui->stackedWidget->setCurrentWidget(m_loginPage);
}

void MainWindow::setupConnections()
{
    connect(m_loginPage, &LoginPage::loginSuccessful,
        this, [this](const User& user) -> void {
            UserSession::instance().setCurrentUser(user);
            m_ui->stackedWidget->setCurrentWidget(m_mainAppView);
        });

    // load password only after user successfully logged in
    connect(m_loginPage, &LoginPage::loginSuccessful, m_mainAppView, &MainAppView::onUserLoggedIn);

   connect(m_loginPage, &LoginPage::signupRequested,
        this, [this]() {
            m_ui->stackedWidget->setCurrentWidget(m_signupPage);
        });

    connect(m_signupPage, &SignupPage::loginRequested,
        this, [this]() {
            m_ui->stackedWidget->setCurrentWidget(m_loginPage);
        });

    connect(m_mainAppView, &MainAppView::logoutRequested,
        this, [this]() {
            m_ui->stackedWidget->setCurrentWidget(m_loginPage);
        });
}
