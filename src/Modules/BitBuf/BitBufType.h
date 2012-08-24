#ifndef __INCLUDE_BITBUFTYPE_H__
#define __INCLUDE_BITBUFTYPE_H__

#include "BoostPythonSM.h"
#include "Macros.h"
#include "STL.h"
#include "BitBufCell.h"

namespace Viper {
	namespace Modules {
		namespace BitBuf {
			class BitBufType {
				public:
					boost::python::object WriteAngle(
						boost::python::tuple argumentsTuple,
						boost::python::dict keywordsDict
					);

					boost::python::object WriteAngles(
						boost::python::tuple argumentsTuple,
						boost::python::dict keywordsDict
					);

				private:
					std::vector<BitBufCell *> Cells;
			};

			WRAP_OBJECT_METHOD_STUB(BitBufType, bitbuf, WriteAngle)
			WRAP_OBJECT_METHOD_STUB(BitBufType, bitbuf, WriteAngles)
		}
	}
}

#endif