#pragma once

#include <QWidget>

#include "ui_LoginPage.h"
#include "database/UserRepository.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoginPage; };
QT_END_NAMESPACE

enum MessageType;

class LoginPage : public QWidget
{
	Q_OBJECT

public:
	LoginPage(QWidget *parent = nullptr);
	~LoginPage();

private:
	Ui::LoginPage *m_ui;

	void clearform() const;
	void setupConnections() const;
	void showMessage(const QString& message, MessageType type);

signals:
	void loginSuccessful(const User& user);
	void signupRequested();

private slots:
	void onLoginClicked();
	void onSignupClicked();
};

