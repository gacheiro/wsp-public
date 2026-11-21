"""Script used to replace the road width of the instances from Sakuraba 2016."""


def gen_filename(dir=''):
    for i in range(11):
        for r in (40, 45):
            for b in (5, 10, 15, 20):
                yield f'{dir}/{i}n10r{r}B{b}'


def read(file):
    with open(file) as f:
        return f.readlines()


def write(file, str, mode='w'):
    with open(file, mode) as f:
        f.write(str)


def replace_width_inst(lines, value):
    replaced = []
    rpl_width = str(value)
    # ignore 1st line
    for line in lines[1:]:
        attrs = line.split()
        rpl_attrs = attrs[:-2] + [rpl_width] + attrs[-1:]
        print(' '.join(rpl_attrs))
        replaced.append(' '.join(rpl_attrs))

    return '\n'.join(replaced)


def replace_width_lp(lines, value):

    replaced = []
    for line in lines:
        if line.startswith('c11'):
            fst, _ = line.split('=')
            replaced.append(f'{fst}= {value}')
        else:
            replaced.append(line.strip())

    return '\n'.join(replaced)


def replace_all(replacer, dir, ext, width, rename=True):
    for file in gen_filename(dir):
        rplced = replacer(read(file + f'.{ext}'), width)
        if rename:
            write(f'{file}u{width}.{ext}', rplced)
        else:
            write(f'{file}.{ext}', rplced)
        break


def replace_all_insts(dir='', width=2, rename=True):
    replace_all(replace_width_inst, dir, 'txt', width, rename)


def replace_all_lps(dir='', width=2, rename=True):
    replace_all(replace_width_lp, dir, 'lp', width, rename)

