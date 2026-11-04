#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QGuiApplication>
#include <QClipboard>
#include <QTimer>
#include <QMessageBox>
#include <QPointer>

#include "DashboardPage.h"

DashboardPage::DashboardPage(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::DashboardPage())
    , m_searchDebounceTimer(new QTimer(this))
{
    m_ui->setupUi(this);

    m_searchDebounceTimer->setSingleShot(true);
    m_searchDebounceTimer->setInterval(200);

    setupPasswordTable();
    setupConnections();

}

DashboardPage::~DashboardPage()
{
    delete m_ui;
}

void DashboardPage::setupConnections() const
{
    connect(this, &DashboardPage::passwordDeleteSuccess, this, &DashboardPage::loadPasswords);
    connect(m_ui->refreshPasswordListButton, &QPushButton::clicked, this, &DashboardPage::onRefreshPasswordsClicked);
    connect(m_ui->searchLineEdit, &QLineEdit::textChanged, this, &DashboardPage::onSearchTextChanged
    );
    connect(m_searchDebounceTimer, &QTimer::timeout, this,
        &DashboardPage::performSearch);
    connect(m_ui->addPasswordButton, &QPushButton::clicked, this, [this]() {
        emit addPasswordClicked();
        });
    connect(m_ui->addPasswordButton_2, &QPushButton::clicked, this, [this]() {
        emit addPasswordClicked();
        });
}

void DashboardPage::setupPasswordTable()
{
    m_ui->passwordTable->setColumnCount(3);
    m_ui->passwordTable->setHorizontalHeaderLabels(
        {"Domain", "Username", "Actions" });

    QHeaderView* header = m_ui->passwordTable->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Stretch); // Domain
    header->setSectionResizeMode(1, QHeaderView::Stretch); // Username
    header->setSectionResizeMode(2, QHeaderView::Fixed);   // Actions

    m_ui->passwordTable->setColumnWidth(2, 350);

    // align header to the left
    m_ui->passwordTable->horizontalHeader()->setDefaultAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
    );
}

void DashboardPage::addPasswordRow(const PasswordEntry& item)
{
    // create new row
    const int row = m_ui->passwordTable->rowCount();
    m_ui->passwordTable->insertRow(row);

    // fill data in row
    m_ui->passwordTable->setItem(row, 0, new QTableWidgetItem(item.domain()));
    m_ui->passwordTable->setItem(row, 1, new QTableWidgetItem(item.username()));

    // create action button layout
    QWidget* actionWidget = new QWidget(m_ui->passwordTable);
    QHBoxLayout* layout = new QHBoxLayout(actionWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    QPushButton* editBtn = new QPushButton("Edit", actionWidget);
    QPushButton* deleteBtn = new QPushButton("Delete", actionWidget);
    QPushButton* copyUserBtn = new QPushButton("Copy User", actionWidget);
    QPushButton* copyPassBtn = new QPushButton("Copy Pass", actionWidget);

    copyUserBtn->setObjectName("copyUserBtn");
    copyPassBtn->setObjectName("copyPassBtn");
    editBtn->setObjectName("editBtn");
    deleteBtn->setObjectName("deleteBtn");

    copyUserBtn->setFixedWidth(85);
    copyPassBtn->setFixedWidth(85);

    // create (Button, passwordId) Mappings
    for (auto btn : { editBtn, deleteBtn, copyUserBtn, copyPassBtn }) {
        btn->setCursor(Qt::PointingHandCursor);
        m_buttonToPasswordId.insert(btn, item.id());
    }

    connect(editBtn, &QPushButton::clicked, this, &DashboardPage::onEditPassword);
    connect(deleteBtn, &QPushButton::clicked, this, &DashboardPage::onDeletePassword);
    connect(copyUserBtn, &QPushButton::clicked, this, &DashboardPage::onCopyUsername);
    connect(copyPassBtn, &QPushButton::clicked, this, &DashboardPage::onCopyPassword);

    // add buttons to the layout
    layout->addWidget(editBtn);
    layout->addWidget(deleteBtn);
    layout->addWidget(copyUserBtn);
    layout->addWidget(copyPassBtn);

    actionWidget->setLayout(layout);
    m_ui->passwordTable->setCellWidget(row, 2, actionWidget);
}

void DashboardPage::loadPasswords()
{
    auto user = UserSession::instance().currentUser();

    if (!user.has_value()) {
        return;
    }

    int userId = user->id();
    const auto passwordList = m_passwordRepo.fetchPasswords(userId);

    if (passwordList.isEmpty()) {
        setViewState(ViewState::Empty);
        return;
    }

    renderPasswords(passwordList);
    setViewState(ViewState::Passwords);
}

void DashboardPage::renderPasswords(const QVector<PasswordEntry>& entries)
{
    m_buttonToPasswordId.clear();
    m_ui->passwordTable->setRowCount(0);

    for (const auto& entry : entries) {
        addPasswordRow(entry);
    }
}

void DashboardPage::performSearch()
{
    auto user = UserSession::instance().currentUser();

    if (!user.has_value()) {
        return;
    }

    int userId = user->id();
    const QString searchText = m_ui->searchLineEdit->text().trimmed();

    if (searchText.isEmpty()) {
        loadPasswords();
        return;
    }

    const auto passwordList =
        m_passwordRepo.searchPasswords(userId, searchText);

    if (passwordList.isEmpty()) {
        setViewState(ViewState::NoResults, searchText);
        return;
    }

    setViewState(ViewState::Passwords);
    renderPasswords(passwordList);
}

void DashboardPage::onPasswordUpdate(const PasswordEntry& entry) {

    auto user = UserSession::instance().currentUser();

    if (!user.has_value()) {
        return;
    }
    
    int userId = user->id();

    bool isUpdated = m_passwordRepo.updatePassword(userId, entry);

    if (!isUpdated) {
        qDebug() << "password update failed";
        return;
    }

    loadPasswords();
}

void DashboardPage::onSearchTextChanged()
{
    m_searchDebounceTimer->start();
}

void DashboardPage::onRefreshPasswordsClicked()
{
    onSearchTextChanged();
}

void DashboardPage::setViewState(
    ViewState state,
    const QString& searchText
)
{
    switch (state) {
    case ViewState::Passwords:
        m_ui->stackedWidget->setCurrentWidget(m_ui->pagePasswords);
        break;

    case ViewState::NoResults:
        m_ui->noResultsLabel->setText(
            QString("No results found for \"%1\"").arg(searchText)
        );
        m_ui->stackedWidget->setCurrentWidget(m_ui->pageNoResults);
        break;

    case ViewState::Empty:

        m_ui->stackedWidget->setCurrentWidget(m_ui->pageEmpty);
        break;
    }
}

void DashboardPage::clearSearchText()
{
    m_ui->searchLineEdit->clear();
}

std::optional<ButtonMappings> DashboardPage::resolvePasswordIdFromSender() const
{
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn)
        return std::nullopt;

    const int id = m_buttonToPasswordId.value(btn, -1);
    if (id < 0)
        return std::nullopt;

    return ButtonMappings{ btn, id };
}

