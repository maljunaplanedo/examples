from finite_automaton import FiniteAutomaton

if __name__ == '__main__':
    a = FiniteAutomaton.from_regexp('(a(ab+b(ba)*a)*)*', alphabet='ab')
    a.complement()
    a.determinize_minimal()
    a.print()
