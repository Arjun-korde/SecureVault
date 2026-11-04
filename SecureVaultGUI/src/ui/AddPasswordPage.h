#pragma once

#include <QWidget>
#include "ui_AddPasswordPage.h"
#include "database/PasswordRepository.h"
#include "app/UserSession.h"

enum MessageType;

QT_BEGIN_NAMESPACE
namespace Ui { class AddPasswordPage; };
QT_END_NAMESPACE

class AddPasswordPage : public QWidget
{
	Q_OBJECT

public:
	AddPasswordPage(QWidget *parent = nullptr);
	~AddPasswordPage();

private:
	Ui::AddPasswordPage *m_ui;

	void setupConnections() const;
	void clearform() const;
	void showMessage(const QString& message, MessageType type);

signals:
	void passwordSaveSuccess();
	

private slots:
	void onPasswordSaveClicked();
	
};

