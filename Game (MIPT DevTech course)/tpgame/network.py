import socket
from abc import abstractmethod
from collections import defaultdict, deque
import json


# Observer pattern
class INetworkEventSubscriber:
    @abstractmethod
    def handle_network_event(self, type_: str, event: dict) -> None:
        pass


class Network:
    MESSAGE_LEN = 16384

    def __init__(self) -> None:
        self.subscribers = defaultdict(list)
        self.socket = socket.socket()
        self.receiving_message = b""
        self.sending_messages = deque()

        self.target_ip = ""
        self.target_port = 0
        self.is_host = False

    def open_network_info_screen(self) -> None:
        pass

    def cause_event(self, type_: str, event: dict) -> None:
        for sub in self.subscribers[type_]:
            sub.handle_network_event(type_, event)

    @staticmethod
    def encode_message(message: str) -> str:
        message = json.dumps(message).encode()
        message += b"#" * (Network.MESSAGE_LEN - len(message))

        return message

    @staticmethod
    def decode_message(message: str) -> dict:
        message = message.rstrip(b"#")
        return json.loads(message.decode())

    def send_message(self, message: str) -> None:
        message = self.encode_message(message)
        self.sending_messages.append(message)

    def connect(self) -> None:
        if self.is_host:
            self.socket.bind(("", self.target_port))
            self.socket.listen(1)
            self.socket.setblocking(False)

            while True:
                try:
                    self.socket, address = self.socket.accept()
                    break
                except BlockingIOError:
                    pass

            self.socket.setblocking(False)
            self.cause_event("connect", {"host": True})
        else:
            self.socket.setblocking(False)

            while True:
                try:
                    self.socket.connect((self.target_ip, self.target_port))
                    break
                except BlockingIOError:
                    pass

            self.cause_event("connect", {"host": False})

    def __del__(self) -> None:
        self.socket.close()

    def subscribe(self, type_: str, subscriber) -> None:
        self.subscribers[type_].append(subscriber)

    def check_file_data(self) -> bool:
        try:
            with open("netconfig.txt", "r") as f:
                file = f.read()
                file.strip()

                self.target_ip, self.target_port = file.split()
                self.target_port = int(self.target_port)

                if self.target_ip == "host":
                    self.is_host = True

            self.connect()
            return True

        except FileNotFoundError:
            self.open_network_info_screen()
            return False
        except ValueError:
            exit(0)

    def receive_message(self) -> None:
        msg = self.decode_message(self.receiving_message)
        type_ = msg["type"]
        event = msg["event"]
        self.cause_event(type_, event)

    def receive_iteration(self) -> None:
        try:
            self.receiving_message += self.socket.recv(
                self.MESSAGE_LEN - len(self.receiving_message)
            )
            if len(self.receiving_message) == self.MESSAGE_LEN:
                self.receive_message()
                self.receiving_message = b""

        except BlockingIOError:
            pass

    def send_iteration(self) -> None:
        if not self.sending_messages:
            return
        try:
            bytes_sent = self.socket.send(
                self.sending_messages[0], len(self.sending_messages[0])
            )
            self.sending_messages[0] = self.sending_messages[0][bytes_sent:]
            if not self.sending_messages[0]:
                self.sending_messages.popleft()

        except BlockingIOError:
            pass

    def iteration(self):
        self.receive_iteration()
        self.send_iteration()

