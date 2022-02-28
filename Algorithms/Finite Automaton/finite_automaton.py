import string

from collections import namedtuple
from copy import copy


class FiniteAutomaton:
    Transition = namedtuple("Transition", ["target", "string"])

    class State:
        def __init__(self):
            self.transitions = set()
            self.start = False
            self.terminal = False

    def __init__(self, alphabet=string.ascii_lowercase):
        self.states = []
        self.alphabet = alphabet

    @property
    def starts(self):
        return (state for state in self.states if state.start)

    @property
    def terminals(self):
        return (state for state in self.states if state.terminal)

    class BadRegExpException(Exception):
        pass

    class NonOneLetterTransitionException(Exception):
        pass

    def check(self, word):
        # only for one letter transitions!

        layer = {start for start in self.starts}
        for i in range(len(word)):
            new_layer = set()
            for st in layer:
                for tr in st.transitions:
                    if len(tr.string) != 1:
                        raise FiniteAutomaton.NonOneLetterTransitionException(tr.string)
                    if tr.string == word[i]:
                        new_layer.add(tr.target)
            layer = new_layer

        for st in layer:
            if st.terminal:
                return True
        return False

    def inverted(self):
        inverted = FiniteAutomaton(self.alphabet)
        inverted.states = [FiniteAutomaton.State() for i in range(len(self.states))]

        for idx, state in enumerate(self.states):
            inverted.states[idx].start = state.terminal
            inverted.states[idx].terminal = state.start
            for tr in state.transitions:
                from_ = self.states.index(tr.target)

                inverted.states[from_].transitions.add(
                    FiniteAutomaton.Transition(
                        target=inverted.states[idx],
                        string=tr.string
                    )
                )

        return inverted

    def _remove_cul_de_sacs(self, image):
        def dfs(v):
            visited[image.states.index(v)] = True
            for tr in v.transitions:
                if not visited[image.states.index(tr.target)]:
                    dfs(tr.target)

        visited = [False for i in range(len(image.states))]

        for start in image.starts:
            dfs(start)

        cul_de_sacs = [st for idx, st in enumerate(self.states) if not visited[idx]]
        for st in self.states:
            st.transitions = {tr for tr in st.transitions if tr.target not in cul_de_sacs}

        self.states = [st for st in self.states if st not in cul_de_sacs]

    def remove_odd(self):
        self._remove_cul_de_sacs(self)
        self._remove_cul_de_sacs(self.inverted())

    def remove_multiletter(self):
        new_states = []
        for state in self.states:
            new_transitions = set()
            for tr in state.transitions:
                if len(tr.string) < 2:
                    continue

                new_states.append(FiniteAutomaton.State())
                new_transitions.add(FiniteAutomaton.Transition(
                    target=new_states[-1],
                    string=tr.string[0]
                ))

                for i in range(len(tr.string) - 2):
                    new_states.append(FiniteAutomaton.State())
                    new_states[-2].transitions.add(FiniteAutomaton.Transition(
                        target=new_states[-1],
                        string=tr.string[i + 1]
                    ))

                new_states[-1].transitions.add(FiniteAutomaton.Transition(
                    target=tr.target,
                    string=tr.string[-1]
                ))

            state.transitions |= new_transitions

        for state in self.states:
            state.transitions = {tr for tr in state.transitions if len(tr.string) < 2}

        self.states += new_states

    def remove_eps(self):
        def dfs(v):
            reached_by_eps.add(v)
            for tr in v.transitions:
                if tr.string == '' and tr.target not in reached_by_eps:
                    dfs(tr.target)

        for st in self.states:
            reached_by_eps = set()
            dfs(st)

            reached_by_eps -= {st}

            for rbe in reached_by_eps:
                if rbe.terminal:
                    st.terminal = True
                for tr in rbe.transitions:
                    if tr.string:
                        st.transitions.add(FiniteAutomaton.Transition(
                            target=tr.target,
                            string=tr.string
                        ))

        for st in self.states:
            st.transitions = {tr for tr in st.transitions if tr.string}

        self.remove_odd()

    def one_letter_transitions(self):
        self.remove_multiletter()
        self.remove_eps()

    def complement(self):
        self.determinize_minimal()
        for st in self.states:
            st.terminal = not st.terminal
        self.ideal()

    @staticmethod
    def _from_regexp(exp, alphabet=string.ascii_lowercase):
        if not exp:
            raise FiniteAutomaton.BadRegExpException("[empty expression]")

        exp += '@'
        exp = '*'.join([token for token in exp.split('*') if token])
        exp = exp[:-1]
        print(exp)

        balance = 0
        indicies = []

        for i, c in enumerate(exp):
            if c == '(':
                balance += 1
            elif c == ')':
                balance -= 1
            if balance == 0:
                indicies.append(i)

        if len(exp) >= 2 and indicies[0] == len(exp) - 1:
            return FiniteAutomaton._from_regexp(exp[1:-1], alphabet)

        if indicies[0] == len(exp) - 2 and exp[-1] == '*':
            automaton = FiniteAutomaton._from_regexp(exp[:-1], alphabet)

            automaton.states.append(FiniteAutomaton.State())
            new_state = automaton.states[-1]

            for start in automaton.starts:
                new_state.transitions.add(
                    FiniteAutomaton.Transition(
                        target=start,
                        string=''
                    )
                )
                start.start = False

            for terminal in automaton.terminals:
                terminal.transitions.add(
                    FiniteAutomaton.Transition(
                        target=new_state,
                        string=''
                    )
                )
                terminal.terminal = False

            new_state.start = True
            new_state.terminal = True

            return automaton

        plus_index = None
        for i in indicies:
            if i + 1 < len(exp) and exp[i + 1] == '+':
                plus_index = i + 1
                break

        if plus_index is not None:
            left = FiniteAutomaton._from_regexp(exp[:plus_index], alphabet)
            right = FiniteAutomaton._from_regexp(exp[plus_index + 1:], alphabet)

            plus = FiniteAutomaton(alphabet)
            plus.states.append(FiniteAutomaton.State())
            new_state = plus.states[0]

            def add_starts(starts):
                for start in starts:
                    new_state.transitions.add(
                        FiniteAutomaton.Transition(
                            target=start,
                            string=''
                        )
                    )
                    start.start = False

            add_starts(left.starts)
            add_starts(right.starts)

            plus.states += left.states + right.states
            new_state.start = True

            return plus

        if len(exp) > 1:
            index = indicies[0]
            if exp[index + 1] == '*':
                index += 1

            left = FiniteAutomaton._from_regexp(exp[:index + 1], alphabet)
            right = FiniteAutomaton._from_regexp(exp[index + 1:], alphabet)

            for terminal in left.terminals:
                terminal.terminal = False
                for start in right.starts:
                    terminal.transitions.add(
                        FiniteAutomaton.Transition(
                            target=start,
                            string=''
                        )
                    )

            for start in right.starts:
                start.start = False

            concat = FiniteAutomaton(alphabet)
            concat.states = left.states + right.states

            return concat

        if exp in alphabet:
            automaton = FiniteAutomaton(alphabet)
            automaton.states.append(FiniteAutomaton.State())
            automaton.states.append(FiniteAutomaton.State())

            start = automaton.states[0]
            terminal = automaton.states[1]

            start.start = True
            terminal.terminal = True

            start.transitions.add(
                FiniteAutomaton.Transition(
                    target=terminal,
                    string=exp
                )
            )

            return automaton

        if exp == '0':
            return FiniteAutomaton(alphabet)

        if exp == '1':
            automaton = FiniteAutomaton(alphabet)
            automaton.states.append(FiniteAutomaton.State())

            automaton.states[0].start = True
            automaton.states[0].terminal = True

            return automaton

        raise FiniteAutomaton.BadRegExpException(exp)

    @staticmethod
    def from_regexp(exp, alphabet=string.ascii_lowercase):
        automaton = FiniteAutomaton._from_regexp(exp, alphabet)
        automaton.ideal()
        return automaton

    @staticmethod
    def _regexp_beautify(s, op):
        if not s or s == '1':
            if op == '+' or op == '*':
                return '1'
            else:
                return ''

        if len(s) == 1:
            return s

        if op != '+':
            plus_last = False
            balance = 0
            for idx, i in enumerate(s):
                if i == '(':
                    balance += 1
                if i == ')':
                    balance -= 1
                if balance == 0 and idx < len(s) - 1 and s[idx + 1] == '+':
                    plus_last = True
                    break

            if plus_last:
                return '(' + s + ')'

        if op == '*':
            return '(' + s + ')'

        return s

    def _general_remove_duplicate_transitions(self):
        flag = False
        for st in self.states:
            used = set()
            to = None

            for tr in st.transitions:
                if tr.target in used:
                    to = tr.target
                    break
                used.add(tr.target)

            if to is None:
                continue

            flag = True

            all_strings = {tr.string for tr in st.transitions if tr.target == to}
            all_strings = {FiniteAutomaton._regexp_beautify(s, '+') for s in all_strings}
            st.transitions = {tr for tr in st.transitions if tr.target != to}

            new_string = '+'.join(all_strings)
            st.transitions.add(FiniteAutomaton.Transition(
                target=to,
                string=new_string
            ))

        return flag

    def _general_shrink_path(self):
        for st in self.states:
            if st.start or st.terminal:
                continue

            for in_ in self.states:
                if in_ == st:
                    continue

                new_transitions = set()

                for tr_in in in_.transitions:
                    if tr_in.target != st:
                        continue
                    for tr_out in st.transitions:
                        left_string = tr_in.string
                        right_string = tr_out.string

                        middle_string = ''

                        loop = None
                        for tr in st.transitions:
                            if tr.target == st:
                                loop = tr
                                break

                        if loop is not None:
                            middle_string = FiniteAutomaton._regexp_beautify(loop.string, '*')
                            if middle_string != '1':
                                middle_string += '*'
                            else:
                                middle_string = ''

                        if not left_string and not middle_string:
                            new_string = right_string
                        elif not right_string and not middle_string:
                            new_string = left_string
                        else:
                            left_string = FiniteAutomaton._regexp_beautify(left_string, 'c')
                            right_string = FiniteAutomaton._regexp_beautify(right_string, 'c')
                            new_string = left_string + middle_string + right_string

                        if not new_string:
                            new_string = '1'

                        new_transitions.add(FiniteAutomaton.Transition(
                            target=tr_out.target,
                            string=new_string
                        ))
                in_.transitions |= new_transitions

            for in_ in self.states:
                in_.transitions = {tr for tr in in_.transitions if tr.target != st}
            self.states.remove(st)

            return True

        return False

    def to_regexp(self):
        general = copy(self)
        general.ideal()

        new_start = FiniteAutomaton.State()
        for start in general.starts:
            new_start.transitions.add(FiniteAutomaton.Transition(
                target=start,
                string='1'
            ))
            start.start = False

        new_terminal = FiniteAutomaton.State()
        for terminal in general.terminals:
            terminal.transitions.add(FiniteAutomaton.Transition(
                target=new_terminal,
                string='1'
            ))
            terminal.terminal = False

        new_start.start = True
        new_terminal.terminal = True

        general.states.append(new_start)
        general.states.append(new_terminal)

        while True:
            flag1 = general._general_remove_duplicate_transitions()
            flag2 = general._general_shrink_path()
            if not flag1 and not flag2:
                break

        if not next(general.starts).transitions:
            return '0'

        return next(iter(next(general.starts).transitions)).string

    def determinize(self):
        self.one_letter_transitions()

        determined_states = [FiniteAutomaton.State()]
        determined_states[0].start = True

        for start in self.starts:
            if start.terminal:
                determined_states[0].terminal = True

        queue = [0]
        associations = [set(self.starts)]

        while queue:
            top = queue.pop(0)
            for char in self.alphabet:
                new_associations = set()

                terminal = False

                for state in associations[top]:
                    for tr in state.transitions:
                        if tr.string == char:
                            new_associations.add(tr.target)
                            if tr.target.terminal:
                                terminal = True

                if new_associations in associations:
                    index = associations.index(new_associations)
                else:
                    determined_states.append(FiniteAutomaton.State())
                    queue.append(len(associations))
                    associations.append(new_associations)
                    index = queue[-1]
                    determined_states[-1].terminal = terminal

                determined_states[top].transitions.add(
                    FiniteAutomaton.Transition(
                        target=determined_states[index],
                        string=char
                    )
                )

        self.states = determined_states

    def determinize_minimal(self):
        self.determinize()

        self.states.insert(0, FiniteAutomaton.State())

        for char in self.alphabet:
            self.states[0].transitions.add(FiniteAutomaton.Transition(
                target=self.states[0],
                string=char
            ))

        inverted = self.inverted()
        queue = []
        mark = [[False for j in range(len(self.states))] for i in range(len(self.states))]

        for idx1, state1 in enumerate(self.states):
            for idx2, state2 in enumerate(self.states):
                if state1.terminal != state2.terminal:
                    mark[idx1][idx2] = mark[idx2][idx1] = True
                    queue.append((idx1, idx2))

        while queue:
            u, v = queue.pop(0)
            for char in self.alphabet:
                for in_u in (tr.target for tr in inverted.states[u].transitions if tr.string == char):
                    for in_v in (tr.target for tr in inverted.states[v].transitions if tr.string == char):
                        in_u_idx = inverted.states.index(in_u)
                        in_v_idx = inverted.states.index(in_v)

                        if not mark[in_u_idx][in_v_idx]:
                            mark[in_u_idx][in_v_idx] = mark[in_v_idx][in_u_idx] = True
                            queue.append((in_u_idx, in_v_idx))

        component = [-1 for i in range(len(self.states))]

        for i in range(len(self.states)):
            if not mark[0][i]:
                component[i] = 0

        count = 0
        for i in range(1, len(self.states)):
            if component[i] == -1:
                count += 1
                component[i] = count
                for j in range(i + 1, len(self.states)):
                    if not mark[i][j]:
                        component[j] = count

        minimal_states = [FiniteAutomaton.State() for i in range(count + 1)]

        for idx, st in enumerate(self.states):
            if st.start:
                minimal_states[component[idx]].start = True
            if st.terminal:
                minimal_states[component[idx]].terminal = True
            for tr in st.transitions:
                minimal_states[component[idx]].transitions.add(
                    FiniteAutomaton.Transition(
                        target=minimal_states[component[self.states.index(tr.target)]],
                        string=tr.string
                    )
                )

        self.states = minimal_states

    def __copy__(self):
        res = FiniteAutomaton(self.alphabet)
        res.states = [FiniteAutomaton.State() for i in range(len(self.states))]

        for idx, st in enumerate(self.states):
            res.states[idx].start = st.start
            res.states[idx].terminal = st.terminal
            for tr in st.transitions:
                res.states[idx].transitions.add(FiniteAutomaton.Transition(
                    target=res.states[self.states.index(tr.target)],
                    string=tr.string
                ))

        return res

    @staticmethod
    def from_dict(image):
        if 'alphabet' in image:
            alphabet = image['alphabet']
        else:
            alphabet = string.ascii_lowercase

        automaton = FiniteAutomaton(alphabet)
        automaton.states = [FiniteAutomaton.State() for i in range(len(image['states']))]

        for state, image_state in zip(automaton.states, image['states']):
            state.start = image_state['start']
            state.terminal = image_state['terminal']

            for tr in image_state['transitions']:
                state.transitions.add(FiniteAutomaton.Transition(
                    target=automaton.states[tr['target']],
                    string=tr['string']
                ))

        return automaton

    def ideal(self):
        self.determinize_minimal()
        self.remove_odd()

    def to_dict(self):
        result = {'alphabet': self.alphabet, 'states': []}
        for state in self.states:
            result['states'].append({})
            result['states'][-1]['start'] = state.start
            result['states'][-1]['terminal'] = state.terminal

            transitions = []
            for tr in state.transitions:
                transitions.append({
                    'target': self.states.index(tr.target),
                    'string': tr.string
                })

            result['states'][-1]['transitions'] = transitions

        return result

    @staticmethod
    def from_input(alphabet=string.ascii_lowercase):
        automaton = FiniteAutomaton(alphabet)
        n = int(input("Enter states number: "))
        automaton.states = [FiniteAutomaton.State() for i in range(n)]

        n_transitions = int(input("Enter transitions number: "))

        print("Enter list of start states:")
        for st in map(int, input().split()):
            automaton.states[st].start = True

        print("Enter list of terminal states:")
        for st in map(int, input().split()):
            automaton.states[st].terminal = True

        print("Enter list of transitions ([from] [to] [letter (eps for empty)]):")

        for i in range(n_transitions):
            from_, to, letter = input().split()
            from_, to = map(int, (from_, to))

            letter = letter if letter != 'eps' else ''

            automaton.states[from_].transitions.add(
                FiniteAutomaton.Transition(
                    target=automaton.states[to],
                    string=letter
                )
            )

        return automaton

    def print(self):
        print("Number of states: ", len(self.states))
        print("All the states: ")

        for idx, st in enumerate(self.states):
            print("\n\n ------------STATE", idx, "-----------")
            if st.start:
                print("START")
            if st.terminal:
                print("TERMINAL")

            for tr in st.transitions:
                print(" --", tr.string, "-->", self.states.index(tr.target))
