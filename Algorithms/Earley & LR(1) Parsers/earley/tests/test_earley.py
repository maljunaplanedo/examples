import pytest
import typing as tp
from grammar import Grammar, Rule
from earley import Algo
from earley.src.earley import Situation


@pytest.mark.parametrize('grammar, word, j, situations, expected', [
    (
        Grammar(terminals='acd', non_terminals='ADQ', start='Q'),
        'bac',
        2,
        [
            set(),
            {
                Situation(Rule('A', 'cDaQd'), 2, 0),
                Situation(Rule('A', 'daQ'), 2, 0),
                Situation(Rule('Q', 'aDQ'), 0, 1)
            },
            set(), set()
        ],
        {
            Situation(Rule('A', 'cDaQd'), 3, 0),
            Situation(Rule('Q', 'aDQ'), 1, 1)
        }
    ),
    (
        Grammar(terminals='ab', non_terminals='LZM', start='L'),
        'bbbb',
        3,
        [
            set(), set(),
            {
                Situation(Rule('L', 'Zb'), 2, 1),
                Situation(Rule('M', 'ZMbZZM'), 2, 2),
                Situation(Rule('Z', ''), 0, 2)
            },
            set(), set()
        ],
        {
            Situation(Rule('M', 'ZMbZZM'), 3, 2)
        }
    )
])
def test_Scan(grammar: Grammar, word: str, j: int, situations: tp.List[tp.Set[Situation]],
              expected: tp.Set[Situation]):
    algo = Algo.fit(grammar)
    algo.Scan(situations, word, j)
    assert situations[j] == expected


@pytest.mark.parametrize('grammar, j, situations, expected, times', [
    (
        Grammar(terminals='abc', non_terminals='SABC', start='S', rules=[
            Rule('A', 'AaA'),
            Rule('A', 'bcBCc'),
            Rule('B', 'ccbB'),
            Rule('B', 'B'),
            Rule('S', 'BS'),
            Rule('S', 'SA'),
            Rule('C', '')
        ]),
        1,
        [
            set(),
            {
                Situation(Rule('B', 'B'), 0, 0),
                Situation(Rule('S', 'BS'), 1, 1),
                Situation(Rule('A', 'bcBCc'), 3, 1)
            }
        ],
        {
            Situation(Rule('B', 'B'), 0, 0),
            Situation(Rule('S', 'BS'), 1, 1),
            Situation(Rule('A', 'bcBCc'), 3, 1),
            Situation(Rule('S', 'BS'), 0, 1),
            Situation(Rule('S', 'SA'), 0, 1),
            Situation(Rule('B', 'ccbB'), 0, 1),
            Situation(Rule('B', 'B'), 0, 1),
            Situation(Rule('C', ''), 0, 1)
        },
        1
    ),

    (
        Grammar(terminals='xy', non_terminals='SXYZ', start='S', rules=[
            Rule('S', 'X'),
            Rule('X', 'YxY'),
            Rule('Y', 'ZyZ'),
            Rule('Z', '')
        ]),
        2,
        [
            set(), set(),
            {
                Situation(Rule('S', 'X'), 0, 1)
            }
        ],
        {
            Situation(Rule('S', 'X'), 0, 1),
            Situation(Rule('X', 'YxY'), 0, 2),
            Situation(Rule('Y', 'ZyZ'), 0, 2),
            Situation(Rule('Z', ''), 0, 2)
        },
        10
    )
])
def test_Predict(grammar: Grammar, j: int, situations: tp.List[tp.Set[Situation]], expected: tp.Set[Situation],
                 times: int):
    algo = Algo.fit(grammar)

    previous_result = situations[j]
    for i in range(times):
        result = algo.Predict(previous_result,  j)
        situations[j] |= result
        previous_result = result

    assert situations[j] == expected


