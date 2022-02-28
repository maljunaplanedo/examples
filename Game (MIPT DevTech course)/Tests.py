import unittest
from tpgame.player import Player
from tpgame.squad import Squad
from tpgame.soldier import Soldier
from tpgame.fortress import Fortress
from tpgame.game import Game
from tpgame.map_ import Map


class TestPlayer(unittest.TestCase):
    def setUp(self):
        self.player = Player(None)

    def test_change_gold(self):
        self.assertEqual(self.player.gold, 500)
        self.player.reset_from_info({"gold": 600})
        self.assertEqual(self.player.gold, 600)


class TestSquad(unittest.TestCase):
    def setUp(self):
        self.player = Player(None)
        self.squad = Squad(self.player, 0, 0)

    def test_info(self):
        self.assertIs(self.squad.player, self.player)
        self.assertEqual(self.squad.x, 0)
        self.assertEqual(self.squad.y, 0)
        self.squad.reset_from_info({"x": 5, "y": 8, "soldiers": {}})
        self.assertEqual(self.squad.x, 5)
        self.assertEqual(self.squad.y, 8)

    def test_move(self):
        self.assertEqual((self.squad.x, self.squad.y), (0, 0))
        self.squad.move(10, 15)
        self.assertEqual((self.squad.x, self.squad.y), (10, 15))
        self.squad.move(-10, -15)
        self.assertEqual((self.squad.x, self.squad.y), (0, 0))
        self.squad.move(3, -8)
        self.assertEqual((self.squad.x, self.squad.y), (3, -8))


class TestSoldier(unittest.TestCase):
    def setUp(self):
        self.player = Player(None)
        self.squad = Squad(self.player, 0, 0)
        self.soldier = Soldier(self.squad, 100, 10)

    def test_info(self):
        self.assertEqual(
            self.soldier.get_info(), {"armor": 100, "attack": 10, "hp": 100}
        )
        self.soldier.reset_from_info({"armor": 50, "attack": 30, "hp": 5})
        self.assertEqual(self.soldier.get_info(),
                         {"armor": 50, "attack": 30, "hp": 5})

    def test_alive(self):
        self.soldier.reset_from_info({"armor": 50, "attack": 30, "hp": -5})
        self.assertFalse(self.soldier.alive())

    def test_fight(self):
        self.player1 = Player(None)
        self.squad1 = Squad(self.player1, 0, 0)
        self.soldier1 = Soldier(self.squad1, 10, 300)
        self.player2 = Player(None)
        self.squad2 = Squad(self.player2, 0, 0)
        self.soldier2 = Soldier(self.squad2, 30, 300)
        self.soldier1.fight(self.soldier2)
        self.assertFalse(self.soldier1.alive())
        self.assertEqual(
            self.soldier1.get_info(), {"armor": 10, "attack": 300, "hp": -171}
        )
        self.assertEqual(
            self.soldier2.get_info(), {"armor": 30, "attack": 300, "hp": -111}
        )


class TestFortress(unittest.TestCase):
    def setUp(self):
        self.fortress = Fortress(None, 0, 0)
        self.player = Player(None)

    def test_change_master(self):
        self.assertIsNone(self.fortress.master)
        self.fortress.change_master(self.player)
        self.assertIs(self.fortress.master, self.player)


class TestMap(unittest.TestCase):
    def setUp(self):
        self.map = Map(Game())

    def test_get_player_code(self):
        self.assertIsNone(self.map.get_player_by_code(-1))
        self.assertIs(self.map.get_player_by_code(0), self.map.protagonist)
        self.assertIs(self.map.get_player_by_code(1), self.map.antagonist)

    def test_player_code(self):
        self.assertEqual(self.map.get_player_code(self.map.protagonist), 1)
        self.assertEqual(self.map.get_player_code(self.map.antagonist), 0)
        self.assertEqual(self.map.get_player_code(Player(None)), -1)

    def test_add_squad(self):
        self.map.add_squad(Player(None), 0, 0)
        squad = self.map.squads_at_cell(0, 0)[0]
        self.assertIn(squad, self.map.squads)


if __name__ == "__main__":
    unittest.main()
