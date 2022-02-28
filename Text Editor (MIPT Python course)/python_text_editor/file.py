class FileLine:
    def __init__(self, contents):
        self.contents = list(contents)

    def __iter__(self):
        return iter(self.contents)

    def __str__(self) -> str:
        return ''.join(self.contents)

    def __len__(self):
        return len(self.contents)

    def __getitem__(self, item: int) -> str:
        return self.contents[item]

    def __setitem__(self, key: int, value: str):
        self.contents[key] = value

    def __delitem__(self, key: int):
        del self.contents[key]

    def add(self, text: str):
        self.contents += list(text)

    def insert(self, index: int, text: str):
        self.contents = self.contents[:index] + list(text) + self.contents[index:]


class File:
    def __init__(self, filename: str):
        self.filename = filename
        self.contents = []
        self.read()

    def __iter__(self):
        return iter(self.contents)

    def __getitem__(self, item: int) -> FileLine:
        return self.contents[item]

    def __setitem__(self, key: int, value):
        self.contents[key] = FileLine(value)

    def __delitem__(self, key: int):
        del self.contents[key]

    def __len__(self):
        return len(self.contents)

    def add(self, new_line):
        self.contents.append(FileLine(new_line))

    def insert(self, index: int, new_line):
        self.contents.insert(index, FileLine(new_line))

    def read(self):
        try:
            with open(self.filename, 'r') as file:
                has_newline_at_the_end = False

                for line in file:
                    has_newline_at_the_end = (line[-1] == '\n')
                    self.add(line.rstrip('\n'))
                if has_newline_at_the_end or len(self) == 0:
                    self.add('')
        except FileNotFoundError:
            self.add('')

    def write(self):
        with open(self.filename, 'w') as file:
            file.write('\n'.join(str(line) for line in self))
