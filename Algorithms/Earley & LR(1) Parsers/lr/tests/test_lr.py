import pytest
import typing as tp
from pytest_lazyfixture import lazy_fixture

from lr import Algo
from lr.src.lr import State, Cell
from grammar import Grammar, Rule


@pytest.fixture()
def grammar_cc() -> Grammar:
    return Grammar(terminals='cd', non_terminals='SC', start='S', rules=[
        Rule('S', 'CC'),
        Rule('C', 'cC'),
        Rule('C', 'd')
    ])


@pytest.fixture()
def grammar_ab() -> Grammar:
    return Grammar(terminals='ab', non_terminals='S', start='S', rules=[
        Rule('S', 'aSbS'),
        Rule('S', '')
    ])


@pytest.fixture()
def grammar_cc_items() -> tp.List[tp.Set[State]]:
    return [
        {
            State(Rule('@', 'S'), 0, '$'),
            State(Rule('S', 'CC'), 0, '$'),
            State(Rule('C', 'cC'), 0, 'c'),
            State(Rule('C', 'cC'), 0, 'd'),
            State(Rule('C', 'd'), 0, 'c'),
            State(Rule('C', 'd'), 0, 'd')
        },
        {
            State(Rule('@', 'S'), 1, '$')
        },
        {
            State(Rule('S', 'CC'), 1, '$'),
            State(Rule('C', 'cC'), 0, '$'),
            State(Rule('C', 'd'), 0, '$')
        },
        {
            State(Rule('C', 'cC'), 1, 'c'),
            State(Rule('C', 'cC'), 1, 'd'),
            State(Rule('C', 'cC'), 0, 'c'),
            State(Rule('C', 'cC'), 0, 'd'),
            State(Rule('C', 'd'), 0, 'c'),
            State(Rule('C', 'd'), 0, 'd')
        },
        {
            State(Rule('C', 'd'), 1, 'c'),
            State(Rule('C', 'd'), 1, 'd')
        },
        {
            State(Rule('S', 'CC'), 2, '$')
        },
        {
            State(Rule('C', 'cC'), 1, '$'),
            State(Rule('C', 'cC'), 0, '$'),
            State(Rule('C', 'd'), 0, '$')
        },
        {
            State(Rule('C', 'd'), 1, '$')
        },
        {
            State(Rule('C', 'cC'), 2, 'c'),
            State(Rule('C', 'cC'), 2, 'd')
        },
        {
            State(Rule('C', 'cC'), 2, '$')
        }
    ]


@pytest.fixture()
def grammar_ab_items() -> tp.List[tp.Set[State]]:
    return [
        {
            State(Rule('@', 'S'), 0, '$'),
            State(Rule('S', 'aSbS'), 0, '$'),
            State(Rule('S', ''), 0, '$')
        },
        {
            State(Rule('@', 'S'), 1, '$')
        },
        {
            State(Rule('S', 'aSbS'), 1, '$'),
            State(Rule('S', 'aSbS'), 0, 'b'),
            State(Rule('S', ''), 0, 'b')
        },
        {
            State(Rule('S', 'aSbS'), 2, '$')
        },
        {
            State(Rule('S', 'aSbS'), 1, 'b'),
            State(Rule('S', 'aSbS'), 0, 'b'),
            State(Rule('S', ''), 0, 'b')
        },
        {
            State(Rule('S', 'aSbS'), 3, '$'),
            State(Rule('S', 'aSbS'), 0, '$'),
            State(Rule('S', ''), 0, '$')
        },
        {
            State(Rule('S', 'aSbS'), 2, 'b')
        },
        {
            State(Rule('S', 'aSbS'), 4, '$')
        },
        {
            State(Rule('S', 'aSbS'), 3, 'b'),
            State(Rule('S', 'aSbS'), 0, 'b'),
            State(Rule('S', ''), 0, 'b')
        },
        {
            State(Rule('S', 'aSbS'), 4, 'b')
        }
    ]


