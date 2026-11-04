#include <QDebug>
#include <QTimer>

#include "SignupPage.h"

enum MessageType {
    Error,
    Success,
};

SignupPage::SignupPage(QWidget *parent)
	: QWidget(parent)
	, m_ui(new Ui::SignupPage())
{
	m_ui->setupUi(this);
    setupConnections();

    // add shortcut
    m_ui->signupButton->setShortcut(QKeySequence(Qt::Key_Return));
}

SignupPage::~SignupPage()
{
	delete m_ui;
}

void SignupPage::setupConnections() const
{
    connect(m_ui->loginButton, &QPushButton::clicked, this, &SignupPage::onLoginClicked);
    connect(m_ui->signupButton, &QPushButton::clicked, this, &SignupPage::onSignupClicked);
}

void SignupPage::onLoginClicked() 
{
    clearform();
	emit loginRequested();
}

void SignupPage::onSignupClicked() 
{
    const QString& username = m_ui->usernameLineEdit->text().trimmed();
    const QString& password = m_ui->passwordLineEdit->text();
    const QString& confirmPass = m_ui->confirmPasswordLineEdit->text();

    if (username.isEmpty() || password.isEmpty() || confirmPass.isEmpty()) {
        showMessage("Username and password are required", Error);
        return;
    }

    if (username.size() < 2) {
        showMessage("Username must be at least 2 characters.", Error);
        return;
    }

    if (password.size() < 6) {
        showMessage("Password must be at least 6 characters.", Error);
        return;
    }

    if (password != confirmPass) {
        showMessage("password not matched", Error);
        return;
    }

    // everything is right, we can create user
    UserRepository userRepo;
    bool isCreated = userRepo.createUser(username, password);

    if (!isCreated) {
        showMessage("error while creating user, try again", Error);
        return;
    }

    showMessage("User created successfully, Go to login page", Success);

    // clear input box
    clearform();
}

void SignupPage::clearform() const 
{
    m_ui->usernameLineEdit->clear();
    m_ui->passwordLineEdit->clear();
    m_ui->confirmPasswordLineEdit->clear();
}

void SignupPage::showMessage(const QString& message, MessageType type) 
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
