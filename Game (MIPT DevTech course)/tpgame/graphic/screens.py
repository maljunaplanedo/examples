from tpgame.graphic.graphics import Screen
from tpgame.graphic.graphics import Window


class FortressScreen(Screen):
    WINDOW_LINE_HEIGHT = 12

    def __init__(self, window: Window, fortress) -> None:
        super().__init__(window)
        self.fortress = fortress
        self.selected_soldier = [self.fortress.shop, 0]

    def up_down_event(self, key) -> None:
        if key == "w":
            new_index = max(0, self.selected_soldier[1] - 1)
        else:
            new_index = min(
                len(self.selected_soldier[0]) - 1, self.selected_soldier[1] + 1
            )
        self.selected_soldier[1] = new_index

    def right_left_event(self, key) -> None:
        items = [self.fortress.garrison,
                 self.fortress.guest, self.fortress.shop]
        if key == "a":
            new_squad_index = (items.index(self.selected_soldier[0]) -
                               1) % len(items)
        else:
            new_squad_index = (items.index(self.selected_soldier[0]) +
                               1) % len(items)
        if len(items[new_squad_index]):
            self.selected_soldier[0] = items[new_squad_index]
            self.selected_soldier[1] = 0

    def equip_event(self) -> None:
        if self.selected_soldier[0] == self.fortress.shop:
            self.fortress.recruit_soldier(self.selected_soldier[1])
        else:
            self.fortress.move_soldier(
                self.selected_soldier[1],
                self.selected_soldier[0] != self.fortress.guest,
            )
        if self.selected_soldier[1] >= len(self.selected_soldier[0]):
            if len(self.selected_soldier[0]) == 0:
                self.selected_soldier[0] = self.fortress.shop
                self.selected_soldier[1] = 0
            else:
                self.selected_soldier[1] -= 1

    def exit_event(self) -> None:
        self.fortress.throw_guest_away()

    def keyboard_event(self, key) -> None:
        if key in ["w", "s"]:
            self.up_down_event(key)
        elif key in ["a", "d"]:
            self.right_left_event(key)
        elif key == "e":
            self.equip_event()
        elif key == "Escape":
            self.exit_event()

    def draw(self) -> None:
        self.window.graphics_facade.draw_background(True)

        columns = [[]]
        for i in self.fortress.garrison.soldiers:
            columns[-1].append({"hp": i.hp,
                                "attack": i.attack, "armor": i.armor})
        if self.selected_soldier[0] == self.fortress.garrison:
            columns[-1][self.selected_soldier[1]]["selected"] = True

        columns.append([])
        for i in self.fortress.guest.soldiers:
            columns[-1].append({"hp": i.hp,
                                "attack": i.attack, "armor": i.armor})
        if self.selected_soldier[0] == self.fortress.guest:
            columns[-1][self.selected_soldier[1]]["selected"] = True

        columns.append([])
        for i in self.fortress.shop:
            columns[-1].append(
                {"attack": i[0].attack, "armor": i[0].armor, "cost": i[1]}
            )
        if self.selected_soldier[0] == self.fortress.shop:
            columns[-1][self.selected_soldier[1]]["selected"] = True

        for i in range(3):
            for j in range(len(columns[i])):
                line = columns[i][j]
                self.window.graphics_facade.draw_line_background(
                    i, j, "selected" in line.keys()
                )
                if "hp" in line.keys():
                    self.window.graphics_facade.draw_line_text(
                        i, j, str(line["hp"]), 0)
                self.window.graphics_facade.draw_line_text(
                    i, j, str(line["attack"]), 1)
                self.window.graphics_facade.draw_line_text(
                    i, j, str(line["armor"]), 2)
                if "cost" in line.keys():
                    self.window.graphics_facade.draw_line_text(
                        i, j, str(line["cost"]), 3
                    )

        self.window.graphics_facade.draw_panel_background()
        self.window.graphics_facade.draw_gold(self.fortress.master.gold)


class MapScreen(Screen):
    WINDOW_CELL_WIDTH = 16
    WINDOW_CELL_HEIGHT = 12

    def __init__(self, window: Window, map_) -> None:
        super().__init__(window)
        self.map = map_

    def movement_event(self, key) -> None:
        if key == "w":
            movement = (0, -1)
        elif key == "a":
            movement = (-1, 0)
        elif key == "s":
            movement = (0, 1)
        else:
            movement = (1, 0)
        self.map.move_selected_squad(*movement)

    def change_squad_event(self, key) -> None:
        if key == "Right":
            delta = 1
        else:
            delta = -1
        self.map.select_other_squad(delta)

    def keyboard_event(self, key) -> None:
        if self.map.turn != self.map.protagonist:
            return
        if key in ["w", "a", "s", "d"]:
            self.movement_event(key)
        elif key in ["Left", "Right"]:
            self.change_squad_event(key)

    def draw(self) -> None:
        if self.map.turn is None:
            return

        if self.map.selected_squad.is_garrison():
            fort = self.map.get_fort_by_garrison(self.map.selected_squad)
            camera_x = fort.x
            camera_y = fort.y
        else:
            camera_x = self.map.selected_squad.x
            camera_y = self.map.selected_squad.y

        diff_x = camera_x - self.WINDOW_CELL_WIDTH // 2 + 1
        diff_y = camera_y - self.WINDOW_CELL_HEIGHT // 2 + 1

        self.window.graphics_facade.draw_background()
        self.window.graphics_facade.draw_ground(
            -diff_x, -diff_y, self.map.WIDTH -
            diff_x - 1, self.map.HEIGHT - diff_y - 1
        )

        for i in self.map.fortresses:
            player_code = self.map.get_player_code(i.master)
            self.window.graphics_facade.draw_fortress(
                player_code, i.x - diff_x, i.y - diff_y
            )

        for i in self.map.squads:
            player_code = self.map.get_player_code(i.player)
            if not i.is_garrison():
                self.window.graphics_facade.draw_squad(
                    player_code, i.x - diff_x, i.y - diff_y
                )

        self.window.graphics_facade.draw_target(
            round(self.WINDOW_CELL_WIDTH / 2) - 1,
            round(self.WINDOW_CELL_HEIGHT / 2) - 1,
        )

        self.window.graphics_facade.draw_panel_background()
        self.window.graphics_facade.draw_gold(self.map.protagonist.gold)
        self.window.graphics_facade.draw_moves_left(self.map.moves_left)


class EndScreen(Screen):
    def __init__(self, window: Window, result: int) -> None:
        super().__init__(window)
        self.result = result

    def keyboard_event(self, key) -> None:
        self.window.close()

    def draw(self) -> None:
        if self.result == 0:
            text = "Win"
        elif self.result == 1:
            text = "Lose"
        else:
            text = "Draw"
        self.window.graphics_facade.draw_end_text(text)

