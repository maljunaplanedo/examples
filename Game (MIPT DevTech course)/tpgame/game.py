from tpgame.map_ import Map
from tpgame.network import Network
from tpgame.graphic.graphics import Window
from tpgame.graphic.screens import MapScreen


class Game:
    def __init__(self) -> None:
        self.network = Network()
        self.map = Map(self)
        self.window = Window(self)
        self.main_loop_running = False

    def run(self) -> None:
        if self.network.check_file_data():
            MapScreen(self.window, self.map).open_()
        self.main_loop_running = True
        while self.main_loop_running:
            self.window.iteration()
            self.network.iteration()

    def stop(self) -> None:
        self.main_loop_running = False

