import math

import pytwogeom as py2geom
from pytwogeom import PathVector, Path

from asywriter import AsyWriter
import asyconverters



def sq_dist((x1,y1),(x2,y2)) :
    dx, dy = x2-x1, y2-y1
    return dx*dx + dy*dy

def no_dups(pts, epsilon) :
    sq_epsilon = epsilon*epsilon
    pp = None
    for p in pts :
        if not pp or sq_dist(p,pp) > sq_epsilon :
            yield p
        pp = p


def hatches(pathvector, spacing, angle) :

    xform = py2geom.Affine() * py2geom.Rotate(angle)
    pv = pathvector * xform
    pws = [p.to_piecewise() for p in pv]
    bounds = pv.bounds_exact()

    xmin,ymin = bounds.min()
    xmax,ymax = bounds.max()

    def iter_xs() :
        x = xmin
        while x <= xmax :
            yield x
            x += spacing

    hatches_pv = py2geom.PathVector()

    for x in iter_xs() :
        line_pw = py2geom.PiecewiseD2SBasis(py2geom.D2SBasis(
            py2geom.SBasis(x, x),
            py2geom.SBasis(ymin, ymax)
        ))
        ps = []
        for pw in pws :
            ps0 = py2geom.find_intersection_points(line_pw, pw, 1e-12)
            ps0 = list(no_dups(sorted(ps0, key=lambda (x,y):y), 1e-12))
            if len(ps0)>1 :
                ps+=ps0

        ps.sort(key=lambda (x,y):y)

        for p1,p2 in zip(ps[0::2],ps[1::2]) :
            pw = py2geom.PiecewiseD2SBasis(py2geom.D2SBasis(
                py2geom.SBasis(p1[0], p2[0]),
                py2geom.SBasis(p1[1], p2[1])
            ))
            paths = py2geom.paths_from_piecewise(pw)
            hatches_pv.append(paths[0])

    return hatches_pv * xform.inverse()

def hatches1(pathvector, spacing, angle) :


    xform = py2geom.Affine() * py2geom.Rotate(angle)
    pathvector_closed = close_pathvector(pathvector) * xform


    d2pw = pathvector_closed.to_piecewise()
    pwx,pwy = py2geom.make_cuts_independent(d2pw)
    xbounds = pwx.bounds_fast()
    ybounds = pwy.bounds_fast()

    epsilon = 1e-6

    def iter_xs() :
        x = xbounds.min()
        while x <= xbounds.max() :
            yield x
            x += spacing

    hatches_pv = py2geom.PathVector()

    for x in iter_xs() :
        ts = py2geom.multi_roots(pwx, [x])[0]
        if len(ts) > 1000:
            ts = list(no_dups1(sorted(ts)))

        def iter_pairs() :
            ps = filter(lambda (px,py):abs(px-x)<spacing/2., (d2pw(t) for t in ts))
            ps = no_dups(sorted(ps, key=lambda (x,y):y))
            pp = None
            for i,p in enumerate(ps) :
                if i%2 == 1 :
                    yield pp,p
                pp = p

        for p1,p2 in iter_pairs() :
            pw = py2geom.PiecewiseD2SBasis(py2geom.D2SBasis(
                py2geom.SBasis(p1[0], p2[0]),
                py2geom.SBasis(p1[1], p2[1])
            ))
            paths = py2geom.paths_from_piecewise(pw)
            hatches_pv.append(paths[0])

    return hatches_pv * xform.inverse()

pathvector0 = py2geom.parse_svg_path(open('2geom.svgd').read())

asy = AsyWriter(flip_y=True)
asy.draw(pathvector0, 'lightgrey')

h1 = hatches(pathvector0, 1, math.radians(-30))
asy.draw(h1, 'black')
asy.dot(h1, 'black+1')

asy.compile('generated/hatches.pdf', include='.')
