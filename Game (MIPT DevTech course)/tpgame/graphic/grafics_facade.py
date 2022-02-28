from abc import abstractmethod


# Facade pattern
class GraphicsFacade:
    def __init__(self, window) -> None:
        self.window = window

    @abstractmethod
    def iteration(self) -> None:
        pass

    @abstractmethod
    def redraw(self) -> None:
        pass

    @abstractmethod
    def draw_ground(self, x1: int, y1: int, x2: int, y2: int) -> None:
        pass

    @abstractmethod
    def draw_squad(self, owner: int, x: int, y: int) -> None:
        pass

    @abstractmethod
    def draw_fortress(self, owner: int, x: int, y: int) -> None:
        pass

    @abstractmethod
    def draw_background(self, black: bool = False) -> None:
        pass

    @abstractmethod
    def draw_line_background(self, col: int,
                             row: int, is_selected: bool) -> int:
        pass

    @abstractmethod
    def draw_line_text(self, col: int, row: int, text: str, pos: int) -> None:
        pass

    @abstractmethod
    def draw_end_text(self, text: str) -> None:
        pass

    @abstractmethod
    def draw_panel_background(self) -> None:
        pass

    @abstractmethod
    def draw_gold(self, gold: int) -> None:
        pass

    @abstractmethod
    def draw_moves_left(self, moves: int) -> None:
        pass

    @abstractmethod
    def draw_target(self, row: int, col: int) -> None:
        pass

