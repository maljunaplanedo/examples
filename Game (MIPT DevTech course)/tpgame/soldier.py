from tpgame.json_serializable import IJsonSerializable


class Soldier(IJsonSerializable):
    DEFAULT_COST = 30
    DEFAULT_ARMOR = 0
    DEFAULT_ATTACK = 30
    BONUS_COST = 100
    MIN_ATTACK = 30
    MAX_ATTACK = 230
    MIN_ARMOR = 0
    MAX_ARMOR = 80

    def __init__(
        self, squad=None, armor: int = DEFAULT_ARMOR,
        attack: int = DEFAULT_ATTACK
    ) -> None:
        self.squad = squad
        self.armor = armor
        self.attack = attack
        self.hp = 100

    def get_info(self) -> dict:
        info = {"armor": self.armor, "attack": self.attack, "hp": self.hp}
        return info

    def reset_from_info(self, info: str) -> None:
        self.armor = info["armor"]
        self.attack = info["attack"]
        self.hp = info["hp"]

    def alive(self) -> bool:
        return self.hp > 0

    def fight(self, enemy) -> None:
        my_attack = self.attack * (100 - enemy.armor) // 100 + 1
        enemy_attack = enemy.attack * (100 - self.armor) // 100 + 1
        self.hp -= enemy_attack
        enemy.hp -= my_attack

    def cost(self) -> int:
        delta_armor = self.armor - self.MIN_ARMOR
        delta_attack = self.attack - self.MIN_ATTACK
        delta_armor_percent = delta_armor / (self.MAX_ARMOR - self.MIN_ARMOR)
        delta_attack_percent = delta_attack / (
            self.MAX_ATTACK - self.MIN_ATTACK)
        return self.DEFAULT_COST + int(
            self.BONUS_COST * (delta_armor_percent + delta_attack_percent)
        )

