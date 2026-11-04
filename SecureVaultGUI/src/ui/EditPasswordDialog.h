#pragma once

#include <QDialog>
#include "ui_EditPasswordDialog.h"
#include "models/PasswordEntry.h"

QT_BEGIN_NAMESPACE
namespace Ui { class EditPasswordDialog; };
QT_END_NAMESPACE

class EditPasswordDialog : public QDialog
{
	Q_OBJECT

public:
	EditPasswordDialog(const PasswordEntry& entry, QWidget *parent = nullptr);
	~EditPasswordDialog();

private:
	Ui::EditPasswordDialog *m_ui;
	PasswordEntry m_entry;

	void fillInputs();
	void setupConnections() const;

signals:
	void passwordUpdate(const PasswordEntry& entry);

private slots:
	void onUpdateClicked();
	void onCancelClicked();
};

