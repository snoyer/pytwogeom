
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <2geom/affine.h>
#include <2geom/transforms.h>


void defs_transforms() {
    using namespace boost::python;


    class_<Geom::Affine>("Affine")
        .def(init<>())
        .def(init<double,double,double,double,double,double>())
        .def(init<Geom::Rotate>())
        .def(init<Geom::Scale>())
        .def(init<Geom::Translate>())

        .def("inverse", &Geom::Affine::inverse)

        .def(self * self)
        .def(self * Geom::Rotate())
        .def(self * Geom::Scale())
        .def(self * Geom::Translate())
    ;

    class_<Geom::Rotate>("Rotate")
        .def(init<double>())
        .def(self == self)
        .def(self != self)
        .def("inverse", &Geom::Rotate::inverse)
        .def("from_degrees", &Geom::Rotate::from_degrees)
        .staticmethod("from_degrees")
        .def(Geom::Point() * self)
        .def(self * self)
        .def(Geom::Affine() * self)
    ;

    class_<Geom::Scale>("Scale")
        .def(init<double, double>())
        .def(self == self)
        .def(self != self)
        .def("inverse", &Geom::Scale::inverse)
        .def(Geom::Point() * self)
        .def(self * self)
        .def(self * Geom::Affine())
    ;

    class_<Geom::Translate>("Translate")
        .def(init<double, double>())
        .def(init<Geom::Point>())
        .def(self == self)
        .def(self != self)
        .def("inverse", &Geom::Translate::inverse)
        .def(Geom::Point() * self)
        .def(self * self)
        .def(self * other<Geom::Rotate>())
        .def(self * other<Geom::Scale>())
    ;

}
