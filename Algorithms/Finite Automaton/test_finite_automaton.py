import pytest
from copy import copy
from finite_automaton import FiniteAutomaton


@pytest.mark.parametrize("regexp, tests",
                         (
                                 ('a(bc)*+b*+c',
                                  (
                                          ('abcbcbcbc', True),
                                          ('a', True),
                                          ('bbbbbbbbbbbbb', True),
                                          ('bc', False),
                                          ('c', True),
                                          ('cc', False),
                                          ('ab', False)
                                  ),
                                  ),

                                 ('(ab)*a*+((a+b)(a+b))*',
                                  (
                                          ('abababab', True),
                                          ('aaaaa', True),
                                          ('bbbbb', False),
                                          ('bbbbbb', True),
                                          ('abaabab', False),
                                          ('abbbabaaba', True),
                                          ('abababaaaa', True),
                                          ('', True),
                                  ),
                                  ),
                         )
                         )
def test_regexp_to_automaton(regexp, tests):
    automaton = FiniteAutomaton.from_regexp(regexp)
    for case in tests:
        word, result = case
        assert (automaton.check(word) == result)


@pytest.mark.parametrize("source_automaton, tests", [
    (
            {"alphabet": "abcd",
             "states": [
                 {  # 0
                    "start": True,
                     "terminal": False,
                     "transitions": [
                         {
                             "target": 1,
                             "string": "abcd"
                         },
                         {
                             "target" : 3,
                             "string": "aaa"
                         },
                     ]
                 },

                 {   # 1
                     "start": False,
                     "terminal": False,
                     "transitions": [
                         {
                             "target": 1,
                             "string": "bcd"
                         },

                         {
                             "target": 2,
                             "string": "dab"
                         }
                     ]
                 },

                 {   # 2
                     "start": False,
                     "terminal": True,
                     "transitions": [
                         {
                             "target": 1,
                             "string": ""
                         }
                     ]
                 },

                 {   # 3
                     "start": False,
                     "terminal": True,
                     "transitions": [
                         {
                             "target": 2,
                             "string": "bcb"
                         },

                         {
                             "target": 3,
                             "string": ""
                         },

                         {
                             "target": 3,
                             "string": "bbbbb"
                         }
                     ]
                 }
             ]
             },
            (
                ("abcd", False),
                ("abcdbcddab", True),
                ("abcdbcddabbcdbcddab", True),
                ("aaa", True),
                ("aaabcbbcdbcddab", True),
                ("aaaaaa", False),
                ("aabbbbb", False),
                ("aaabbbbbbcbdabbcdbcddabbcddab", True),
            )
    ),

    (
            {
                "alphabet": "ab",
                "states": [
                    {   # 0
                        "start": True,
                        "terminal": True,
                        "transitions": [
                            {
                                "target": 0,
                                "string": ""
                            },

                            {
                                "target": 0,
                                "string": ""
                            },

                            {
                                "target": 0,
                                "string": ""
                            },

                            {
                                "target": 1,
                                "string": "abbaabb"
                            }
                        ]
                    },

                    {   # 1
                        "start": False,
                        "terminal": False,
                        "transitions": [
                            {
                                "target": 2,
                                "string": ""
                            }
                        ]
                    },

                    {   # 2
                        "start": False,
                        "terminal": False,
                        "transitions": [
                            {
                                "target": 0,
                                "string": ""
                            }
                        ]
                    }
                ]
            },

            (
                ("", True),
                ("a", False),
                ("abb", False),
                ("abbaabb", True),
                ("abbaabbabbaabb", True),
                ("abbabb", False)
            )
    )
])
def test_one_letter_transitions(source_automaton, tests):
    automaton = FiniteAutomaton.from_dict(source_automaton)
    automaton.one_letter_transitions()

    for test in tests:
        word, result = test
        assert(automaton.check(word) == result)

    for state in automaton.states:
        for tr in state.transitions:
            assert(len(tr.string) == 1)


