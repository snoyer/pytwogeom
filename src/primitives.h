
#include <boost/python.hpp>

#include <2geom/sbasis.h>
#include <2geom/piecewise.h>
#include <2geom/bezier.h>
#include <2geom/d2.h>
#include <2geom/sbasis-to-bezier.h>
#include <2geom/sbasis-geometric.h>
#include <2geom/affine.h>

#include "helpers.h"

typedef Geom::Point Point;
typedef Geom::Bezier Bezier;
typedef Geom::SBasis SBasis;
// typedef Geom::Piecewise Piecewise;
typedef Geom::D2<Geom::SBasis> D2SBasis;
typedef Geom::D2<Geom::Bezier> D2Bezier;
typedef Geom::Piecewise<Geom::SBasis> PiecewiseSBasis;
typedef Geom::Piecewise<Geom::D2<Geom::SBasis> > PiecewiseD2SBasis;


boost::python::class_<Point> class_Point(){
    using namespace boost::python;
    return class_<Point>("Point")
        .def(init<double, double>())
        .def("__getitem__", python_getitem<Point, double, 2>)

        .def("normalized", &Point::normalized)
        .def("length", &Point::length)

        .def(-self)
        .def(self * double()) .def(self *= double())
        .def(self / double()) .def(self /= double())
        .def(self + self) .def(self += self)
        .def(self - self) .def(self -= self)
    ;
}


boost::python::class_<Geom::Interval> class_Interval(){
    using namespace boost::python;
    return class_<Geom::Interval>("Interval")
        .def(init<>())
        .def("min", &Geom::Interval::min)
        .def("max", &Geom::Interval::max)
        .def("extent", &Geom::Interval::extent)
        .def("middle", &Geom::Interval::middle)
    ;
}

boost::python::class_<Geom::Rect> class_Rect(){
    using namespace boost::python;
    return class_<Geom::Rect>("Rect")
        .def(init<>())
        .def("min", &Geom::Rect::min)
        .def("max", &Geom::Rect::max)
        .def("width", &Geom::Rect::width)
        .def("height", &Geom::Rect::height)
    ;
}


boost::python::class_<SBasis> class_SBasis(){
    using namespace boost::python;
    return class_<SBasis>("SBasis")
        .def(init<>())
        .def(init<double>())
        .def(init<double,double>())
        .def(init<double,double,double,double>())
        .def(init<double,double,double,double,double,double>())

        .def("__call__", &SBasis::valueAt)
    ;
}


template <typename T, typename V, typename OptI>
boost::python::class_<Geom::Piecewise<T> > templateclass_Piecewise(char const* class_name){
    typedef Geom::Piecewise<T> PW;
    using namespace boost::python;
    return class_<PW>(class_name)
        .def(init< >())
        .def(init<T>())
        .def(init<V>())

        .def("__getitem__", python_getitem<PW, T>)
        .def("__len__", &PW::size)
        .def("__call__", &PW::valueAt)

        .def("value_at", &PW::valueAt)
        .def("first_value", &PW::firstValue)
        .def("last_value", &PW::lastValue)

        .def("domain", &PW::domain)
        .def("bounds_fast" , (OptI (*)(PW const &)) &Geom::bounds_fast , return_value_policy<return_Opt>())
        .def("bounds_exact", (OptI (*)(PW const &)) &Geom::bounds_exact, return_value_policy<return_Opt>())

        .def("concat", &PW::concat)
        .def("continuous_concat", &PW::continuousConcat)

        .def("push_seg", &PW::push_seg)
        .def("push", &PW::push)

        .def("portion", (PW (*)(PW const &, double, double)) &Geom::portion)
        .def("reversed", (PW (*)(PW const &)) &Geom::reverse)

        .def(-self)
        .def(self * double()) .def(self *= double())
        .def(self / double()) .def(self /= double())
        .def(self + V()) .def(self += V())
        .def(self - V()) .def(self -= V())
        .def(self + self) .def(self += self)
        .def(self - self) .def(self -= self)
    ;
}


template<typename T, int D>
T d2_dim(Geom::D2<T> const& d2){
    return d2[D];
}

template <typename T>
boost::python::class_<Geom::D2<T> > templateclass_D2(char const* class_name){
    typedef Geom::D2<T> D2T;
    using namespace boost::python;
    return class_<D2T>(class_name)
        .def(init<T,T>())

        .add_property("X", &d2_dim<T, 0>)
        .add_property("Y", &d2_dim<T, 1>)

        .def("__call__", &D2T::valueAt)

        .def("at0", &D2T::at0)
        .def("at1", &D2T::at1)

        .def("to_sbasis", &D2T::toSBasis)
    ;
}

template<typename D2T>
int d2_size(D2T const& t){
    return t[0].size();
}

