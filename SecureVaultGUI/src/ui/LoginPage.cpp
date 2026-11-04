#include "LoginPage.h"

#include <QDebug>
#include <QKeySequence>
#include <QTimer>

enum MessageType {
    Error,
    Success,
};

LoginPage::LoginPage(QWidget *parent)
	: QWidget(parent)
	, m_ui(new Ui::LoginPage())
{
	m_ui->setupUi(this);
    setupConnections();

    // add shortcut
    m_ui->loginButton->setShortcut(QKeySequence(Qt::Key_Return));
}

LoginPage::~LoginPage()
{
	delete m_ui;
}

void LoginPage::setupConnections() const 
{
    connect(m_ui->loginButton, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
    connect(m_ui->createAccountButton, &QPushButton::clicked, this, &LoginPage::onSignupClicked);
}

void LoginPage::onLoginClicked() 
{
    const QString username = m_ui->usernameLineEdit->text().trimmed();
    const QString password = m_ui->passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        showMessage("Username and password are required", Error);
        return;
    }

    UserRepository userRepo;
    const auto userOpt = userRepo.login(username, password);

    if (!userOpt.has_value()) {
        showMessage("Invalid username or password", Error);
        return;
    }

    clearform();
    emit loginSuccessful(userOpt.value());

}

void LoginPage::onSignupClicked() 
{
    clearform();
    emit signupRequested();
}

void LoginPage::showMessage(const QString& message, MessageType type)
{
    if (type == MessageType::Error) {
        m_ui->messageLabel->setStyleSheet("color:red");
    }
    else if (type == MessageType::Success) {
        m_ui->messageLabel->setStyleSheet("color:green");
    }
    m_ui->messageLabel->setText(message);

    QTimer::singleShot(2000, this, [this]() {
        m_ui->messageLabel->clear();
        });
}

void LoginPage::clearform() const {
    m_ui->passwordLineEdit->clear();
    m_ui->usernameLineEdit->clear();
}