@pytest.fixture()
def grammar_cc_control_table() -> tp.Tuple[tp.List[tp.Dict[str, Cell]], tp.List[tp.Dict[str, int]]]:
    return (
        [
            {
                'c': Cell(Cell.SHIFT, 3),
                'd': Cell(Cell.SHIFT, 4),
                '$': Cell(Cell.ERROR)
            },
            {
                'c': Cell(Cell.ERROR),
                'd': Cell(Cell.ERROR),
                '$': Cell(Cell.ACCEPT)
            },
            {
                'c': Cell(Cell.SHIFT, 6),
                'd': Cell(Cell.SHIFT, 7),
                '$': Cell(Cell.ERROR)
            },
            {
                'c': Cell(Cell.SHIFT, 3),
                'd': Cell(Cell.SHIFT, 4),
                '$': Cell(Cell.ERROR)
            },
            {
                'c': Cell(Cell.REDUCE, Rule('C', 'd')),
                'd': Cell(Cell.REDUCE, Rule('C', 'd')),
                '$': Cell(Cell.ERROR)
            },
            {
                'c': Cell(Cell.ERROR),
                'd': Cell(Cell.ERROR),
                '$': Cell(Cell.REDUCE, Rule('S', 'CC'))
            },
            {
                'c': Cell(Cell.SHIFT, 6),
                'd': Cell(Cell.SHIFT, 7),
                '$': Cell(Cell.ERROR)
            },
            {
                'c': Cell(Cell.ERROR),
                'd': Cell(Cell.ERROR),
                '$': Cell(Cell.REDUCE, Rule('C', 'd'))
            },
            {
                'c': Cell(Cell.REDUCE, Rule('C', 'cC')),
                'd': Cell(Cell.REDUCE, Rule('C', 'cC')),
                '$': Cell(Cell.ERROR)
            },
            {
                'c': Cell(Cell.ERROR),
                'd': Cell(Cell.ERROR),
                '$': Cell(Cell.REDUCE, Rule('C', 'cC'))
            }
        ],
        [
            {
                '@': -1,
                'S': 1,
                'C': 2
            },
            {
                '@': -1,
                'S': -1,
                'C': -1
            },
            {
                '@': -1,
                'S': -1,
                'C': 5
            },
            {
                '@': -1,
                'S': -1,
                'C': 8
            },
            {
                '@': -1,
                'S': -1,
                'C': -1
            },
            {
                '@': -1,
                'S': -1,
                'C': -1
            },
            {
                '@': -1,
                'S': -1,
                'C': 9
            },
            {
                '@': -1,
                'S': -1,
                'C': -1
            },
            {
                '@': -1,
                'S': -1,
                'C': -1
            },
            {
                '@': -1,
                'S': -1,
                'C': -1
            }
        ]
    )


@pytest.fixture()
def grammar_ab_control_table() -> tp.Tuple[tp.List[tp.Dict[str, Cell]], tp.List[tp.Dict[str, int]]]:
    return (
        [
            {
                'a': Cell(Cell.SHIFT, 2),
                'b': Cell(Cell.ERROR),
                '$': Cell(Cell.REDUCE, Rule('S', ''))
            },
            {
                'a': Cell(Cell.ERROR),
                'b': Cell(Cell.ERROR),
                '$': Cell(Cell.ACCEPT)
            },
            {
                'a': Cell(Cell.SHIFT, 4),
                'b': Cell(Cell.REDUCE, Rule('S', '')),
                '$': Cell(Cell.ERROR)
            },
            {
                'a': Cell(Cell.ERROR),
                'b': Cell(Cell.SHIFT, 5),
                '$': Cell(Cell.ERROR)
            },
            {
                'a': Cell(Cell.SHIFT, 4),
                'b': Cell(Cell.REDUCE, Rule('S', '')),
                '$': Cell(Cell.ERROR)
            },
            {
                'a': Cell(Cell.SHIFT, 2),
                'b': Cell(Cell.ERROR),
                '$': Cell(Cell.REDUCE, Rule('S', ''))
            },
            {
                'a': Cell(Cell.ERROR),
                'b': Cell(Cell.SHIFT, 8),
                '$': Cell(Cell.ERROR)
            },
            {
                'a': Cell(Cell.ERROR),
                'b': Cell(Cell.ERROR),
                '$': Cell(Cell.REDUCE, Rule('S', 'aSbS'))
            },
            {
                'a': Cell(Cell.SHIFT, 4),
                'b': Cell(Cell.REDUCE, Rule('S', '')),
                '$': Cell(Cell.ERROR)
            },
            {
                'a': Cell(Cell.ERROR),
                'b': Cell(Cell.REDUCE, Rule('S', 'aSbS')),
                '$': Cell(Cell.ERROR)
            }
        ],
        [
            {
                '@': -1,
                'S': 1
            },
            {
                '@': -1,
                'S': -1
            },
            {
                '@': -1,
                'S': 3
            },
            {
                '@': -1,
                'S': -1
            },
            {
                '@': -1,
                'S': 6
            },
            {
                '@': -1,
                'S': 7
            },
            {
                '@': -1,
                'S': -1
            },
            {
                '@': -1,
                'S': -1
            },
            {
                '@': -1,
                'S': 9
            },
            {
                '@': -1,
                'S': -1
            }
        ]
    )


