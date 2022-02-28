from abc import abstractmethod
import curses


class VisualsFacade:
    @abstractmethod
    def __init__(self):
        pass

    @abstractmethod
    def __del__(self):
        pass

    @abstractmethod
    def put(self, text: str):
        pass

    @abstractmethod
    def move(self, y: int, x: int):
        pass

    @abstractmethod
    def clear(self):
        pass

    @abstractmethod
    def refresh(self):
        pass

    @abstractmethod
    def get_event(self) -> str:
        pass

    @property
    @abstractmethod
    def width(self) -> int:
        pass

    @property
    @abstractmethod
    def height(self) -> int:
        pass

    @property
    @abstractmethod
    def x(self) -> int:
        pass

    @property
    @abstractmethod
    def y(self) -> int:
        pass


class CursesFacade(VisualsFacade):
    def __init__(self):
        super().__init__()
        self.main_screen = curses.initscr()
        curses.noecho()
        curses.cbreak()
        self.main_screen.keypad(True)

    def __del__(self):
        curses.nocbreak()
        self.main_screen.keypad(False)
        curses.echo()
        curses.endwin()

    def put(self, text: str):
        self.main_screen.addstr(text)

    def move(self, y: int, x: int):
        self.main_screen.move(y, x)

    def clear(self):
        self.main_screen.clear()

    def get_event(self) -> str:
        result = self.main_screen.getch()
        if result == curses.KEY_RESIZE:
            return 'Resize'
        elif result == 330:
            return 'Del'
        elif result == curses.KEY_BACKSPACE:
            return 'Backspace'
        elif result == curses.KEY_RIGHT:
            return 'Right'
        elif result == curses.KEY_LEFT:
            return 'Left'
        elif result == curses.KEY_DOWN:
            return 'Down'
        elif result == curses.KEY_UP:
            return 'Up'
        elif result == 27:
            self.main_screen.nodelay(True)
            second_key = self.main_screen.getch()
            self.main_screen.nodelay(False)

            if second_key == ord('s'):
                return 'Alt+s'
            elif second_key == ord('q'):
                return 'Alt+q'
            else:
                return 'Alt'
        else:
            return chr(result)

    def refresh(self):
        self.main_screen.refresh()

    @property
    def width(self) -> int:
        return self.main_screen.getmaxyx()[1]

    @property
    def height(self) -> int:
        return self.main_screen.getmaxyx()[0]

    @property
    def x(self) -> int:
        return self.main_screen.getyx()[1]

    @property
    def y(self) -> int:
        return self.main_screen.getyx()[0]