@pytest.mark.parametrize('grammar, j, situations, expected', [
    (
        Grammar(terminals='abc', non_terminals='SXY', start='S'),
        4,
        [
            set(),
            {
                Situation(Rule('X', 'XYabX'), 1, 1)
            },
            {
                Situation(Rule('S', 'abYXc'), 3, 0)
            },
            {
                Situation(Rule('Y', 'asSaX'), 2, 0)
            },
            {
                Situation(Rule('X', 'XYXY'), 4, 2),
                Situation(Rule('X', 'aabb'), 2, 2),
                Situation(Rule('Y', 'XSxYSS'), 6, 1),
                Situation(Rule('S', 'abYXc'), 4, 3)
            }
        ],
        {
            Situation(Rule('X', 'XYXY'), 4, 2),
            Situation(Rule('X', 'aabb'), 2, 2),
            Situation(Rule('Y', 'XSxYSS'), 6, 1),
            Situation(Rule('S', 'abYXc'), 4, 3),
            Situation(Rule('S', 'abYXc'), 4, 0),
            Situation(Rule('X', 'XYabX'), 2, 1)
        }
    ),
    (
        Grammar(terminals='abc', non_terminals='SFD', start='S'),
        3,
        [
            set(),
            {
                Situation(Rule('S', 'DFS'), 1, 1)
            },
            {
                Situation(Rule('D', 'xxDxyS'), 2, 0)
            },
            {
                Situation(Rule('F', ''), 0, 1),
                Situation(Rule('D', 'D'), 0, 2)
            }
        ],
        {
            Situation(Rule('F', ''), 0, 1),
            Situation(Rule('D', 'D'), 0, 2),
            Situation(Rule('S', 'DFS'), 2, 1)
        }
    )
])
def test_Complete(grammar: Grammar, j: int, situations: tp.List[tp.Set[Situation]], expected: tp.Set[Situation]):
    algo = Algo.fit(grammar)
    situations[j] |= algo.Complete(situations, situations[j])

    assert situations[j] == expected


@pytest.mark.parametrize('grammar, words', [
    (
        Grammar(terminals='abc', non_terminals='STU', start='S', rules=[
            Rule('S', 'SSc'),
            Rule('S', 'Tc'),
            Rule('S', 'acS'),
            Rule('S', 'bU'),
            Rule('T', 'Ub'),
            Rule('U', 'a')
        ]),
        [
            ('abc', True),
            ('ba', True),
            ('acabcbac', True),
            ('baabc', False),
            ('acbabacabcacabccc', True),
            ('acbcbacabcacabccc', False),
            ('', False)
        ]
    ),
    (
        Grammar(terminals='ab', non_terminals='S', start='S', rules=[
            Rule('S', 'aS'),
            Rule('S', 'Sb'),
            Rule('S', '')
        ]),
        [
            ('aaaaabbbbbbbbbb', True),
            ('bbbbb', True),
            ('aaaaaaaa', True),
            ('ab', True),
            ('', True),
            ('aaaaaaaabbbbbbbbbabbbb', False),
            ('babbbb', False),
            ('bbbbbbbbaaaa', False),
            ('aaaaaaaabaaaabbbbbbbbbbbbbbbbb', False)
        ]
    ),
    (
        Grammar(terminals='0123456789+-*/()', non_terminals='FSND', start='F', rules=[
            Rule('F', 'FSF'),
            Rule('F', 'N'),
            Rule('F', '(F)'),
            Rule('S', '+'),
            Rule('S', '-'),
            Rule('S', '*'),
            Rule('S', '/'),
            Rule('N', 'D'),
            Rule('N', 'ND'),
            Rule('D', '0'),
            Rule('D', '1'),
            Rule('D', '2'),
            Rule('D', '3'),
            Rule('D', '4'),
            Rule('D', '5'),
            Rule('D', '6'),
            Rule('D', '7'),
            Rule('D', '8'),
            Rule('D', '9')
        ]),
        [
            ('54', True),
            ('58+218/47', True),
            ('44*(15-218)+((42*(2+4)/3)-(2+2))', True),
            ('+43-12', False),
            ('(24+17*(47-21/544*(22+1)-7)+3', False),
            ('1312/76*(24/3+17(22+4))', False)
        ]
    )
])
def test_early(grammar: Grammar, words: tp.List[tp.Tuple[str, bool]]):
    algo = Algo.fit(grammar)
    for word, expected in words:
        assert algo.predict(word) == expected
