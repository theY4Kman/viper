#ifndef __INCLUDE_MACROS_H__
#define __INCLUDE_MACROS_H__

#define WRAP_OBJECT_METHOD(cpptype, module, method) \
	boost::python::object module##__##cpptype##_##method( \
		boost::python::tuple argumentsTuple, \
		boost::python::dict keywordsDict) { \
	boost::python::object self = argumentsTuple[0]; \
	cpptype *instance = boost::python::extract<cpptype*>(self); \
	return instance->method(argumentsTuple, keywordsDict); \
}

#define WRAP_OBJECT_METHOD_STUB(cpptype, module, method) \
	extern boost::python::object module##__##cpptype##_##method( \
		boost::python::tuple argumentsTuple, boost::python::dict keywordsDict);

// To define a custom exception type, you need to call the following!
// Call either DEFINE_CUSTOM_EXCEPTION_INIT or DEFINE_CUSTOM_EXCEPTION_INIT_EX
// right above the BOOST_PYTHON_MODULE(){} it in the CPP source
#define DEFINE_CUSTOM_EXCEPTION_INIT(cpptype, module) \
	PyObject *module##_##cpptype = NULL; \
	void module##_##cpptype##Translator(const cpptype &e) { \
		PyErr_Format(module##_##cpptype, e.what().c_str()); \
	}

// If you use DEFINE_CUSTOM_EXCEPTION_INIT_EX, you must write your own exception
// translation function: void <module>_<cpptype>Translator(const <cpptype> &e)
#define DEFINE_CUSTOM_EXCEPTION_INIT_EX(cpptype, module) \
	PyObject *module##_##cpptype = NULL;

// Call this inside BOOST_PYTHON_MODULE(){}
#define DEFINE_CUSTOM_EXCEPTION(cpptype, module, pybasetype, pypath, typestr) \
	if(NULL == module##_##cpptype) { \
		module##_##cpptype = PyErr_NewException(pypath, pybasetype,\
			NULL); \
	} \
	PyModule_AddObject(boost::python::scope().ptr(), typestr, \
		module##_##cpptype); \
	boost::python::register_exception_translator<cpptype>( \
		module##_##cpptype##Translator);


// Call this in the header
#define DEFINE_CUSTOM_EXCEPTION_DECL(cpptype, module) \
	extern PyObject *module##_##cpptype;

#endif