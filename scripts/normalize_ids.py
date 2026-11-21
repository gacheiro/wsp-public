"""Normalizes the ids in the edges file.
   Making the id match the line number.
"""

def correct_ids(filename):
    output_file = open(filename + "output.txt", "w")
    with open(filename, "r") as f:
        n = f.readline()
        output_file.write(f'{n}')
        for id, line in enumerate(f, 1):
            _, i, j, d, u, r = line.split()
            # discard id
            output_file.write(f'{id} {i} {j} {d} {u} {r}\n')

if __name__ == '__main__':
    import sys
    correct_ids(sys.argv[1])
