import sys
import os
from design import Ui_Form
from PySide2.QtWidgets import QApplication, QWidget
import threading
import subprocess

class Screen:
	def __init__(self, program, index):
		self.program = program
		self.index = index

	def init(self):
		pass

	def open(self):
		self.program.ui.stackedWidget.setCurrentIndex(self.index)


class MainMenu(Screen):
	def __init__(self, program):
		Screen.__init__(self, program, 0)

	def init(self):
		self.program.ui.documentsButton.clicked.connect(
				self.program.subMenu.open
			)

		self.program.ui.authorsButton.clicked.connect(
				self.program.authors.open
			)

		self.program.ui.exitButton.clicked.connect(
				lambda: sys.exit(0)
			)


class SubMenu(Screen):
	def __init__(self, program):
		Screen.__init__(self, program, 2)

	def init(self):

		self.program.ui.presentationsButton.clicked.connect(
				lambda: self.program.listMenu.open('presentations')
			)

		self.program.ui.booksButton.clicked.connect(
				lambda: self.program.listMenu.open('books')
			)

		self.program.ui.videosButton.clicked.connect(
				lambda: self.program.listMenu.open('videos')
			)

		self.program.ui.menuBackButton.clicked.connect(
				self.program.mainMenu.open
			)


class Authors(Screen):
	def __init__(self, program):
		Screen.__init__(self, program, 1)

	def init(self):

		self.program.ui.authorsBackButton.clicked.connect(
				self.program.mainMenu.open
			)


class ListMenu(Screen):
	def __init__(self, program):
		Screen.__init__(self, program, 3)

	def init(self):

		self.program.ui.listBackButton.clicked.connect(
				self.goBack
			)

		self.program.ui.listWidget.itemSelectionChanged.connect(
				self.checkOpenButton
			)

		self.program.ui.listOpenButton.clicked.connect(
				self.openFile
			)

	def open(self, doctype):
		
		self.path = os.getcwd() + '\\' + doctype
		files = [f for f in os.listdir(self.path) if os.path.isfile(self.path + '\\' + f)]

		for f in files:
			self.program.ui.listWidget.addItem(f)

		Screen.open(self)

	def goBack(self):
		self.program.ui.listWidget.clear()
		self.program.subMenu.open()

	def checkOpenButton(self):
		self.program.ui.listOpenButton.setEnabled(
				not(self.program.ui.listWidget.currentItem() is None)
			)

	def openFile(self):
		file = self.program.ui.listWidget.currentItem().text()
		opener = Opener('"' + self.path + '\\' + file + '"')
		opener.start()


class Opener(threading.Thread):
	def __init__(self, filename):
		threading.Thread.__init__(self)
		self.filename = filename
		self.daemon = True

	def run(self):
		CREATE_NO_WINDOW = 0x08000000
		subprocess.call(self.filename, creationflags = CREATE_NO_WINDOW, shell = True)


class Program(QWidget):
	def __init__(self, ui):
		QWidget.__init__(self)
		self.ui = ui
		self.ui.setupUi(self)

		self.mainMenu = MainMenu(self)
		self.subMenu = SubMenu(self)
		self.authors = Authors(self)
		self.listMenu = ListMenu(self)

		self.mainMenu.init()
		self.subMenu.init()
		self.authors.init()
		self.listMenu.init()

		self.mainMenu.open()
		


if __name__ == '__main__':
	app = QApplication(sys.argv)
	ui = Ui_Form()
	program = Program(ui)
	program.show()
	sys.exit(app.exec_())