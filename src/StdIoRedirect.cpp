#include "StdIoRedirect.h"
#include "sdk/smsdk_ext.h"

void StdIoRedirect::Write(std::string const& str) {
	g_SMAPI->ConPrint(str.c_str());
}

void StdIoRedirect::Flush() {

}