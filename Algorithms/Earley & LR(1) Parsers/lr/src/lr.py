from grammar import Grammar, Rule
import typing as tp
from copy import copy


class State:
    def __init__(self, rule: Rule, dot: int, look: str):
        self.rule = rule
        self.dot = dot
        self.look = look

    @property
    def before_dot(self) -> tp.Optional[str]:
        if self.dot == 0:
            return None
        return self.rule.right[self.dot - 1]

    @property
    def after_dot(self) -> tp.Optional[str]:
        if self.dot == len(self.rule.right):
            return None
        return self.rule.right[self.dot]

    def __hash__(self) -> int:
        return hash((self.rule, self.dot, self.look))

    def __eq__(self, other) -> bool:
        if isinstance(other, State):
            return (self.rule, self.dot, self.look) == (other.rule, other.dot, other.look)
        raise NotImplementedError

    def __ne__(self, other) -> bool:
        return not self == other


class Cell:
    REDUCE = 0
    SHIFT = 1
    ACCEPT = 2
    ERROR = 3

    def __init__(self, command: int, how=None):
        self.command = command
        self.how = how


class Algo:
    def __init__(self, grammar: Grammar):
        self.grammar = grammar
        self.grammar.non_terminals.append('@')
        self.grammar.add_rule(Rule('@', self.grammar.start))
        self.grammar.terminals.append('$')

        self.has_eps = {symbol: False for symbol in self.grammar.non_terminals}
        self.calculate_has_eps()

        self.first_helper = {symbol: set() for symbol in self.grammar.non_terminals}
        self.calculate_first()

        self.control_table = []
        self.goto = []
        self.calculate_control_table()

    def calculate_has_eps(self) -> None:
        for rule in self.grammar.rules:
            if not rule.right:
                self.has_eps[rule.left] = True

        while True:
            changed = False
            for rule in self.grammar.rules:
                if self.has_eps[rule.left]:
                    continue
                good = True
                for symbol in rule.right:
                    if symbol in self.grammar.terminals or not self.has_eps[symbol]:
                        good = False
                        break
                if good:
                    changed = True
                    self.has_eps[rule.left] = True
            if not changed:
                break

    def calculate_first(self) -> None:
        while True:
            changed = False
            for rule in self.grammar.rules:
                old_length = len(self.first_helper[rule.left])
                for symbol in rule.right:
                    if symbol in self.grammar.terminals:
                        self.first_helper[rule.left].add(symbol)
                        break
                    self.first_helper[rule.left] |= self.first_helper[symbol]
                    if not self.has_eps[symbol]:
                        break

                if old_length != len(self.first_helper[rule.left]):
                    changed = True

            if not changed:
                break

    def first(self, word: str) -> tp.Set[str]:
        result = set()
        for symbol in word:
            if symbol in self.grammar.terminals:
                result.add(symbol)
                break
            result |= self.first_helper[symbol]
            if not self.has_eps[symbol]:
                break
        return result

    @staticmethod
    def fit(grammar: Grammar):
        return Algo(grammar)

    def closure_rule(self, state: State, rule: Rule) -> tp.Set[State]:
        if rule.left != state.after_dot:
            return set()
        result = set()
        for new_look in self.first(state.rule.right[state.dot + 1:] + state.look):
            result.add(State(rule, 0, new_look))
        return result

    def closure(self, set_of_states: tp.Set[State]) -> tp.Set[State]:
        result = copy(set_of_states)
        while True:
            new = set()
            for state in result:
                if state.after_dot not in self.grammar.non_terminals:
                    continue
                for rule in self.grammar.rules:
                    new |= self.closure_rule(state, rule)

            old_length = len(result)
            result |= new

            if old_length == len(result):
                break

        return result

    def goto_function(self, set_of_states: tp.Set[State], x: str) -> tp.Set[State]:
        result = set()
        for state in set_of_states:
            if state.after_dot != x:
                continue
            result.add(State(state.rule, state.dot + 1, state.look))
        return self.closure(result)

    def items(self) -> tp.List[tp.Set[State]]:
        result = [self.closure({State(Rule('@', self.grammar.start), 0, '$')})]
        while True:
            changed = False
            new = []
            for superstate in result:
                for symbol in self.grammar.terminals + self.grammar.non_terminals:
                    to = self.goto_function(superstate, symbol)
                    if to and to not in result and to not in new:
                        new.append(to)
                        changed = True
            result += new
            if not changed:
                break

        return result

    def calculate_control_table(self) -> None:
        states = self.items()
        n = len(states)
        self.control_table = [{symbol: Cell(Cell.ERROR) for symbol in self.grammar.terminals}
                              for i in range(n)]
        self.goto = [{symbol: -1 for symbol in self.grammar.non_terminals} for i in range(n)]

        for index, superstate in enumerate(states):
            for state in superstate:
                if state.after_dot in self.grammar.terminals:
                    try:
                        j = states.index(self.goto_function(superstate, state.after_dot))
                        self.control_table[index][state.after_dot] = Cell(Cell.SHIFT, j)
                    except ValueError:
                        pass
                if state.after_dot is None and state.rule.left != '@':
                    self.control_table[index][state.look] = Cell(Cell.REDUCE, state.rule)
                if state == State(Rule('@', self.grammar.start), 1, '$'):
                    self.control_table[index]['$'] = Cell(Cell.ACCEPT)
                if state.after_dot in self.grammar.non_terminals:
                    try:
                        j = states.index(self.goto_function(superstate, state.after_dot))
                        self.goto[index][state.after_dot] = j
                    except ValueError:
                        pass

    def predict(self, word: str) -> bool:
        word += '$'
        i = 0
        stack = [0]
        while i < len(word):
            cur = stack[-1]
            cell = self.control_table[cur][word[i]]
            if cell.command == Cell.SHIFT:
                stack.append(word[i])
                stack.append(cell.how)
                i += 1
            elif cell.command == Cell.REDUCE:
                for j in range(len(cell.how.right)):
                    stack.pop()
                    stack.pop()
                top = stack[-1]
                stack.append(cell.how.left)
                stack.append(self.goto[top][cell.how.left])
            elif cell.command == Cell.ACCEPT:
                return True
            elif cell.command == Cell.ERROR:
                return False
        return False
