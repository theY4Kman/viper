#include "ViperTraceFilter.h"
#include "Macros.h"
#include "InterfaceContainer.h"

namespace py = boost::python;

ViperTraceFilter::ViperTraceFilter(PyThreadState *threadState, boost::python::object callbackFunction) {
	ThreadState = threadState;
	CallbackFunction = callbackFunction;
}

bool ViperTraceFilter::ShouldHitEntity(IHandleEntity *handleEntity, int contentsMask) {
	CBaseEntity *entity = reinterpret_cast<CBaseEntity *>(handleEntity);

	int entityIndex = 0;

	if(entity != NULL) {
		edict_t *edict = g_Interfaces.ServerGameEntsInstance->BaseEntityToEdict(entity);

		if(edict != NULL && !edict->IsFree()) {
			entityIndex = IndexOfEdict(edict);
		}
	}

	if(CallbackFunction.is_none()) {
		return true;
	}

	PyThreadState *oldThreadState = PyThreadState_Get();

	PyThreadState_Swap(ThreadState);

	bool shouldHit;

	shouldHit = py::extract<bool>(CallbackFunction(entityIndex, contentsMask));
	
	PyThreadState_Swap(oldThreadState);

	return shouldHit;
}