@pytest.mark.parametrize('grammar, tests', [
    (
        Grammar(terminals='abcd', non_terminals='ABCDEFGHIJK', start='A', rules=[
            Rule('A', 'BC'),
            Rule('B', 'DEFG'),
            Rule('C', 'HIJK'),

            Rule('D', 'E'),
            Rule('E', 'a'),
            Rule('E', ''),
            Rule('F', 'G'),
            Rule('G', 'b'),

            Rule('H', 'I'),
            Rule('I', 'c'),
            Rule('I', ''),
            Rule('J', 'K'),
            Rule('K', 'd')
        ]),
        [
            ('A', {'a', 'b'}),
            ('B', {'a', 'b'}),
            ('', set()),
            ('C', {'c', 'd'}),
            ('J', {'d'}),
            ('Dcb', {'a', 'c'})
        ]
    ),
    (
        Grammar(terminals='abc', non_terminals='SABQ', rules=[
            Rule('S', 'AB'),
            Rule('S', 'AQ'),
            Rule('Q', ''),
            Rule('S', 'aB'),
            Rule('S', 'Ab'),
            Rule('A', 'ab'),
            Rule('A', ''),
            Rule('B', 'b')
        ]),
        [
            ('S', {'a', 'b'}),
            ('Acbab', {'a', 'c'}),
            ('Bcbab', {'b'}),
            ('Scbab', {'a', 'b', 'c'}),
            ('Q', set())
        ]
    )
])
def test_first(grammar: Grammar, tests: tp.List[tp.Tuple[str, tp.Set[str]]]):
    algo = Algo.fit(grammar)
    for word, expected in tests:
        assert algo.first(word) == expected


