#ifndef __INCLUDE_FORWARDTYPE_H__
#define __INCLUDE_FORWARDTYPE_H__

#include "BoostPythonSM.h"
#include "Macros.h"
#include "STL.h"

struct ForwardFunction {
	boost::python::object PythonFunction;
	PyThreadState *ThreadState;
};

typedef bool (*ForwardCallback)(boost::python::object returnValue);

class ForwardType {
	public:
		ForwardType(ForwardCallback callback, std::string name = std::string());

		void AddFunction(boost::python::object fn);
		bool RemoveFunction(boost::python::object fn);
		void Fire(boost::python::list argumentsList);
		int GetFunctionCount();

		boost::python::object StrMagic();

	private:
		std::string Name;
		std::vector<ForwardFunction> Functions;
		ForwardCallback Callback;
};

#endif