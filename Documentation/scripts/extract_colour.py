# Python script to extract colours for the Display.cfg file
#  from colour_list.tex.

import re

x = re.compile(r'colorbox\[RGB\]\{([0-9,]+)\}')
n = 1
for line in open('colour_list.tex'):
    r = x.search(line)
    if r:
        print('\Css{\#colorbox%d{background-color:rgb(%s);}}' % (n, r.group(1)))
        n += 1