@pytest.mark.parametrize('grammar, tests', [
    (
        lazy_fixture('grammar_cc'),
        [
            (
                    {
                        State(Rule('@', 'S'), 0, '$')
                    },
                    {
                        State(Rule('@', 'S'), 0, '$'),
                        State(Rule('S', 'CC'), 0, '$'),
                        State(Rule('C', 'cC'), 0, 'c'),
                        State(Rule('C', 'cC'), 0, 'd'),
                        State(Rule('C', 'd'), 0, 'c'),
                        State(Rule('C', 'd'), 0, 'd')
                    }
            ),
            (
                    {
                        State(Rule('C', 'cC'), 1, 'c'),
                        State(Rule('C', 'cC'), 1, 'd'),
                        State(Rule('C', 'cC'), 0, 'c')
                    },
                    {
                        State(Rule('C', 'cC'), 1, 'c'),
                        State(Rule('C', 'cC'), 1, 'd'),
                        State(Rule('C', 'cC'), 0, 'c'),
                        State(Rule('C', 'cC'), 0, 'd'),
                        State(Rule('C', 'd'), 0, 'c'),
                        State(Rule('C', 'd'), 0, 'd')
                    }
            )
        ]
    ),
    (
        lazy_fixture('grammar_ab'),
        [
            (
                    {
                        State(Rule('S', 'aSbS'), 1, 'b')
                    },
                    {
                        State(Rule('S', 'aSbS'), 1, 'b'),
                        State(Rule('S', 'aSbS'), 0, 'b'),
                        State(Rule('S', ''), 0, 'b')
                    }
            ),
            (
                    {
                        State(Rule('S', 'aSbS'), 3, '$'),
                        State(Rule('S', ''), 0, '$')
                    },
                    {
                        State(Rule('S', 'aSbS'), 3, '$'),
                        State(Rule('S', 'aSbS'), 0, '$'),
                        State(Rule('S', ''), 0, '$')
                    }
            )
        ]
    )
])
def test_closure(grammar: Grammar, tests: tp.List[tp.Tuple[tp.Set[State], tp.Set[State]]]):
    algo = Algo.fit(grammar)
    for set_of_states, expected in tests:
        assert algo.closure(set_of_states) == expected


@pytest.mark.parametrize('grammar, tests', [
    (
        lazy_fixture('grammar_cc'),
        [
            (
                    {
                        State(Rule('@', 'S'), 0, '$'),
                        State(Rule('S', 'CC'), 0, '$'),
                        State(Rule('C', 'cC'), 0, 'c'),
                        State(Rule('C', 'cC'), 0, 'd'),
                        State(Rule('C', 'd'), 0, 'c'),
                        State(Rule('C', 'd'), 0, 'd')
                    },
                    'c',
                    {
                        State(Rule('C', 'cC'), 1, 'c'),
                        State(Rule('C', 'cC'), 1, 'd'),
                        State(Rule('C', 'cC'), 0, 'c'),
                        State(Rule('C', 'cC'), 0, 'd'),
                        State(Rule('C', 'd'), 0, 'c'),
                        State(Rule('C', 'd'), 0, 'd')
                    }
            ),
            (
                    {
                        State(Rule('S', 'CC'), 1, '$'),
                        State(Rule('C', 'cC'), 0, '$'),
                        State(Rule('C', 'd'), 0, '$')
                    },
                    'C',
                    {
                        State(Rule('S', 'CC'), 2, '$')
                    }
            )
        ]
    ),
    (
        lazy_fixture('grammar_ab'),
        [
            (
                    {
                        State(Rule('S', 'aSbS'), 1, '$'),
                        State(Rule('S', 'aSbS'), 0, 'b'),
                        State(Rule('S', ''), 0, 'b')
                    },
                    'a',
                    {
                        State(Rule('S', 'aSbS'), 1, 'b'),
                        State(Rule('S', 'aSbS'), 0, 'b'),
                        State(Rule('S', ''), 0, 'b')
                    }
            ),
            (
                    {
                        State(Rule('S', 'aSbS'), 3, '$'),
                        State(Rule('S', 'aSbS'), 0, '$'),
                        State(Rule('S', ''), 0, '$')
                    },
                    'S',
                    {
                        State(Rule('S', 'aSbS'), 4, '$')
                    }
            )
        ]
    )
])
def test_goto(grammar: Grammar, tests: tp.List[tp.Tuple[tp.Set[State], str, tp.Set[State]]]):
    algo = Algo.fit(grammar)
    for superstate, symbol, expected in tests:
        assert algo.goto_function(superstate, symbol) == expected


@pytest.mark.parametrize('grammar, expected', [
    (
        lazy_fixture('grammar_cc'),
        lazy_fixture('grammar_cc_items')
    ),
    (
        lazy_fixture('grammar_ab'),
        lazy_fixture('grammar_ab_items')
    )
])
def test_items(grammar: Grammar, expected: tp.List[tp.Set[State]]):
    algo = Algo.fit(grammar)
    items = algo.items()
    assert len(items) == len(expected)
    for superstate in items:
        assert superstate in expected
    for superstate in expected:
        assert superstate in items


