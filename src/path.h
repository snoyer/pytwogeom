
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <2geom/path.h>
#include <2geom/pathvector.h>
#include <2geom/sbasis-to-bezier.h>
#include <2geom/path-sink.h>
#include <2geom/svg-path-parser.h>
#include <2geom/svg-path-writer.h>

#include "helpers.h"

// class PathSinkWrap: public Geom::PathSink, public boost::python::wrapper<Geom::PathSink> {
//     void moveTo(Geom::Point const &p) {this->get_override("moveTo")(p);}
//     void lineTo(Geom::Point const &p) {this->get_override("lineTo")(p);}
//     void curveTo(Geom::Point const &c0, Geom::Point const &c1, Geom::Point const &p) {this->get_override("curveTo")(c0, c1, p);}
//     void quadTo(Geom::Point const &c, Geom::Point const &p) {this->get_override("quadTo")(c, p);}
//     void arcTo(double rx, double ry, double angle, bool large_arc, bool sweep, Geom::Point const &p) {this->get_override("arcTo")(rx, ry, angle, large_arc, sweep, p);}
//     bool backspace() {return this->get_override("backspace")();}
//     void closePath() {this->get_override("closePath")();}
//     void flush() {this->get_override("flush")();}
// };
// void (Geom::PathSink::*feed_path)(Geom::Path const &) = &Geom::PathSink::feed;
// void (Geom::PathSink::*feed_pathvector)(Geom::PathVector const &) = &Geom::PathSink::feed;

Geom::Piecewise<Geom::D2<Geom::SBasis> > path_to_pw(const Geom::Path& p){
    return Geom::paths_to_pw(Geom::PathVector(p));
}

void path_append(Geom::Path& path, Geom::Path& other){
    path.append(other);
}

Geom::PathVector (*parse_svg_path_str) (char const *) = &Geom::parse_svg_path;

/* allow passing Path rather than PathVector to Geom::write_svg_path */
std::string path_write_svg_path(Geom::Path* path, int prec, bool optimize, bool shorthands) {
    return Geom::write_svg_path(Geom::PathVector(*path), prec, optimize, shorthands);
}


void defs_path() {
    using namespace boost::python;

    // class_<PathSinkWrap, boost::noncopyable>("PathSink")
    //     .def("moveTo", pure_virtual(&Geom::PathSink::moveTo))
    //     .def("lineTo", pure_virtual(&Geom::PathSink::lineTo))
    //     .def("curveTo", pure_virtual(&Geom::PathSink::curveTo))
    //     .def("quadTo", pure_virtual(&Geom::PathSink::quadTo))
    //     .def("arcTo", pure_virtual(&Geom::PathSink::arcTo))
    //     .def("backspace", pure_virtual(&Geom::PathSink::backspace))
    //     .def("closePath", pure_virtual(&Geom::PathSink::closePath))
    //     .def("flush", pure_virtual(&Geom::PathSink::flush))
    //     .def("feed", feed_path)
    //     .def("feed", feed_pathvector)
    // ;

    class_<Geom::Path>("Path")

        .def("initial_point", &Geom::Path::initialPoint)
        .def("final_point", &Geom::Path::finalPoint)

        .def("bounds_fast" , &Geom::Path::boundsFast , return_value_policy<return_Opt>())
        .def("bounds_exact", &Geom::Path::boundsExact, return_value_policy<return_Opt>())

        .def("close", &Geom::Path::close)
        .def("closed", &Geom::Path::closed)

        .def("reversed", &Geom::Path::reversed)

        .def("append", path_append)
        .def("set_stitching", &Geom::Path::setStitching)
        // .def("appendNew", &Geom::Path::appendNew)

        .def("winding", &Geom::Path::winding)

        .def(self * Geom::Affine())

        /* not in original class */
        .def("to_piecewise", path_to_pw) // -> Piecewise<D2<SBasis>>
    ;

    class_<Geom::PathVector>("PathVector")
        .def(init<Geom::Path>())
        .def(vector_indexing_suite<Geom::PathVector>())

        .def("bounds_fast" , &Geom::PathVector::boundsFast , return_value_policy<return_Opt>())
        .def("bounds_exact", &Geom::PathVector::boundsExact, return_value_policy<return_Opt>())

        .def("initial_point", &Geom::PathVector::initialPoint)
        .def("final_point", &Geom::PathVector::finalPoint)
        .def("reversed", &Geom::PathVector::reversed)
        .def("reverse", &Geom::PathVector::reverse)
        //
        // .def(self * Geom::Affine())
        // .def(self *= Geom::Affine())

        .def("winding", &Geom::PathVector::winding)

        .def(self * Geom::Affine())

        /* not in original class */
        .def("to_piecewise", Geom::paths_to_pw) // -> Piecewise<D2<SBasis>>
    ;


    def("paths_from_piecewise", Geom::path_from_piecewise,
        (arg("piecewise"), arg("tol")=1., arg("only_cubic")=true));

    def("path_from_sbasis", Geom::path_from_sbasis,
        (arg("d2sbasis"), arg("tol")=1., arg("only_cubic")=true));


    def("parse_svg_path", parse_svg_path_str);
    def("format_svg_path", Geom::write_svg_path,
        (arg("pathvector"), arg("prec")=-1, args("optimize")=false, arg("shorthands")=true));
    def("format_svg_path", path_write_svg_path,
        (arg("pathvector"), arg("prec")=-1, args("optimize")=false, arg("shorthands")=true));



    // def("to_piecewise", path_to_pw); // Path -> Piecewise<D2<SBasis>>
    // def("to_piecewise", Geom::paths_to_pw); // PathVector -> Piecewise<D2<SBasis>>

}
