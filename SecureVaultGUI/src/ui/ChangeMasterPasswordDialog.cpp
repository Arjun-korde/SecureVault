#include <QTimer>

#include "ChangeMasterPasswordDialog.h"

ChangeMasterPasswordDialog::ChangeMasterPasswordDialog(QWidget *parent)
	: QDialog(parent)
	, m_ui(new Ui::ChangeMasterPasswordDialog())
{
	m_ui->setupUi(this);
	setupConnections();
}

void ChangeMasterPasswordDialog::setupConnections() const
{
	connect(m_ui->updateButton, &QPushButton::clicked, this, &ChangeMasterPasswordDialog::onUpdateClicked);
	connect(m_ui->cancelButton, &QPushButton::clicked, this, &ChangeMasterPasswordDialog::onCancelClicked);
}

ChangeMasterPasswordDialog::~ChangeMasterPasswordDialog()
{
	delete m_ui;
}

void ChangeMasterPasswordDialog::onCancelClicked() {
	reject();
}

void ChangeMasterPasswordDialog::onUpdateClicked() {

	const QString oldpass = m_ui->oldPasswordLineEdit->text();
	const QString newPass = m_ui->newPasswordLineEdit->text();
	const QString confirmPass = m_ui->confirmPasswordLineEdit->text();

	if (oldpass.isEmpty() || newPass.isEmpty() || confirmPass.isEmpty()) {
		qDebug() << "all fields are required";
		m_ui->messageLabel->setText("all fields are required");

		QTimer::singleShot(2000, this, [this]() {
			m_ui->messageLabel->clear();
			});
		return;
	}

	emit changedPassword(oldpass,newPass, confirmPass);
	accept();
}