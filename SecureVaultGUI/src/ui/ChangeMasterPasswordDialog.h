#pragma once

#include <QDialog>
#include "ui_ChangeMasterPasswordDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ChangeMasterPasswordDialog; };
QT_END_NAMESPACE

class ChangeMasterPasswordDialog : public QDialog
{
	Q_OBJECT

public:
	ChangeMasterPasswordDialog(QWidget *parent = nullptr);
	~ChangeMasterPasswordDialog();

private:
	Ui::ChangeMasterPasswordDialog *m_ui;

	void setupConnections() const;

signals:
	void changedPassword(const QString& oldPassword, const QString& newPassword, const QString& confirmPassword);

private slots:
	void onCancelClicked();
	void onUpdateClicked();
};