D2Bezier d2sbasis_to_d2bezier(D2SBasis const& sb, const size_t sz=0) {
    D2Bezier res;
    Geom::sbasis_to_bezier(res, sb, sz);
    return res;
}

Geom::Point d2bezier_getitem(Geom::D2<Bezier> const& d2, int index){
    return Geom::Point(python_getitem<Bezier, double>(d2[0], index),
                       python_getitem<Bezier, double>(d2[1], index));
}





//
// D2Bezier points_to_d2bezier4(const Point& a, const Point& b, const Point& c, const Point& d){
//     return D2Bezier(Geom::Bezier(a[0], b[0], c[0], d[0]),
//                     Geom::Bezier(a[1], b[1], c[1], d[1]));
// }
//
// D2Bezier points_to_d2bezier3(const Point& a, const Point& b, const Point& c){
//     return D2Bezier(Geom::Bezier(a[0], b[0], c[0]),
//                     Geom::Bezier(a[1], b[1], c[1]));
// }
//
// D2Bezier points_to_d2bezier2(const Point& a, const Point& b){
//     return D2Bezier(Geom::Bezier(a[0], b[0]),
//                     Geom::Bezier(a[1], b[1]));
// }

void defs_primitives() {

    using namespace boost::python;

    class_Point();

    class_Interval();

    class_Rect();

    class_SBasis();

    templateclass_D2<SBasis>("D2SBasis")
        .def("__len__", d2_size<D2SBasis>)

        .def("to_bezier", d2sbasis_to_d2bezier,
            (arg("self"), arg("n")=0))

        .def(self * Geom::Affine())
    ;

    templateclass_D2<Bezier>("D2Bezier")
        .def("__len__", d2_size<D2Bezier>)
        .def("__getitem__", d2bezier_getitem)

        .def("point", &D2Bezier::point)
    ;

    templateclass_Piecewise<SBasis, double, Geom::OptInterval>("PiecewiseSBasis")
    ;

    templateclass_Piecewise<D2SBasis, Point, Geom::OptRect>("PiecewiseD2SBasis")
        .def(self * Geom::Affine())
    ;

    // def("points_to_bezier", points_to_d2bezier4);
    // def("points_to_bezier", points_to_d2bezier3);
    // def("points_to_bezier", points_to_d2bezier2);



    def("derivative", (PiecewiseD2SBasis (*)(PiecewiseD2SBasis const &)) &Geom::derivative);
    def("rot90", (PiecewiseD2SBasis (*)(PiecewiseD2SBasis const &)) &Geom::rot90);
    def("unit_vector", (PiecewiseD2SBasis (*)(PiecewiseD2SBasis const &, double, unsigned int)) &Geom::unitVector,
        (arg("pwd2sb"), arg("tol")=.01, arg("order")=3));
    def("arc_length_sb", (PiecewiseSBasis(*)(PiecewiseD2SBasis const &, double))&Geom::arcLengthSb,
        (arg("pwd2sb"), arg("tol")=.01));

    def("curvature", (Geom::Piecewise<Geom::SBasis> (*)(PiecewiseD2SBasis const &, double)) &Geom::curvature,
        (arg("pwd2sb"), arg("tol")=.01));
    def("curvature", (Geom::Piecewise<Geom::SBasis> (*)(D2SBasis const &, double)) &Geom::curvature,
        (arg("d2sb"), arg("tol")=.01));

    def("compose", (Geom::SBasis(*)(const Geom::SBasis&, const Geom::SBasis&)) &Geom::compose);
    def("compose", (PiecewiseD2SBasis(*)(const PiecewiseD2SBasis&, const Geom::SBasis&)) &Geom::compose);
    def("compose", (PiecewiseD2SBasis(*)(const PiecewiseD2SBasis&, const Geom::Piecewise<Geom::SBasis>&)) &Geom::compose);

    def("multi_roots", (std::vector<std::vector<double> >(*)(const PiecewiseSBasis& f, const std::vector<double>& values)) &Geom::multi_roots);


    def("make_cuts_independent", &Geom::make_cuts_independent);

    def("arc_length_parametrization", (PiecewiseD2SBasis(*)(const PiecewiseD2SBasis&, unsigned int, double)) &Geom::arc_length_parametrization,
        (arg("pwd2sb"), arg("order")=3, arg("tol")=.01));
    def("arc_length_parametrization", (PiecewiseD2SBasis(*)(const D2SBasis&, unsigned int, double)) &Geom::arc_length_parametrization,
        (arg("d2sb"), arg("order")=3, arg("tol")=.01));

    def("length", (double(*)(const D2SBasis&, double)) &Geom::length,
        (arg("d2sb"), arg("tol")=.01));
    def("length", (double(*)(const PiecewiseD2SBasis&, double)) &Geom::length,
        (arg("pwd2sb"), arg("tol")=.01));
}