void DashboardPage::onCopyPassword()
{
    std::optional<ButtonMappings> btnMapping = resolvePasswordIdFromSender();
    if (!btnMapping)
        return;

    auto entry = m_passwordRepo.fetchById(btnMapping->passwordId);
    if (!entry.has_value()) {
        qDebug() << "Password not found:" << btnMapping->passwordId;
        return;
    }

    if (!verifyMasterPasswordIfRequired(this, entry.value()))
        return;

    auto* btn = btnMapping->button;

    btn->setText("Copied");
    QTimer::singleShot(2000, this, [btn]() {
        if (btn)
            btn->setText("Copy Pass");
        });

    QGuiApplication::clipboard()->setText(entry->password());
}

void DashboardPage::onDeletePassword()
{
    std::optional<ButtonMappings> btnMapping = resolvePasswordIdFromSender();
    if (!btnMapping)
        return;

    auto entry = m_passwordRepo.fetchById(btnMapping->passwordId);
    if (!entry.has_value()) {
        qDebug() << "Password not found:" << btnMapping->passwordId;
        return;
    }

    if (!verifyMasterPasswordIfRequired(this, entry.value()))
        return;

    const auto reply = QMessageBox::question(
        this,
        "Delete Password",
        "Are you sure you want to delete this password?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply != QMessageBox::Yes)
        return;

    m_passwordRepo.deletePasswordById(btnMapping->passwordId);
    emit passwordDeleteSuccess();
}

void DashboardPage::onEditPassword()
{
    std::optional<ButtonMappings> btnMapping = resolvePasswordIdFromSender();
    if (!btnMapping)
        return;

    auto entry = m_passwordRepo.fetchById(btnMapping->passwordId);
    if (!entry.has_value()) {
        qDebug() << "Password not found:" << btnMapping->passwordId;
        return;
    }

    if (!verifyMasterPasswordIfRequired(this, entry.value()))
        return;

    EditPasswordDialog dialog(entry.value(), this);

    connect(&dialog, &EditPasswordDialog::passwordUpdate,
        this, &DashboardPage::onPasswordUpdate);

    dialog.exec();
}

void DashboardPage::onCopyUsername()
{
    std::optional<ButtonMappings> btnMapping = resolvePasswordIdFromSender();
    if (!btnMapping)
        return;

    auto entry = m_passwordRepo.fetchById(btnMapping->passwordId);
    if (!entry.has_value()) {
        qDebug() << "Username not found for:" << btnMapping->passwordId;
        return;
    }

    auto* btn = btnMapping->button;

    btn->setText("copied");
    QTimer::singleShot(2000, this, [this, btn]() {
        btn->setText("Copy User");
        });

    QGuiApplication::clipboard()->setText(entry->username());
}
  
bool DashboardPage::verifyMasterPasswordIfRequired(
    QWidget* parent,
    const PasswordEntry& entry
)
{
    if (!entry.requiresMasterPassword())
        return true;

    MasterPasswordDialog dialog(parent);
    return dialog.exec() == QDialog::Accepted;
}
