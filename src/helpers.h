
#ifndef _PYTWOGEOM_HELPERS_H_
#define _PYTWOGEOM_HELPERS_H_

#include <boost/python.hpp>
#include <boost/mpl/if.hpp>



/* https://stackoverflow.com/a/26644530 */
template <typename T> struct is_Opt : boost::false_type {};
template <> struct is_Opt<Geom::OptInterval> : boost::true_type {};
template <> struct is_Opt<Geom::OptRect>     : boost::true_type {};

template <typename T>
struct to_python_Opt {
    bool convertible() const { return is_Opt<T>::value; }

    PyObject* operator()(const T& obj) const {
        namespace py = boost::python;
        return py::incref((
            obj ? py::object(*obj) // defer to Boost.Python converter.
                : py::object()     // otherwise, return Python None.
        ).ptr());
    }

    /* used for documentation. */
    const PyTypeObject* get_pytype() const { return 0; }
};

template <typename> struct return_Opt_requires_a_Opt_return_type {};

struct return_Opt {
    template <class T> struct apply {
        /* the ResultConverter only checks if T is convertible at runtime
           this MPL branch causes a compile time error if T is not Opt... type
           by providing a type that is not a ResultConverter model. */
        typedef typename boost::mpl::if_<
            is_Opt<T>,
            to_python_Opt<T>,
            return_Opt_requires_a_Opt_return_type<T>
        >::type type;
    };
};





template <typename T, typename R, unsigned L>
R python_getitem(T const& p, int index){
    unsigned i = index;
    if (index < 0)
        i = index = L + index;

    if ((index < 0) || (i > (L - 1))) {
        PyErr_SetString(PyExc_IndexError, "index out of range");
        boost::python::throw_error_already_set();
    }
    return p[i];
}

template <typename T, typename R>
R python_getitem_size(T const& p, int index, const size_t L){
    unsigned i = index;
    if (index < 0)
        i = index = L + index;

    if ((index < 0) || (i > (L - 1))) {
        PyErr_SetString(PyExc_IndexError, "index out of range");
        boost::python::throw_error_already_set();
    }
    return p[i];
}

template <typename T, typename R>
R python_getitem(T const& p, int index){
    return python_getitem_size<T,R>(p, index, p.size());
}






struct Point_from_tuple{
    Point_from_tuple(){
        boost::python::converter::registry::push_back(
            &convertible,
            &construct,
            boost::python::type_id<Geom::Point>()
        );
    }

    static void* convertible(PyObject* obj_ptr){
        if(!PyTuple_Check(obj_ptr)) return 0;

        const boost::python::tuple tuple = boost::python::extract<boost::python::tuple>(obj_ptr);
        if(boost::python::len(tuple) != 2) return 0;

        return obj_ptr;
    }

    static void construct(PyObject* obj_ptr, boost::python::converter::rvalue_from_python_stage1_data* data){
        const boost::python::tuple tuple = boost::python::extract<boost::python::tuple>(obj_ptr);

        void* storage = (
            (boost::python::converter::rvalue_from_python_storage<Geom::Point>*)data
        )->storage.bytes;

        new (storage) Geom::Point(
            boost::python::extract<double>(tuple[0]),
            boost::python::extract<double>(tuple[1])
        );
        data->convertible = storage;
    }
};


template<typename T>
boost::python::object d2_to_pytuple(const Geom::D2<T>& d2){
    return boost::python::make_tuple(d2[0], d2[1]);
}

struct D2PwSb_to_python{
    static PyObject* convert(Geom::D2<Geom::Piecewise<Geom::SBasis> > const& d2pwsb){
        return boost::python::incref(
            d2_to_pytuple(d2pwsb).ptr()
        );
    }
};

template<typename T>
struct custom_vector_from_seq{
	custom_vector_from_seq(){ boost::python::converter::registry::push_back(&convertible,&construct,boost::python::type_id<std::vector<T> >()); }
	static void* convertible(PyObject* obj_ptr){
		// the second condition is important, for some reason otherwise there were attempted conversions of Body to list which failed afterwards.
		if(!PySequence_Check(obj_ptr) || !PyObject_HasAttrString(obj_ptr,"__len__")) return 0;
		return obj_ptr;
	}
	static void construct(PyObject* obj_ptr, boost::python::converter::rvalue_from_python_stage1_data* data){
		 void* storage=((boost::python::converter::rvalue_from_python_storage<std::vector<T> >*)(data))->storage.bytes;
		 new (storage) std::vector<T>();
		 std::vector<T>* v=(std::vector<T>*)(storage);
		 int l=PySequence_Size(obj_ptr); if(l<0) abort(); /*std::cerr<<"l="<<l<<"; "<<typeid(T).name()<<std::endl;*/ v->reserve(l); for(int i=0; i<l; i++) { v->push_back(boost::python::extract<T>(PySequence_GetItem(obj_ptr,i))); }
		 data->convertible=storage;
	}
};

template<typename T>
struct custom_vector_to_list{
    static PyObject* convert(std::vector<T> const& ts){
        boost::python::list list;
        typename std::vector<T>::const_iterator it;
        for(it = ts.begin(); it != ts.end(); ++it)
            list.append(*it);
        return boost::python::incref(list.ptr());
    }
};


#endif
