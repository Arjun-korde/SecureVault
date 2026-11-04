#include <QTimer>

#include "MasterPasswordDialog.h"
#include "database/UserRepository.h"

MasterPasswordDialog::MasterPasswordDialog(QWidget *parent)
	: QDialog(parent)
	, m_ui(new Ui::MasterPasswordDialog())
{
	m_ui->setupUi(this);
	setupConnections();
}

MasterPasswordDialog::~MasterPasswordDialog()
{
	delete m_ui;
}

void MasterPasswordDialog::setupConnections() 
{
	connect(m_ui->confirmButton, &QPushButton::clicked, this, &MasterPasswordDialog::onConfirmClicked);
	connect(m_ui->cancelButton, &QPushButton::clicked, this, &MasterPasswordDialog::onCancelClicked);

	connect(
		m_ui->togglePasswordVisibility,
		&QPushButton::clicked,
		this,
		&MasterPasswordDialog::onTogglePasswordVisibility
	);
}

void MasterPasswordDialog::onCancelClicked() 
{
	reject();
}

void MasterPasswordDialog::onConfirmClicked() 
{
	
	const QString masterPasswordInput = m_ui->masterPasswordLineEdit->text();

	if (masterPasswordInput.isEmpty()) {
		m_ui->messageLabel->setText("all fields are required");

		QTimer::singleShot(2000, this, [this]() {
			m_ui->messageLabel->clear();
			});
		return;
	}

	UserRepository userRepo;

	bool isValidPassword = userRepo.verifyPassword(masterPasswordInput);

	if (!isValidPassword) {
		m_ui->messageLabel->setText("Invalid Password");

		QTimer::singleShot(2000, this, [this]() {
			m_ui->messageLabel->clear();
			});
		return;
	}

	accept();
}

void MasterPasswordDialog::onTogglePasswordVisibility()
{
	m_isPasswordVisible = !m_isPasswordVisible;

	m_ui->masterPasswordLineEdit->setEchoMode(
		m_isPasswordVisible
		? QLineEdit::Normal
		: QLineEdit::Password
	);

	m_ui->togglePasswordVisibility->setText(
		m_isPasswordVisible ? "Hide" : "Show"
	);
}
