from grammar import Grammar
from lr import Algo


def main():
    grammar = Grammar.from_input()
    algo = Algo.fit(grammar)

    while True:
        print("Enter the word, '$' to quit:")
        word = input()
        if word == '$':
            break
        print("YES" if algo.predict(word) else "NO")


if __name__ == '__main__':
    main()
