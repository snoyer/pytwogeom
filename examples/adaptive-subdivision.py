import pytwogeom

from asywriter import AsyWriter
import asyconverters



path = pytwogeom.parse_svg_path('m 115.66247,95.747743 c 25.25381,-30.052038 31.8198,-35.355339 49.24493,-24.243661 17.42513,11.111678 39.39595,19.950513 46.9721,1.515229 7.57614,-18.435284 -5.80838,-72.47844523 -37.62819,-53.790623 -31.8198,18.687822 -54.2957,34.850262 -8.33375,34.345186 45.96194,-0.505076 24.4962,-17.172593 24.4962,-17.172593')

pline = pytwogeom.adaptive_subdivision(path, .1**2)

asy = AsyWriter(flip_y=True, imports=['PathHandles'])


var = asy.declare_variable(path)
asy.draw(var, 'grey')
asy.asy.call('draw_path_handles',var, 'grey+dashed', .66)

# asy.draw(pline)
asy.dot(pline)


asy.compile('generated/adaptive-subdivision.pdf', include='.')
