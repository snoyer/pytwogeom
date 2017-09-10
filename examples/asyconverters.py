
import pytwogeom

from asywriter import drawable_converter

def asy_pair(p) :
    return '(' + str(p[0]) + ',' + str(p[1]) + ')'


@drawable_converter(pytwogeom.Point)
def asy_2geomPoint(p) :
    return 'pair', asy_pair(p)


@drawable_converter(pytwogeom.PiecewiseD2SBasis)
def asy_2geomPiecewiseD2SBasis(pw) :
    return asy_2geomPathVector(pytwogeom.paths_from_piecewise(pw))

@drawable_converter(pytwogeom.Path)
def asy_2geomPath(p) :
    bezs = [sb.to_bezier(4) for sb in p.to_piecewise()]
    return 'path', '--'.join('%s..controls %s and %s..%s' % tuple(map(asy_pair, b)) for b in bezs)


@drawable_converter(pytwogeom.PathVector)
def asy_2geomPathVector(pv) :
    return 'path[]', '^^'.join(asy_2geomPath(p)[1] for p in pv)
