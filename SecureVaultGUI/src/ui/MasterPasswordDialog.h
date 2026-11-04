#pragma once

#include <QDialog>
#include "ui_MasterPasswordDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MasterPasswordDialog; };
QT_END_NAMESPACE

class MasterPasswordDialog : public QDialog
{
	Q_OBJECT

public:
	MasterPasswordDialog(QWidget *parent = nullptr);
	~MasterPasswordDialog();

private:
	Ui::MasterPasswordDialog *m_ui;
	bool m_isPasswordVisible = false;

	void setupConnections();

private slots: 
	void onConfirmClicked();
	void onCancelClicked();
	void onTogglePasswordVisibility();

};
