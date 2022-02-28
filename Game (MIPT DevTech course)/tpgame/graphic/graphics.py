from abc import abstractmethod
from tpgame.graphic.gtk_facade import GtkCairoFacade


class Window:
    pass


# Screen is class for Screen events
# ( like fortress menu , win screen, lose screen)
class Screen:
    def __init__(self, window: Window) -> None:
        self.window = window

    def open_(self) -> None:
        self.window.change_screen(self)

    @abstractmethod
    def keyboard_event(self, key) -> None:
        pass

    @abstractmethod
    def draw(self) -> None:
        pass


class Window:
    WIDTH = 640
    HEIGHT = 480

    UsedGraphicsFacade = GtkCairoFacade

    def __init__(self, game) -> None:
        self.screen = None
        self.game = game
        self.graphics_facade = self.UsedGraphicsFacade(self)

    def iteration(self) -> None:
        self.graphics_facade.iteration()

    def redraw(self) -> None:
        self.graphics_facade.redraw()

    def change_screen(self, screen: Screen) -> None:
        self.screen = screen

    def close(self) -> None:
        self.game.stop()

    def draw(self) -> None:
        self.screen.draw()

    def keyboard_event(self, key) -> None:
        self.screen.keyboard_event(key)
        self.redraw()

    @property
    def width(self) -> int:
        return self.WIDTH

    @property
    def height(self) -> int:
        return self.HEIGHT

