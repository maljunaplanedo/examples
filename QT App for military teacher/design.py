# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'design.ui',
# licensing of 'design.ui' applies.
#
# Created: Tue Jan 21 22:31:54 2020
#      by: pyside2-uic  running on PySide2 5.13.1
#
# WARNING! All changes made in this file will be lost!

from PySide2 import QtCore, QtGui, QtWidgets

class Ui_Form(object):
    def setupUi(self, Form):
        Form.setObjectName("Form")
        Form.resize(797, 600)
        Form.setMinimumSize(QtCore.QSize(797, 600))
        Form.setMaximumSize(QtCore.QSize(797, 600))
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap(":/res/icon.ico"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        Form.setWindowIcon(icon)
        Form.setStyleSheet("")
        self.stackedWidget = QtWidgets.QStackedWidget(Form)
        self.stackedWidget.setEnabled(True)
        self.stackedWidget.setGeometry(QtCore.QRect(0, 0, 800, 600))
        self.stackedWidget.setStyleSheet("* {\n"
"    border: none;\n"
"}\n"
"\n"
"QStackedWidget > QWidget {    \n"
"    \n"
"    background-image: url(:/res/background.jpg);\n"
"}\n"
"\n"
"QPushButton {\n"
"    background: rgba(255, 255, 255, 128);\n"
"    font-size: 30px;\n"
"    border-radius: 30px;\n"
"    height: 150px;\n"
"}\n"
"\n"
"* {\n"
"    font-family: \'Calibri\';\n"
"    font-weight: bold;\n"
"    color: green;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background: rgba(255, 255, 255, 255);\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"    color: #52514e;\n"
"}")
        self.stackedWidget.setLineWidth(1)
        self.stackedWidget.setObjectName("stackedWidget")
        self.page = QtWidgets.QWidget()
        self.page.setObjectName("page")
        self.gridLayoutWidget = QtWidgets.QWidget(self.page)
        self.gridLayoutWidget.setGeometry(QtCore.QRect(-1, -1, 801, 601))
        self.gridLayoutWidget.setObjectName("gridLayoutWidget")
        self.gridLayout_2 = QtWidgets.QGridLayout(self.gridLayoutWidget)
        self.gridLayout_2.setSpacing(0)
        self.gridLayout_2.setContentsMargins(275, -1, 275, -1)
        self.gridLayout_2.setObjectName("gridLayout_2")
        self.exitButton = QtWidgets.QPushButton(self.gridLayoutWidget)
        self.exitButton.setObjectName("exitButton")
        self.gridLayout_2.addWidget(self.exitButton, 3, 0, 1, 1)
        self.documentsButton = QtWidgets.QPushButton(self.gridLayoutWidget)
        self.documentsButton.setObjectName("documentsButton")
        self.gridLayout_2.addWidget(self.documentsButton, 1, 0, 1, 1)
        self.authorsButton = QtWidgets.QPushButton(self.gridLayoutWidget)
        self.authorsButton.setObjectName("authorsButton")
        self.gridLayout_2.addWidget(self.authorsButton, 2, 0, 1, 1)
        self.stackedWidget.addWidget(self.page)
        self.page_2 = QtWidgets.QWidget()
        self.page_2.setStyleSheet("* {\n"
"    font-family: \'Calibri\';\n"
"    font-weight: bold;\n"
"    color: green;\n"
"}")
        self.page_2.setObjectName("page_2")
        self.gridLayoutWidget_2 = QtWidgets.QWidget(self.page_2)
        self.gridLayoutWidget_2.setGeometry(QtCore.QRect(-1, -1, 801, 601))
        self.gridLayoutWidget_2.setObjectName("gridLayoutWidget_2")
        self.gridLayout_3 = QtWidgets.QGridLayout(self.gridLayoutWidget_2)
        self.gridLayout_3.setContentsMargins(40, 40, 40, 40)
        self.gridLayout_3.setObjectName("gridLayout_3")
        self.authors = QtWidgets.QWidget(self.gridLayoutWidget_2)
        self.authors.setStyleSheet("#authors {\n"
"    background-color: rgba(255, 255, 255, 128);\n"
"    border-radius: 30px;\n"
"}")
        self.authors.setObjectName("authors")
        self.verticalLayoutWidget = QtWidgets.QWidget(self.authors)
        self.verticalLayoutWidget.setGeometry(QtCore.QRect(-1, -1, 1103, 441))
        self.verticalLayoutWidget.setObjectName("verticalLayoutWidget")
        self.verticalLayout = QtWidgets.QVBoxLayout(self.verticalLayoutWidget)
        self.verticalLayout.setContentsMargins(30, -1, -1, -1)
        self.verticalLayout.setObjectName("verticalLayout")
        self.label = QtWidgets.QLabel(self.verticalLayoutWidget)
        font = QtGui.QFont()
        font.setFamily("Calibri")
        font.setPointSize(30)
        font.setWeight(75)
        font.setBold(True)
        self.label.setFont(font)
        self.label.setStyleSheet("")
        self.label.setObjectName("label")
        self.verticalLayout.addWidget(self.label)
        self.label_4 = QtWidgets.QLabel(self.verticalLayoutWidget)
        font = QtGui.QFont()
        font.setFamily("Calibri")
        font.setPointSize(30)
        font.setWeight(75)
        font.setBold(True)
        self.label_4.setFont(font)
        self.label_4.setObjectName("label_4")
        self.verticalLayout.addWidget(self.label_4)
        self.label_5 = QtWidgets.QLabel(self.verticalLayoutWidget)
        font = QtGui.QFont()
        font.setFamily("Calibri")
        font.setPointSize(30)
        font.setWeight(75)
        font.setBold(True)
        self.label_5.setFont(font)
        self.label_5.setText("")
        self.label_5.setObjectName("label_5")
        self.verticalLayout.addWidget(self.label_5)
        self.label_6 = QtWidgets.QLabel(self.verticalLayoutWidget)
        font = QtGui.QFont()
        font.setFamily("Calibri")
        font.setPointSize(30)
        font.setWeight(75)
        font.setBold(True)
        self.label_6.setFont(font)
        self.label_6.setObjectName("label_6")
        self.verticalLayout.addWidget(self.label_6)
        self.label_2 = QtWidgets.QLabel(self.verticalLayoutWidget)
        font = QtGui.QFont()
        font.setFamily("Calibri")
        font.setPointSize(30)
        font.setWeight(75)
        font.setBold(True)
        self.label_2.setFont(font)
        self.label_2.setObjectName("label_2")
        self.verticalLayout.addWidget(self.label_2)
        self.gridLayout_3.addWidget(self.authors, 0, 0, 1, 1)
        self.authorsBackButton = QtWidgets.QPushButton(self.gridLayoutWidget_2)
        self.authorsBackButton.setStyleSheet("height: 70px;\n"
"margin-left:150px;\n"
"margin-right:150px;")
        self.authorsBackButton.setObjectName("authorsBackButton")
        self.gridLayout_3.addWidget(self.authorsBackButton, 1, 0, 1, 1)
        self.stackedWidget.addWidget(self.page_2)
        self.page_3 = QtWidgets.QWidget()
        self.page_3.setStyleSheet("QPushButton {\n"
"    height: 110px;\n"
"}")
        self.page_3.setObjectName("page_3")
        self.gridLayoutWidget_3 = QtWidgets.QWidget(self.page_3)
        self.gridLayoutWidget_3.setGeometry(QtCore.QRect(-1, -1, 801, 623))
        self.gridLayoutWidget_3.setObjectName("gridLayoutWidget_3")
        self.gridLayout = QtWidgets.QGridLayout(self.gridLayoutWidget_3)
        self.gridLayout.setContentsMargins(275, -1, 275, 20)
        self.gridLayout.setObjectName("gridLayout")
        self.booksButton = QtWidgets.QPushButton(self.gridLayoutWidget_3)
        self.booksButton.setObjectName("booksButton")
        self.gridLayout.addWidget(self.booksButton, 1, 0, 1, 1)
        self.videosButton = QtWidgets.QPushButton(self.gridLayoutWidget_3)
        self.videosButton.setObjectName("videosButton")
        self.gridLayout.addWidget(self.videosButton, 2, 0, 1, 1)
        self.presentationsButton = QtWidgets.QPushButton(self.gridLayoutWidget_3)
        self.presentationsButton.setObjectName("presentationsButton")
        self.gridLayout.addWidget(self.presentationsButton, 0, 0, 1, 1)
        self.menuBackButton = QtWidgets.QPushButton(self.gridLayoutWidget_3)
        self.menuBackButton.setObjectName("menuBackButton")
        self.gridLayout.addWidget(self.menuBackButton, 3, 0, 1, 1)
        self.stackedWidget.addWidget(self.page_3)
        self.page_4 = QtWidgets.QWidget()
        self.page_4.setObjectName("page_4")
        self.gridLayoutWidget_4 = QtWidgets.QWidget(self.page_4)
        self.gridLayoutWidget_4.setGeometry(QtCore.QRect(0, 0, 801, 601))
        self.gridLayoutWidget_4.setObjectName("gridLayoutWidget_4")
        self.gridLayout_4 = QtWidgets.QGridLayout(self.gridLayoutWidget_4)
        self.gridLayout_4.setSpacing(0)
        self.gridLayout_4.setContentsMargins(30, 30, 30, 30)
        self.gridLayout_4.setObjectName("gridLayout_4")
        self.listBackButton = QtWidgets.QPushButton(self.gridLayoutWidget_4)
        self.listBackButton.setStyleSheet("height: 110px;\n"
"margin-top: 20px;\n"
"margin-left: 20px;")
        self.listBackButton.setObjectName("listBackButton")
        self.gridLayout_4.addWidget(self.listBackButton, 1, 1, 1, 1)
        self.listOpenButton = QtWidgets.QPushButton(self.gridLayoutWidget_4)
        self.listOpenButton.setEnabled(False)
        self.listOpenButton.setStyleSheet("height: 110px;\n"
"margin-top: 20px;\n"
"margin-right: 20px;")
        self.listOpenButton.setObjectName("listOpenButton")
        self.gridLayout_4.addWidget(self.listOpenButton, 1, 0, 1, 1)
        self.listWidget = QtWidgets.QListWidget(self.gridLayoutWidget_4)
        self.listWidget.setStyleSheet("\n"
"QListWidget {\n"
"    background-color: rgba(255, 255, 255, 128);\n"
"    border: none;\n"
"    border-radius: 10px;\n"
"    padding: 10px;\n"
"    font-size: 40px;\n"
"}\n"
"\n"
"QListWidget::item {\n"
"    height: 70px;\n"
"}\n"
"\n"
"QListWidget::item:hover {\n"
"    background-color: white;\n"
"    color: green;\n"
"}\n"
"\n"
"QListWidget::item:selected {\n"
"    background-color: #71f594;\n"
"    border: none;\n"
"    color:green;\n"
"}")
        self.listWidget.setFrameShape(QtWidgets.QFrame.NoFrame)
        self.listWidget.setLineWidth(0)
        self.listWidget.setSelectionMode(QtWidgets.QAbstractItemView.SingleSelection)
        self.listWidget.setUniformItemSizes(False)
        self.listWidget.setBatchSize(100)
        self.listWidget.setSelectionRectVisible(False)
        self.listWidget.setObjectName("listWidget")
        self.gridLayout_4.addWidget(self.listWidget, 0, 0, 1, 2)
        self.stackedWidget.addWidget(self.page_4)

        self.retranslateUi(Form)
        self.stackedWidget.setCurrentIndex(1)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        Form.setWindowTitle(QtWidgets.QApplication.translate("Form", "Допризывная подготовка", None, -1))
        self.exitButton.setText(QtWidgets.QApplication.translate("Form", "ВЫХОД", None, -1))
        self.documentsButton.setText(QtWidgets.QApplication.translate("Form", "ДОКУМЕНТЫ", None, -1))
        self.authorsButton.setText(QtWidgets.QApplication.translate("Form", "АВТОРЫ", None, -1))
        self.label.setText(QtWidgets.QApplication.translate("Form", "АВТОР ИДЕИ: ", None, -1))
        self.label_4.setText(QtWidgets.QApplication.translate("Form", "...", None, -1))
        self.label_6.setText(QtWidgets.QApplication.translate("Form", "РАЗРАБОТЧИК:", None, -1))
        self.label_2.setText(QtWidgets.QApplication.translate("Form", "МАРАТ САЦКЕВИЧ", None, -1))
        self.authorsBackButton.setText(QtWidgets.QApplication.translate("Form", "НАЗАД", None, -1))
        self.booksButton.setText(QtWidgets.QApplication.translate("Form", "УЧЕБНИКИ", None, -1))
        self.videosButton.setText(QtWidgets.QApplication.translate("Form", "ВИДЕО", None, -1))
        self.presentationsButton.setText(QtWidgets.QApplication.translate("Form", "ПРЕЗЕНТАЦИИ", None, -1))
        self.menuBackButton.setText(QtWidgets.QApplication.translate("Form", "НАЗАД", None, -1))
        self.listBackButton.setText(QtWidgets.QApplication.translate("Form", "НАЗАД", None, -1))
        self.listOpenButton.setText(QtWidgets.QApplication.translate("Form", "ОТКРЫТЬ", None, -1))

import resources_rc