@pytest.mark.parametrize('grammar, expected_items, expected_table', [
    (
        lazy_fixture('grammar_cc'),
        lazy_fixture('grammar_cc_items'),
        lazy_fixture('grammar_cc_control_table')
    ),
    (
        lazy_fixture('grammar_ab'),
        lazy_fixture('grammar_ab_items'),
        lazy_fixture('grammar_ab_control_table')
    )
])
def test_control_table(grammar: Grammar, expected_items: tp.List[tp.Set[State]],
                       expected_table: tp.Tuple[tp.List[tp.Dict[str, Cell]], tp.List[tp.Dict[str, int]]]):
    algo = Algo.fit(grammar)
    items = algo.items()

    assert len(items) == len(expected_items)
    actual_to_expected = [-1 for i in range(len(items))]

    for index, superstate in enumerate(items):
        actual_to_expected[index] = expected_items.index(superstate)
    for superstate in expected_items:
        assert superstate in items

    expected_control_table, expected_goto = expected_table

    assert len(expected_control_table) == len(items) and len(algo.control_table) == len(items)

    for index, row in enumerate(algo.control_table):
        assert len(row) == len(algo.grammar.terminals)
        index_in_expected = actual_to_expected[index]
        assert len(expected_control_table[index_in_expected]) == len(algo.grammar.terminals)

        for symbol, cell in row.items():
            assert cell.command == expected_control_table[index_in_expected][symbol].command
            if cell.command == Cell.SHIFT:
                assert actual_to_expected[cell.how] == expected_control_table[index_in_expected][symbol].how
            else:
                assert cell.how == expected_control_table[index_in_expected][symbol].how

    assert len(expected_goto) == len(items) and len(algo.goto) == len(items)

    for index, row in enumerate(algo.goto):
        assert len(row) == len(algo.grammar.non_terminals)
        index_in_expected = actual_to_expected[index]
        assert len(expected_goto[index_in_expected]) == len(algo.grammar.non_terminals)

        for symbol, to in row.items():
            if to == -1:
                assert expected_goto[index_in_expected][symbol] == -1
            else:
                assert actual_to_expected[to] == expected_goto[index_in_expected][symbol]


@pytest.mark.parametrize('grammar, tests', [
    (
        lazy_fixture('grammar_cc'),
        [
            ('dd', True),
            ('d', False),
            ('', False),
            ('cdd', True),
            ('dcd', True),
            ('c', False),
            ('cc', False),
            ('cdcd', True),
            ('ccccccccccccccccccccd', False),
            ('dccccccccccccccccccccccccccccccccc', False),
            ('cccccccccccccccdd', True),
            ('dccccccccccccccccd', True),
            ('ccccccccccccdcccccccccccccccccccccccccccccd', True),
            ('cccccccdccd', True),
            ('cccccccdccccccccccccccccd', True),
            ('ccccccccccdccccccccccccdd', False),
            ('cccccccccccdcccccccccccdcccd', False)
        ]
    ),
    (
        lazy_fixture('grammar_ab'),
        [
            ('', True),
            ('ab', True),
            ('aabb', True),
            ('abab', True),
            ('aabbab', True),
            ('aaaaabbbaabbbaabbababbaaaabbbabbaabbaabb', True),
            ('aaaaabbbaabbaaabbababbaaaabbbabbaabbaabb', False),
            ('aab', False),
            ('ababbaabbbababbabbababaaababaabbbaaabab', False),
            ('a', False),
            ('aaaaaaabbbbbaaaabbbbbaaaababababbbbaaaaaabbbbbbbaaaaaaabbbbbbb', True),
            ('aaaaaaabbbbbaaaabbbbbaaaababababbbbaaaaaaabbbbbbbaaaaaaabbbbbbb', False)
        ]
    )
])
def test_lr(grammar: Grammar, tests: tp.List[tp.Tuple[str, bool]]):
    algo = Algo.fit(grammar)
    for word, expected in tests:
        assert algo.predict(word) == expected
