from tpgame.json_serializable import IJsonSerializable


class Player(IJsonSerializable):
    def __init__(self, map_) -> None:
        self.gold = 500
        self.map = map_

    def get_info(self) -> dict:
        info = {"gold": self.gold}
        return info

    def reset_from_info(self, info: str) -> None:
        self.gold = info["gold"]

