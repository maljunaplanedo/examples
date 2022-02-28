class Editor:
    def __init__(self, app):
        self.app = app

        self.line = 0
        self.col = 0
        self.camera_line = 0
        self.camera_col = 0

        self.notification = ''

        self.app.visuals.move(self.line, self.col)
        self.refresh()

    def iteration(self):
        event = self.app.visuals.get_event()
        if event == 'Resize':
            pass
        elif event == 'Del':
            self.handle_del()
        elif event == 'Backspace':
            self.handle_backspace()
        elif event == 'Right':
            self.move_right()
        elif event == 'Left':
            self.move_left()
        elif event == 'Up':
            self.move_up()
        elif event == 'Down':
            self.move_down()
        elif event == 'Alt+s':
            self.notification = 'SAVED'
            self.app.file_manager.write()
        elif event == 'Alt+q':
            self.app.stop()
        elif event == 'Alt':
            pass
        else:
            self.put(event)

        self.refresh()

    def refresh(self):
        visuals = self.app.visuals

        visuals.clear()

        width = visuals.width
        height = visuals.height

        for i in range(height - 1):
            if self.camera_line + i == len(self.app.file_manager):
                break

            visuals.move(i, 0)
            line = str(self.app.file_manager[self.camera_line + i])
            visuals.put(line[self.camera_col:self.camera_col + width])

        visuals.move(height - 1, 0)
        visuals.put(str(self.line + 1) + ':' + str(self.col + 1))

        if self.notification:
            visuals.move(height - 1, width - len(self.notification) - 1)
            visuals.put(self.notification)
            self.notification = ''

        visuals.move(self.line - self.camera_line, self.col - self.camera_col)
        visuals.refresh()

    def handle_del(self):
        file_manager = self.app.file_manager

        if self.col == len(file_manager[self.line]):
            if self.line == len(file_manager):
                return
            file_manager[self.line].add(str(file_manager[self.line + 1]))
            del file_manager[self.line + 1]
        else:
            del file_manager[self.line][self.col]

    def handle_backspace(self):
        if self.col == 0:
            if self.line == 0:
                return
            self.move_up()
            self.col = len(self.app.file_manager[self.line])
            self.camera_col = max(self.camera_col, self.col - self.app.visuals.width + 1)
        else:
            self.move_left()
        self.handle_del()

    def move_right(self):
        if self.col == len(self.app.file_manager[self.line]):
            return
        self.col += 1
        if self.col == self.camera_col + self.app.visuals.width:
            self.camera_col += 1

    def move_left(self):
        if self.col == 0:
            return
        if self.col == self.camera_col:
            self.camera_col -= 1
        self.col -= 1

    def move_up(self):
        if self.line == 0:
            return
        if self.line == self.camera_line:
            self.camera_line -= 1
        self.line -= 1
        self.col = min(self.col, len(self.app.file_manager[self.line]))

    def move_down(self):
        if self.line == len(self.app.file_manager) - 1:
            return
        self.line += 1
        if self.line == self.camera_line + self.app.visuals.height - 1:
            self.camera_line += 1
        self.col = min(self.col, len(self.app.file_manager[self.line]))

    def put(self, text: str):
        if text == '\n':
            line = self.app.file_manager[self.line]
            first_part = line[:self.col]
            second_part = line[self.col:]

            self.app.file_manager[self.line] = first_part
            self.app.file_manager.insert(self.line + 1, second_part)

            self.move_down()
            self.camera_col = 0
            self.col = 0
        else:
            self.app.file_manager[self.line].insert(self.col, text)
            self.move_right()
