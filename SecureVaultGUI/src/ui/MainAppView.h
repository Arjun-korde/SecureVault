#pragma once

#include <QWidget>
#include "ui_MainAppView.h"
#include "DashboardPage.h"
#include "AddPasswordPage.h"
#include "SettingsPage.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainAppView; };
QT_END_NAMESPACE

class MainAppView : public QWidget
{
	Q_OBJECT

public:
	MainAppView(QWidget *parent = nullptr);
	~MainAppView();

private:
	Ui::MainAppView *m_ui;

	DashboardPage* m_dashboardPage;
	AddPasswordPage* m_addPasswordPage;
	SettingsPage* m_settingsPage;

	void setupPages();
	void setupConnections();

signals:
	void logoutRequested();

private slots:
	void showDashboard();
	void showAddPassword();
	void showSettings();
	void onLogoutClicked();
	void onPasswordSave();
	void onDeleteAccount();

public slots:
	void onUserLoggedIn();
};

