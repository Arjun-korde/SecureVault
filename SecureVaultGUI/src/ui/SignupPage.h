#pragma once

#include <QWidget>
#include "ui_SignupPage.h"
#include "database/UserRepository.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SignupPage; };
QT_END_NAMESPACE

enum MessageType;

class SignupPage : public QWidget
{
	Q_OBJECT

public:
	SignupPage(QWidget *parent = nullptr);
	~SignupPage();

private:
	Ui::SignupPage *m_ui;

	void clearform() const;
	void setupConnections() const;
	void showMessage(const QString& message, MessageType type);

signals:
	void loginRequested();

private slots: 
	void onLoginClicked();
	void onSignupClicked();

};

