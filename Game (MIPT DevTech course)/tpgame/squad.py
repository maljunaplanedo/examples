from tpgame.json_serializable import IJsonSerializable
from tpgame.soldier import Soldier


class Squad(IJsonSerializable):
    def __init__(self, player, x: int = -1, y: int = -1) -> None:
        self.player = player
        self.x = x
        self.y = y
        self.soldiers = []

    def get_info(self) -> dict:
        player = self.player.map.get_player_code(self.player)
        info = {"player": player, "x": self.x, "y": self.y, "soldiers": []}

        for i in self.soldiers:
            info["soldiers"].append(i.get_info())

        return info

    def __len__(self) -> int:
        return len(self.soldiers)

    def reset_from_info(self, info: str) -> None:
        self.x = info["x"]
        self.y = info["y"]
        self.soldiers = []

        for i in info["soldiers"]:
            self.soldiers.append(Soldier(self))
            self.soldiers[-1].reset_from_info(i)

    def move(self, dx: int, dy: int) -> None:
        self.x += dx
        self.y += dy

    def add_soldier(self, soldier: Soldier) -> None:
        self.soldiers.append(soldier)

    def empty(self) -> bool:
        return len(self.soldiers) == 0

    def update(self) -> None:
        self.soldiers = [soldier for soldier in self.soldiers
                         if soldier.alive()]

    def size(self) -> int:
        return len(self.soldiers)

    def is_garrison(self) -> bool:
        return self.x == -1

    def fight(self, enemy) -> bool:
        while (not self.empty()) and (not enemy.empty()):
            for i in range(min(self.size(), enemy.size())):
                self.soldiers[i].fight(enemy.soldiers[i])
            self.update()
            enemy.update()
        if self.empty():
            # Defeat
            return False
        else:
            # Victory
            return True

    def unite(self, friend_squad) -> None:
        self.soldiers += friend_squad.soldiers
        friend_squad.soldiers = []

    def interact(self, other) -> None:
        if self.player == other.player:
            self.unite(other)
        else:
            self.fight(other)

