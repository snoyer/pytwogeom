
import math

import pytwogeom

from asywriter import AsyWriter
import asyconverters

def spiral_pts(n, r0, r1, turns=2) :
    for i in range(n) :
        t = float(i)/(n-1)
        a = t * (turns*2*math.pi)
        r = r0 + t*(r1-r0)
        yield r*math.cos(a), r*math.sin(a)


pline = list(spiral_pts(64, 1, 3, 4))
path = pytwogeom.fit_path(pline, .01**2)

asy = AsyWriter(flip_y=True, size=(500,0), imports=['PathHandles'])

asy.draw(pline, 'grey+dashed')
asy.dot(pline, 'grey')

var = asy.declare_variable(path)
asy.draw(var, 'black')
asy.asy.call('draw_path_handles', var, 'black', .025)

asy.compile('generated/fit-path.pdf', include='.')
