
#include <boost/python.hpp>

#include <2geom/sbasis.h>
#include <2geom/piecewise.h>
#include <2geom/d2.h>
#include <2geom/path.h>
#include <2geom/bezier-utils.h>
#include <2geom/basic-intersection.h>
#include <2geom/intersection-graph.h>

Geom::PathVector boolop_union(const Geom::PathVector& a, const Geom::PathVector& b){
    return Geom::PathIntersectionGraph(a, b).getUnion();
}

Geom::PathVector boolop_intersection(const Geom::PathVector& a, const Geom::PathVector& b){
    return Geom::PathIntersectionGraph(a, b).getIntersection();
}

Geom::PathVector boolop_minus(const Geom::PathVector& a, const Geom::PathVector& b){
    return Geom::PathIntersectionGraph(a, b).getAminusB();
}

Geom::PathVector boolop_xor(const Geom::PathVector& a, const Geom::PathVector& b){
    return Geom::PathIntersectionGraph(a, b).getXOR();
}


Geom::Path bezier_fit(const boost::python::list& point_list, const double tolerance_sq, const unsigned max_beziers){

    /* python list to point array */
    const boost::python::ssize_t points_len = boost::python::len(point_list);
    Geom::Point *points = new Geom::Point[points_len];
    for(boost::python::ssize_t i=0; i<points_len; ++i)
        points[i] = boost::python::extract<Geom::Point>(point_list[i]);

    /* allocate result array and do actual fitting */
    Geom::Point *result = new Geom::Point[max_beziers*4];
    const int n_segs = Geom::bezier_fit_cubic_r(result, points, points_len, tolerance_sq, max_beziers);

    /* convert result beziers to path */
    Geom::Path fit;
    if(n_segs > 0){
        fit.start(result[0]);
        for(int i=0; i<n_segs; ++i)
            fit.appendNew<Geom::CubicBezier>(result[4*i + 1], result[4*i + 2], result[4*i + 3]);
    }

    delete[] points;
    delete[] result;

    return fit;
}

using namespace Geom;



Piecewise<D2<SBasis> > path_along_path(
    const Piecewise<D2<SBasis> >& skeleton,
    const Piecewise<D2<SBasis> >& pattern,
    const double offset, const int copies
){
    const double tol = .01;

    Piecewise<D2<SBasis> > uskeleton = arc_length_parametrization(Piecewise<D2<SBasis> >(skeleton), 3, tol);
    Piecewise<D2<SBasis> > n = rot90(derivative(uskeleton));
    n = force_continuity(n);

    const D2<Piecewise<SBasis> > xxx = make_cuts_independent(pattern);
    const Piecewise<SBasis> x = xxx[0];
    const Piecewise<SBasis> y = xxx[1];

    const double pattWidth = bounds_exact(x)->extent();

    Piecewise<D2<SBasis> > output;

    double offs = offset;
    for (int i=0; i<copies; i++){
        output.concat(compose(uskeleton, x+offs) + y*compose(n, x+offs));
        offs += pattWidth;
    }

    return arc_length_parametrization(output);
}

Piecewise<D2<SBasis> > path_variable_offset(
    const Piecewise<D2<SBasis> >& path,
    const Piecewise<SBasis>& offset,
    const double tol
){
    const Piecewise<D2<SBasis> > upath = arc_length_parametrization(path, 3, tol);
    const Piecewise<D2<SBasis> > n = force_continuity(rot90(derivative(upath)));

    const SBasis x = SBasis(0.,1.) * upath.cuts.back();

    return compose(upath, x) + offset*compose(n, x);
}


std::vector<Geom::Point> find_intersection_points_pwd2sb(
    const Piecewise<D2<SBasis> >& pw1,
    const Piecewise<D2<SBasis> >& pw2,
    const double precision
){
    std::vector<Geom::Point> intersections;

    const unsigned n1 = pw1.size();
    const unsigned n2 = pw2.size();

    for(unsigned i1=0; i1<n1; ++i1){
        for(unsigned i2=0; i2<n2; ++i2){
            std::vector<std::pair<double, double> > xys;
            Geom::find_intersections(xys, pw1[i1], pw2[i2], precision);
            for(unsigned k=0; k<xys.size(); ++k)
                intersections.push_back(pw1[i1].valueAt(xys[k].first));
        }
    }

    return intersections;
}


int turn_direction(const Geom::Point& a, const Geom::Point& b, const Geom::Point& c){
    if(a == b || b == c || c == a) return 0;
    const Geom::Coord x = Geom::cross(b-a, c-a);
    if(x < 0) return +1;
    if(x > 0) return -1;
    return 0;
}

int path_direction(const Geom::Path& path){
    double area;
    Geom::Point centre;
    Geom::centroid(path.toPwSb(), centre, area);
    if(area > 0) return +1;
    if(area < 0) return -1;
    return 0;
}


void defs_algorithms() {
    using namespace boost::python;

    def("boolop_union", &boolop_union);
    def("boolop_intersection", &boolop_intersection);
    def("boolop_minus", &boolop_minus);
    def("boolop_xor", &boolop_xor);

    def("fit_path", &bezier_fit,
        (arg("points"), arg("tolerance_sq"), arg("max_beziers")=256));

    def("path_along_path", &path_along_path);

    def("path_variable_offset", &path_variable_offset,
        (arg("path"), arg("offset"), arg("tol")=.01));


    def("find_intersection_points", &find_intersection_points_pwd2sb,
        (arg("pwd2sb1"), arg("pwd2sb2"), arg("precision")=.00001));

    def("turn_direction", &turn_direction);
    def("path_direction", &path_direction);
}
