from tpgame.network import INetworkEventSubscriber
from tpgame.json_serializable import IJsonSerializable
from tpgame.player import Player
from tpgame.squad import Squad
from tpgame.soldier import Soldier
from tpgame.graphic import screens
from tpgame.fortress import Fortress
import random


class Map(INetworkEventSubscriber, IJsonSerializable):
    WIDTH = 32
    HEIGHT = 32
    FORTRESSES_NUMBER = 12
    ONE_TURN_MOVES = 10

    def __init__(self, game) -> None:
        self.game = game
        self.squads = []
        self.fortresses = []
        self.protagonist = Player(self)
        self.antagonist = Player(self)
        self.turn = None
        self.selected_squad = None
        self.moves_left = 0

        self.game.network.subscribe("connect", self)
        self.game.network.subscribe("map_update", self)

    def get_info(self) -> dict:
        turn = self.get_player_code(self.turn)

        selected_squad = self.get_index_of_squad(self.selected_squad)

        info = {
            "squads": [],
            "fortresses": [],
            "selected_squad": selected_squad,
            "moves_left": self.moves_left,
            "turn": turn,
            "protagonist": self.protagonist.get_info(),
            "antagonist": self.antagonist.get_info(),
        }

        for i in self.squads:
            info["squads"].append(i.get_info())

        for i in self.fortresses:
            info["fortresses"].append(i.get_info())

        return info

    def reset_from_info(self, info: str) -> None:
        self.protagonist = Player(self)
        self.antagonist = Player(self)

        self.protagonist.reset_from_info(info["antagonist"])
        self.antagonist.reset_from_info(info["protagonist"])

        self.squads = []
        self.fortresses = []

        for i in info["squads"]:
            player = self.get_player_by_code(i["player"])
            self.squads.append(Squad(player))
            self.squads[-1].reset_from_info(i)

        for i in info["fortresses"]:
            self.fortresses.append(Fortress(self.game, i["x"], i["y"]))
            self.fortresses[-1].reset_from_info(i)

        self.moves_left = info["moves_left"]
        self.turn = self.get_player_by_code(info["turn"])
        self.selected_squad = self.get_squad_by_index(info["selected_squad"])

    def get_index_of_squad(self, squad: Squad) -> int:
        try:
            return self.squads.index(squad)
        except ValueError:
            return -1

    def get_squad_by_index(self, index: int) -> Squad:
        if index == -1:
            return None
        return self.squads[index]

    def get_player_code(self, player: Player) -> int:
        if player == self.antagonist:
            return 0
        elif player == self.protagonist:
            return 1
        return -1

    def get_player_by_code(self, code: int) -> Player:
        if code == -1:
            return None
        return self.protagonist if code == 0 else self.antagonist

    def handle_network_event(self, type_: str, event: dict) -> None:
        if type_ == "connect":
            self.on_network_connected(event["host"])
        elif type_ == "map_update":
            self.reset_from_info(event)
            if not self.check_game_end():
                self.check_turn_end()
        self.game.window.redraw()

    def on_network_connected(self, is_host: bool) -> None:
        if is_host:
            self.generate_map()
            self.send_state()

    def get_fort_by_garrison(self, squad: Squad) -> list:
        return [i for i in self.fortresses if i.garrison is squad][0]

    def move_selected_squad(self, dx: int, dy: int) -> None:
        if self.selected_squad.is_garrison():
            fort = self.get_fort_by_garrison(self.selected_squad)
            self.add_squad(self.protagonist, fort.x, fort.y)
            fort.guest = self.squads[-1]
            fort.open_fortress_menu()
            return

        x = self.selected_squad.x
        y = self.selected_squad.y
        x += dx
        y += dy
        if not self.is_valid_cell(x, y):
            return
        self.selected_squad.move(dx, dy)

        squads_at_cell = self.squads_at_cell(x, y)
        fort_at_cell = self.forts_at_cell(x, y)

        if fort_at_cell:
            fort_at_cell[0].accept_visitor(self.selected_squad)
            self.moves_left += 1
        elif len(squads_at_cell) == 2:
            other_squad = (
                squads_at_cell[0]
                if squads_at_cell[1] is self.selected_squad
                else squads_at_cell[1]
            )
            self.selected_squad.interact(other_squad)
            self.remove_invalid_squads()

        self.moves_left -= 1
        self.send_state()

        if not self.check_game_end():
            self.check_turn_end()

    def end_game(self, result: int) -> None:
        screens.EndScreen(self.game.window, result).open_()

    def check_game_end(self) -> bool:
        protagonist_forts = [i for i in self.fortresses
                             if i.master == self.protagonist]
        antagonist_forts = [i for i in self.fortresses
                            if i.master == self.antagonist]
        protagonist_squads = [
            i for i in self.squads
            if i.player == self.protagonist and not i.empty()
        ]
        antagonist_squads = [
            i for i in self.squads
            if i.player == self.antagonist and not i.empty()
        ]

        if len(protagonist_squads) == 0 and len(antagonist_squads) == 0:
            self.end_game(-1)
            return True
        elif len(protagonist_squads) == 0:
            self.end_game(1)
            return True
        elif len(antagonist_squads) == 0:
            self.end_game(0)
            return True
        elif len(protagonist_forts) == self.FORTRESSES_NUMBER:
            self.end_game(0)
            return True
        elif len(antagonist_forts) == self.FORTRESSES_NUMBER:
            self.end_game(1)
            return True
        return False

    def check_turn_end(self) -> None:
        if not self.moves_left:
            self.start_turn(
                self.protagonist
                if self.turn == self.antagonist else self.antagonist
            )

    def add_squad(self, player: Player, x: int, y: int) -> None:
        self.squads.append(Squad(player, x, y))

    def remove_invalid_squads(self) -> None:
        has_non_empty_squad = False
        for squad in self.squads:
            if squad.player == self.protagonist and not squad.empty():
                has_non_empty_squad = True
                break
        if not has_non_empty_squad:
            return
        while self.selected_squad.empty():
            self.select_other_squad()
        self.squads = [squad for squad in self.squads if not squad.empty()]

    def select_first_squad(self, player: Player) -> None:
        index = 0
        while self.squads[index].player != player:
            index += 1
        self.selected_squad = self.squads[index]

    def select_other_squad(self, delta: int = 1) -> None:
        index = (self.squads.index(self.selected_squad) +
                 delta) % len(self.squads)
        while self.squads[index].player != self.protagonist:
            index = (index + delta) % len(self.squads)
        self.selected_squad = self.squads[index]

    def send_state(self) -> None:
        state = self.get_info()
        self.game.network.send_message({"type": "map_update", "event": state})

    def squads_at_cell(self, x: int, y: int) -> list:
        return [squad for squad in self.squads
                if squad.x == x and squad.y == y]

    def forts_at_cell(self, x: int, y: int) -> list:
        return [fort for fort in self.fortresses
                if fort.x == x and fort.y == y]

    def is_valid_cell(self, x: int, y: int) -> bool:
        return x in range(self.WIDTH) and y in range(self.HEIGHT)

    def is_empty_cell(self, x: int, y: int) -> bool:
        return (
            self.is_valid_cell(x, y) and not
            self.squads_at_cell(x, y) and not
            self.forts_at_cell(x, y)
        )

    def start_turn(self, player: Player) -> None:
        self.moves_left = self.ONE_TURN_MOVES
        self.turn = player

        for i in self.fortresses:
            if i.master == player:
                player.gold += Fortress.FORTRESS_PROFIT

        self.select_first_squad(player)

    def generate_map(self) -> None:
        self.add_squad(self.protagonist, 0, 0)
        self.squads[-1].add_soldier(Soldier(self.squads[-1]))

        self.add_squad(self.antagonist, self.WIDTH - 1, self.HEIGHT - 1)
        self.squads[-1].add_soldier(Soldier(self.squads[-1]))

        for i in range(self.FORTRESSES_NUMBER):
            fort_x, fort_y = -1, -1
            while True:
                fort_x = random.randrange(self.WIDTH)
                fort_y = random.randrange(self.HEIGHT)
                if self.is_empty_cell(fort_x, fort_y) and self.is_empty_cell(
                    fort_x, fort_y - 1
                ):
                    break

            self.fortresses.append(Fortress(self.game, fort_x, fort_y))

        self.start_turn(self.protagonist
                        if random.randrange(2) else self.antagonist)

