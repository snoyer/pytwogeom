
#include <boost/python.hpp>

#include "primitives.h"
#include "path.h"
#include "transforms.h"
#include "algorithms.h"

using namespace boost::python;

#include <2geom/sbasis-geometric.h>


BOOST_PYTHON_MODULE(_pytwogeom)
{

    defs_primitives();
    defs_path();
    defs_transforms();

    defs_algorithms();


    Point_from_tuple();
    to_python_converter<Geom::D2<Geom::Piecewise<Geom::SBasis> >, D2PwSb_to_python>();

    #define VECTOR_SEQ_CONV(T) \
        custom_vector_from_seq<T>(); \
        boost::python::to_python_converter<std::vector<T>, custom_vector_to_list<T> >();
    VECTOR_SEQ_CONV(double);
    VECTOR_SEQ_CONV(std::vector<double>);
    VECTOR_SEQ_CONV(Geom::Point);
    #undef VECTOR_SEQ_CONV
}
