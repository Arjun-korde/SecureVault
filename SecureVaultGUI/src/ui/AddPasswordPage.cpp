#include <QMessageBox>
#include <QTimer>

#include "AddPasswordPage.h"

enum MessageType {
    Error,
    Success,
};

AddPasswordPage::AddPasswordPage(QWidget *parent)
	: QWidget(parent)
	, m_ui(new Ui::AddPasswordPage())
{
	m_ui->setupUi(this);
    setupConnections();
}

AddPasswordPage::~AddPasswordPage()
{
	delete m_ui;
}

void AddPasswordPage::setupConnections() const
{
    connect(m_ui->saveButton, &QPushButton::clicked, this, &AddPasswordPage::onPasswordSaveClicked);
}

void AddPasswordPage::onPasswordSaveClicked() {

    const QString domain = m_ui->domainLineEdit->text().trimmed();
    const QString username = m_ui->usernameLineEdit->text().trimmed();
    const QString password = m_ui->passwordLineEdit->text();
    const bool isMasterPasswordRequired = m_ui->requireMasterPasswordCheck->isChecked();

    if (domain.isEmpty() || username.isEmpty() || password.isEmpty()) {
        qDebug() << "all fields are required";
        showMessage("All fields are required", Error);
        return;
    }

    // get the current user
    auto user = UserSession::instance().currentUser();
    if (!user.has_value()) {
        return;
    }
    const int userId = user->id();

    // here id parameter is dummy
    PasswordEntry entry(
        0,
        domain,
        username,
        password,
        isMasterPasswordRequired
    );

    PasswordRepository repo;
    bool success = repo.addPassword(userId, entry);

    if (!success) {
        qDebug() << "failed to save password for user:" << user->username();
        showMessage("failed to save password for user", Error);
        return;
    }

    qDebug() << "Password saved for user:" << user->username();
    showMessage("Password Saved Successfully", Success);
    clearform();
	emit passwordSaveSuccess();
}

void AddPasswordPage::showMessage(const QString& message, MessageType type)
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

void AddPasswordPage::clearform() const
{
    m_ui->domainLineEdit->clear();
    m_ui->usernameLineEdit->clear();
    m_ui->passwordLineEdit->clear();
}
