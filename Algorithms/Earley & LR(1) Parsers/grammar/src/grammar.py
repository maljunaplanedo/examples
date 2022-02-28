import typing as tp


class GrammarException(Exception):
    pass


class Rule:
    def __init__(self, left: str, right: str):
        self.left = left
        self.right = right

    def __hash__(self):
        return hash((self.left, self.right))

    def __eq__(self, other):
        if isinstance(other, Rule):
            return (self.left, self.right) == (other.left, other.right)
        raise NotImplementedError

    def __ne__(self, other):
        return not self == other


class Grammar:
    def __init__(self, terminals: str = '', non_terminals: str = 'S', start: str = 'S',
                 rules: tp.List[Rule] = None):

        self.terminals = list(terminals)
        self.non_terminals = list(non_terminals)
        self.start = start or 'S'
        self.rules = rules or []

        for rule in self.rules:
            self.check_rule(rule)

        if self.start not in self.non_terminals:
            raise GrammarException("Start non-terminal not in alphabet")

    def check_rule(self, rule: Rule) -> None:
        if rule.left not in self.non_terminals:
            raise GrammarException("Grammar must be context-free")

    def add_rule(self, rule: Rule) -> None:
        self.check_rule(rule)
        self.rules.append(rule)

    @staticmethod
    def from_input():
        print("Describe the grammar.")
        print("Enter terminals alphabet (as one string):")
        terminals = input()
        print("Enter non-terminals alphabet (as one string):")
        non_terminals = input()
        print("Enter start non-terminal:")
        start = input()

        grammar = Grammar(terminals=terminals, non_terminals=non_terminals, start=start)
        print("Enter number of rules: ")
        n = int(input())

        for i in range(n):
            print("Enter a rule ([left part]->[right part]):")
            left, right = input().split('->')
            grammar.add_rule(Rule(left, right))

        return grammar