@pytest.mark.parametrize("source_automaton, tests", [
        (
                {"alphabet": "abc",
                 "states": [
                     {   # 0
                         "start": True,
                         "terminal": False,
                         "transitions": [
                             {
                                 "target": 1,
                                 "string": ""
                             },

                             {
                                 "target": 3,
                                 "string": ""
                             }
                         ]
                     },

                     {   # 1
                         "start": False,
                         "terminal": False,
                         "transitions": [
                             {
                                 "target": 2,
                                 "string": 'a'
                             },

                             {
                                 "target": 3,
                                 "string": 'a'
                             }
                         ]
                     },

                     {   # 2
                         "start": False,
                         "terminal": True,
                         "transitions": [
                             {
                                 "target": 2,
                                 "string": "cc"
                             }
                         ]
                     },

                     {   # 3
                         "start": False,
                         "terminal": True,
                         "transitions": [
                             {
                                 "target": 3,
                                 "string": "ba"
                             }
                         ]
                     }
                 ]
                 },
                (
                    ("a", True),
                    ("acc", True),
                    ("ac", False),
                    ("ab", False),
                    ("aba", True),
                    ("aa", False),
                    ("", True),
                    ("abababa", True),
                    ("acccccc", True),
                    ("abacc", False),
                    ("acca", False),
                    ("abaa", False)
                )
        )
])
def test_determinization(source_automaton, tests):
    automaton = FiniteAutomaton.from_dict(source_automaton)
    automaton.determinize_minimal()

    for test in tests:
        word, result = test
        assert(automaton.check(word) == result)


@pytest.mark.parametrize("source_automaton, tests", [
    (
            {
                "alphabet": "abc",
                "states": [
                    {   # 0
                        "start": True,
                        "terminal": True,
                        "transitions": [
                            {
                                "target": 1,
                                "string": "a"
                            },

                            {
                                "target": 4,
                                "string": "b"
                            },

                            {
                                "target": 5,
                                "string": "c"
                            }
                        ]
                    },

                    {   # 1
                        "start": False,
                        "terminal": True,
                        "transitions": [
                            {
                                "target": 2,
                                "string": "b"
                            }
                        ]
                    },

                    {   # 2
                        "start": False,
                        "terminal": False,
                        "transitions": [
                            {
                                "target": 3,
                                "string": "c"
                            }
                        ]
                    },

                    {   # 3
                        "start": False,
                        "terminal": True,
                        "transitions": [
                            {
                                "target": 2,
                                "string": "b"
                            }
                        ]
                    },

                    {   # 4
                        "start": False,
                        "terminal": True,
                        "transitions": [
                            {
                                "target": 4,
                                "string": "b"
                            }
                        ]
                    },

                    {
                        "start": False,
                        "terminal": True,
                        "transitions": []
                    }
                ]
            },

            (
                "a",
                "ab",
                "abc",
                "abcb",
                "abcbcbcbc",
                "b",
                "bbbbbbbbbb",
                "ca",
                "cb",
                "abbbb",
                "c"
            )
    )
])
def test_automaton_to_regexp(source_automaton, tests):
    source = FiniteAutomaton.from_dict(source_automaton)
    from_regexp = FiniteAutomaton.from_regexp(source.to_regexp())

    for test in tests:
        assert(source.check(test) == from_regexp.check(test))


@pytest.mark.parametrize("source_regexp, alphabet, tests", [
    (
        "0",
        "abc",
        (
                ("a", True),
                ("b", True),
                ("c", True),
                ("abcab", True),
                ("cabcacbca", True),
                ("", True)
        )
    ),
    (
        "(a+b+c)*",
        "abc",
        (
                ("a", False),
                ("b", False),
                ("c", False),
                ("bacaca", False),
                ("ccccaaaab", False),
                ("", False)
        )
    ),
    (
        "1",
        "abc",
        (
                ("a", True),
                ("b", True),
                ("c", True),
                ("abcbab", True),
                ("bbcbabaaaa", True),
                ("", False)
        )
    ),
    (
        "(a+b)*ab",
        "ab",
        (
                ("ababbabbab", False),
                ("babaabaababb", True),
                ("", True),
                ("a", True),
                ("b", True),
                ("ab", False),
                ("ba", True)
        )
    )
])
def test_complement(source_regexp, alphabet, tests):
    automaton = FiniteAutomaton.from_regexp(source_regexp)
    automaton.complement()
    for test in tests:
        word, result = test
        assert(automaton.check(word) == result)
