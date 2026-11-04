#include <QTimer>

#include "EditPasswordDialog.h"

EditPasswordDialog::EditPasswordDialog(const PasswordEntry& entry, QWidget *parent)
	: QDialog(parent), 
	m_ui(new Ui::EditPasswordDialog()),
	m_entry(entry)
{
	m_ui->setupUi(this);
	setWindowTitle("Edit Password Dialog");
	setupConnections();

	fillInputs();
}

EditPasswordDialog::~EditPasswordDialog()
{
	delete m_ui;
}

void EditPasswordDialog::setupConnections() const
{
	connect(m_ui->updateButton, &QPushButton::clicked, this, &EditPasswordDialog::onUpdateClicked);
	connect(m_ui->cancelButton, &QPushButton::clicked, this, &EditPasswordDialog::onCancelClicked);
}

void EditPasswordDialog::onCancelClicked() 
{
	reject();
}

void EditPasswordDialog::fillInputs() 
{

	m_ui->domainLineEdit->setText(m_entry.domain());
	m_ui->usernameLineEdit->setText(m_entry.username());
	
}

void EditPasswordDialog::onUpdateClicked() 
{

	const QString domain = m_ui->domainLineEdit->text().trimmed();
	const QString username = m_ui->usernameLineEdit->text().trimmed();
	const QString password = m_ui->passwordLineEdit->text();

	if (domain.isEmpty() || username.isEmpty() || password.isEmpty()) {
		qDebug() << "all fields are required";
		m_ui->messageLabel->setText("all fields are required");

		QTimer::singleShot(2000, this, [this]() {
			m_ui->messageLabel->clear();
			});
		return;
	}

	m_entry.setDomain(m_ui->domainLineEdit->text().trimmed());
	m_entry.setUsername(m_ui->usernameLineEdit->text().trimmed());
	m_entry.setPassword(m_ui->passwordLineEdit->text());

	emit passwordUpdate(m_entry);
	accept();
}