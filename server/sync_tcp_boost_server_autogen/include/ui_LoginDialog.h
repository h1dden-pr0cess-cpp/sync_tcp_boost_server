/********************************************************************************
** Form generated from reading UI file 'LoginDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLineEdit *editLogin;
    QLineEdit *editPassword;
    QPushButton *btnLogin;
    QPushButton *btnRegister;
    QPushButton *btnToggleMode;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName("LoginDialog");
        LoginDialog->resize(320, 220);
        verticalLayout = new QVBoxLayout(LoginDialog);
        verticalLayout->setObjectName("verticalLayout");
        editLogin = new QLineEdit(LoginDialog);
        editLogin->setObjectName("editLogin");

        verticalLayout->addWidget(editLogin);

        editPassword = new QLineEdit(LoginDialog);
        editPassword->setObjectName("editPassword");
        editPassword->setEchoMode(QLineEdit::Password);

        verticalLayout->addWidget(editPassword);

        btnLogin = new QPushButton(LoginDialog);
        btnLogin->setObjectName("btnLogin");

        verticalLayout->addWidget(btnLogin);

        btnRegister = new QPushButton(LoginDialog);
        btnRegister->setObjectName("btnRegister");

        verticalLayout->addWidget(btnRegister);

        btnToggleMode = new QPushButton(LoginDialog);
        btnToggleMode->setObjectName("btnToggleMode");
        btnToggleMode->setFlat(true);

        verticalLayout->addWidget(btnToggleMode);


        retranslateUi(LoginDialog);

        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QCoreApplication::translate("LoginDialog", "\320\222\321\205\320\276\320\264", nullptr));
        editLogin->setPlaceholderText(QCoreApplication::translate("LoginDialog", "Login", nullptr));
        editPassword->setPlaceholderText(QCoreApplication::translate("LoginDialog", "Password", nullptr));
        btnLogin->setText(QCoreApplication::translate("LoginDialog", "\320\222\320\276\320\271\321\202\320\270", nullptr));
        btnRegister->setText(QCoreApplication::translate("LoginDialog", "\320\227\320\260\321\200\320\265\320\263\320\270\321\201\321\202\321\200\320\270\321\200\320\276\320\262\320\260\321\202\321\214\321\201\321\217", nullptr));
        btnToggleMode->setText(QCoreApplication::translate("LoginDialog", "\320\235\320\265\321\202 \320\260\320\272\320\272\320\260\321\203\320\275\321\202\320\260? \320\227\320\260\321\200\320\265\320\263\320\270\321\201\321\202\321\200\320\270\321\200\320\276\320\262\320\260\321\202\321\214\321\201\321\217", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
