import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.3
import QtQuick.Window 2.12
import Qt.labs.platform 1.1

Window {
    width: 320
    height: 260
    visible: true
    title: qsTr("Dr.com Login")

    function saveInput() {
        settings.setHost(textFieldHost.text)
        settings.setAccount(textFieldAccount.text)
        settings.setPassword(textFieldPassword.text)
        settings.setKeepLogin(checkBoxAutoLogin.checked)
    }

    ColumnLayout{
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10
        spacing: 10
        TextField {
            id: textFieldHost
            Layout.preferredWidth: parent.width
            placeholderText: qsTr("Host")
            text: settings.host()
        }
        TextField {
            id: textFieldAccount
            Layout.preferredWidth: parent.width
            placeholderText: qsTr("Account")
            text: settings.account()
        }
        TextField {
            id: textFieldPassword
            echoMode: TextInput.Password
            Layout.preferredWidth: parent.width
            placeholderText: qsTr("Password")
            text: settings.password()
        }

        RowLayout {
            spacing: 30
            Layout.preferredWidth: parent.width
            CheckBox {
                id: checkBoxAutoLogin
                text: qsTr("auto login")
                checked: settings.keepLogin()
                onCheckedChanged: {
                    settings.setKeepLogin(this.checked)

                    if (this.checked) {
                        drcomController.keepLogin(textFieldHost.text, textFieldAccount.text, textFieldPassword.text)
                    } else {
                        drcomController.setWorkerKeepLoginStop(true)
                    }
                }

                Component.onCompleted: {
                    if (checkBoxAutoLogin.checked) {
                        drcomController.keepLogin(textFieldHost.text, textFieldAccount.text, textFieldPassword.text)
                    }
                }
            }
            Button {
                id: btnLogin
                Layout.preferredWidth: parent.width/2
                Layout.alignment: Qt.AlignRight
                text: qsTr("Login")
                onClicked: {
                    btnLogin.enabled = false
                    drcomController.login(textFieldHost.text, textFieldAccount.text, textFieldPassword.text)
                    // 顺便保存配置
                    saveInput()
                }
            }
        }
        Button {
            id: btnLogout
            Layout.preferredWidth: parent.width
            text: qsTr("Logout")
            onClicked: {
                btnLogout.enabled = false
                drcomController.logout(textFieldHost.text)
                saveInput()
            }
        }
    }

    Connections {
        target: drcomController
        function onWorkerLoginSuccess() {
            dialog.text = "登录成功"
            dialog.open()
        }

        function onWorkerLoginDone() {
            btnLogin.enabled = true
        }

        function onWorkerLogoutSuccess() {
            dialog.text = "退出登录成功"
            dialog.open()
        }

        function onWorkerLogoutDone() {
            btnLogout.enabled = true
        }

        function onError(err) {
            dialog.text = err
            dialog.open()
        }
    }

    MessageDialog {
        id: dialog
    }
}
