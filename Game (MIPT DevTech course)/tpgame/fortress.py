import random
import copy
from tpgame.graphic import screens
from tpgame.json_serializable import IJsonSerializable
from tpgame.player import Player
from tpgame.soldier import Soldier


class Fortress(IJsonSerializable):
    SHOP_SIZE = 6
    FORTRESS_PROFIT = 100

    def __init__(self, game, x: int, y: int) -> None:
        self.game = game
        self.guest = None
        self.garrison = None
        self.shop = []
        self.master = None
        self.x = x
        self.y = y
        self.generate_shop()

    def get_info(self) -> dict:
        master = self.game.map.get_player_code(self.master)

        info = {
            "guest": self.game.map.get_index_of_squad(self.guest),
            "garrison": self.game.map.get_index_of_squad(self.garrison),
            "shop": [],
            "master": master,
            "x": self.x,
            "y": self.y,
        }

        for i in self.shop:
            info["shop"].append([i[0].get_info(), i[1]])

        return info

    def reset_from_info(self, info: str) -> None:
        self.guest = self.game.map.get_squad_by_index(info["guest"])
        self.garrison = self.game.map.get_squad_by_index(info["garrison"])
        self.master = self.game.map.get_player_by_code(info["master"])
        self.shop = []

        for i in info["shop"]:
            self.shop.append((Soldier(), i[1]))
            self.shop[-1][0].reset_from_info(i[0])

    def open_fortress_menu(self) -> None:
        screens.FortressScreen(self.game.window, self).open_()

    def change_master(self, master: Player) -> None:
        self.master = master

    def generate_shop(self) -> None:
        for i in range(self.SHOP_SIZE):
            soldier = Soldier(
                None,
                random.randint(Soldier.MIN_ARMOR, Soldier.MAX_ARMOR),
                random.randint(Soldier.MIN_ATTACK, Soldier.MAX_ATTACK),
            )
            self.shop.append((soldier, soldier.cost()))

    def recruit_soldier(self, index: int) -> bool:
        soldier_cost = self.shop[index]
        if soldier_cost[1] > self.guest.player.gold:
            return False
        self.guest.player.gold -= soldier_cost[1]
        self.guest.add_soldier(copy.copy(soldier_cost[0]))
        return True

    def move_soldier(self, index: int, to_guest: bool) -> None:
        from_, to_ = (
            (self.garrison, self.guest)
            if to_guest else (self.guest, self.garrison)
        )
        to_.add_soldier(from_.soldiers[index])
        from_.soldiers.pop(index)

    def close_fortress_menu(self) -> None:
        screens.MapScreen(self.game.window, self.game.map).open_()

    def check_garrison_existence(self) -> None:
        if self.garrison.empty():
            self.garrison = None

    def accept_visitor(self, squad) -> None:
        if self.master == self.game.map.antagonist and \
        self.garrison is not None:

            squad.fight(self.garrison)
            self.check_garrison_existence()
            self.game.map.remove_invalid_squads()
            if squad.empty():
                return

        self.change_master(squad.player)

        if self.garrison is None:
            self.game.map.add_squad(self.game.map.protagonist, -1, -1)
            self.garrison = self.game.map.squads[-1]

        self.guest = squad
        self.open_fortress_menu()

    def throw_guest_away(self) -> None:
        self.check_garrison_existence()
        if self.guest.empty():
            self.game.map.selected_squad = self.garrison
        else:
            self.game.map.selected_squad = self.guest
            self.game.map.move_selected_squad(0, -1)

        self.guest = None
        self.game.map.remove_invalid_squads()
        self.close_fortress_menu()

        self.game.map.send_state()

        if not self.game.map.check_game_end():
            self.game.map.check_turn_end()
        self.game.window.redraw()

