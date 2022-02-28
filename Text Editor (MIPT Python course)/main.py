import sys
from python_text_editor import App

if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise RuntimeError("Wrong usage. Should be 1 argument: path to the file.")
    app = App(sys.argv[1])
    app.run()
