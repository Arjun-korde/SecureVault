#pragma once

#include <QWidget>
#include "ui_SettingsPage.h"
#include "app/UserSession.h"
#include "database/UserRepository.h"
#include "ChangeMasterPasswordDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SettingsPage; };
QT_END_NAMESPACE

class SettingsPage : public QWidget
{
	Q_OBJECT

public:
	SettingsPage(QWidget *parent = nullptr);
	~SettingsPage();

	void setupConnections() const;

signals:
	void logoutRequested();
	void deleteAccount();

private:
	Ui::SettingsPage *m_ui;

private slots:
	void onLogoutClicked();
	void onDeleteAccount();
	void onChangePasswordClicked();
	void onPasswordChange(const QString& oldPassword, const QString& newPassword, const QString& confirmPassword);
};

