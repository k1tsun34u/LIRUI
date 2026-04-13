#pragma once
#include "Typedefs.hpp"

namespace LIR {
	namespace UI {
		enum class WorkResult {
			Unknown,
			Success,
			ErrorRootClassNotRegistered,
			ErrorWindowNotCreated,
			ErrorSubclassNotSet,
			ErrorWindowNotUpdated,
			ErrorWindowWorkAsyncThreadNotCreated,
			ErrorWindowWorkAsyncThreadEventNotCreated,
			ErrorWaitWorkAsyncThreadFailedToReceive,
			ErrorNotEnoughMemory,
			ErrorMessageNotSent,
			ErrorTimeout,
			ErrorAlreadyInitialized,
		};
	}
}