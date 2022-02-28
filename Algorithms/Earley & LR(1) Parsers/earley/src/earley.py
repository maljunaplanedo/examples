import typing as tp

from grammar import Grammar, Rule


class Situation:
    def __init__(self, rule: Rule, dot: int, i: int):
        self.rule = rule
        self.dot = dot
        self.i = i

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
        return hash((self.rule, self.dot, self.i))

    def __eq__(self, other) -> bool:
        if isinstance(other, Situation):
            return (self.rule, self.dot, self.i) == (other.rule, other.dot, other.i)
        raise NotImplementedError

    def __ne__(self, other) -> bool:
        return not self == other


class Algo:
    def __init__(self, grammar: Grammar):
        self.grammar = grammar
        self.grammar.non_terminals.append('@')
        self.grammar.add_rule(Rule('@', self.grammar.start))

    @staticmethod
    def fit(grammar: Grammar):
        return Algo(grammar)

    def predict(self, word: str) -> bool:
        situations = [set() for i in range(len(word) + 1)]
        situations[0].add(Situation(Rule('@', self.grammar.start), 0, 0))

        for j in range(len(word) + 1):
            self.Scan(situations, word, j)
            previous_result = situations[j]

            while True:
                old_length = len(situations[j])

                result = self.Complete(situations, previous_result)
                result |= self.Predict(previous_result, j)

                situations[j] |= result
                previous_result = result

                if old_length == len(situations[j]):
                    break

        return Situation(Rule('@', self.grammar.start), 1, 0) in situations[len(word)]

    def Scan(self, situations: tp.List[tp.Set[Situation]], word: str, j: int) -> None:
        if j == 0:
            return
        new = set()
        next_symbol = word[j - 1]
        for situation in situations[j - 1]:
            if situation.after_dot == next_symbol:
                new.add(Situation(situation.rule, situation.dot + 1, situation.i))
        situations[j] |= new

    def Predict(self, situations: tp.Set[Situation], j: int) -> tp.Set[Situation]:
        new = set()
        for situation in situations:
            if situation.after_dot not in self.grammar.non_terminals:
                continue
            for rule in self.grammar.rules:
                if rule.left == situation.after_dot:
                    new.add(Situation(rule, 0, j))
        return new

    def Complete(self, all_situations: tp.List[tp.Set[Situation]],
                 processed_situations: tp.Set[Situation]) -> tp.Set[Situation]:
        new = set()
        for lower in processed_situations:
            if lower.after_dot is not None:
                continue
            i = lower.i
            for upper in all_situations[i]:
                if upper.after_dot == lower.rule.left:
                    new.add(Situation(upper.rule, upper.dot + 1, upper.i))
        return new
