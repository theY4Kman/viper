#ifndef __INCLUDE_FORWARDTYPE_H__
#define __INCLUDE_FORWARDTYPE_H__

#include "BoostPythonSM.h"
#include "Macros.h"
#include "STL.h"
#include "ForwardFunction.h"

typedef bool (*ForwardCallback)(boost::python::object returnValue);

class ForwardType {
	public:
		ForwardType(ForwardCallback callback, std::string name = std::string());

		void AddFunction(boost::python::object fn);
		bool RemoveFunction(boost::python::object fn);
		void Fire(boost::python::list argumentsList);
		int GetFunctionCount();

		boost::python::object StrMagic();

	public:// Called when forwards are unloading
		bool RemoveFirstFunctionByThreadState(PyThreadState *threadState);
		void RemoveFunctionsByThreadState(PyThreadState *threadState);

	private:
		std::string Name;
		std::list<ForwardFunction> Functions;
		ForwardCallback Callback;
};

#endif