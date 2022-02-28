from abc import abstractmethod


class IJsonSerializable:
    @abstractmethod
    def get_info(self) -> dict:
        pass

    @abstractmethod
    def reset_from_info(self, info: str) -> None:
        pass

