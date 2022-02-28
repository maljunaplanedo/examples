from python_text_editor import VisualsFacade, CursesFacade, File, Editor


class App:
    def __init__(self, filename: str):
        self.visuals = CursesFacade()
        self.file_manager = File(filename)
        self.editor = Editor(self)
        self.running = False

    def run(self):
        self.running = True
        while self.running:
            self.editor.iteration()

    def stop(self):
        self.running = False
