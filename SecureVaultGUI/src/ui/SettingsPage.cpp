#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>

#include "SettingsPage.h"

SettingsPage::SettingsPage(QWidget *parent)
	: QWidget(parent)
	, m_ui(new Ui::SettingsPage())
{
	m_ui->setupUi(this);
	setupConnections();
}

SettingsPage::~SettingsPage()
{
	delete m_ui;
}

void SettingsPage::setupConnections() const
{
	connect(m_ui->logoutButton, &QPushButton::clicked, this, &SettingsPage::onLogoutClicked);
	connect(m_ui->deleteAccountButton, &QPushButton::clicked, this, &SettingsPage::onDeleteAccount);
	connect(m_ui->changeMPassButton, &QPushButton::clicked, this, &SettingsPage::onChangePasswordClicked);
}

void SettingsPage::onLogoutClicked() 
{
	emit logoutRequested();
}

void SettingsPage::onDeleteAccount()
{
	bool ok = false;
	const QString password = QInputDialog::getText(
		this,
		"Enter master Password",
		"Password:",
		QLineEdit::Password,
		QString(),
		&ok
	);

	if (!ok) {
		return;
	}

	if (password.isEmpty()) {
		QMessageBox::warning(this, "Error", "Password required");
		return;
	}

	auto user = UserSession::instance().currentUser();

	if (!user.has_value()) {
		return;
	}

	UserRepository userRepo;
	bool isDeleted = userRepo.deleteAccount(user->id(), password);

	if (!isDeleted) {
		QMessageBox::warning(this, "Error", "Invalid Passoword");
		return;
	}

	qDebug() << "Account deleted Successfully " << user->username();

	emit deleteAccount();

}

void SettingsPage::onChangePasswordClicked()
{
	ChangeMasterPasswordDialog dialog(this);

	connect(&dialog, &ChangeMasterPasswordDialog::changedPassword,
		this, &SettingsPage::onPasswordChange);

	dialog.exec();
}

void SettingsPage::onPasswordChange(const QString& oldPassword, const QString& newPassword, const QString& confirmPassword)
{
	qDebug() << "oldPass : " << oldPassword << " newPass : " << newPassword << "con pass: " << confirmPassword;
}