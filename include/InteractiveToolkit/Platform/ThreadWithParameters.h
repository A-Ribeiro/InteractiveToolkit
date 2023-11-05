#pragma once

#include "Thread.h"

namespace Platform
{

	//
	// Thread With Parameters Class
	//

	template <typename _RetType, typename... _ArgsType>
	class ThreadWithParameters
	{
	};

	template <typename _RetType, typename... _ArgsType>
	class ThreadWithParameters<_RetType(_ArgsType...)> : public Thread, public EventCore::HandleCallback
	{
	public:
		using SelfType = ThreadWithParameters<_RetType(_ArgsType...)>;

	protected:
		EventCore::Callback<_RetType(_ArgsType...)> runEntryPointWithParameters;
		std::tuple<_ArgsType...> userParams;

		template <std::size_t... I>
		ITK_INLINE void runEntryPointWithParamsExpanded(STL_Tools::index_sequence<I...>)
		{
			runEntryPointWithParameters(std::forward<_ArgsType>(std::get<I>(userParams))...);
		}

		void runEntryPointWithParams()
		{
			runEntryPointWithParamsExpanded(STL_Tools::make_index_sequence<(sizeof...(_ArgsType))>());
		}

	public:
		ThreadWithParameters(const EventCore::Callback<_RetType(_ArgsType...)>& _runEntryPoint) : Thread(EventCore::CallbackWrapper(&SelfType::runEntryPointWithParams, this))
		{
			runEntryPointWithParameters = _runEntryPoint;
		}

		ThreadWithParameters(EventCore::Callback<_RetType(_ArgsType...)>&& _runEntryPoint) : Thread(EventCore::CallbackWrapper(&SelfType::runEntryPointWithParams, this))
		{
			runEntryPointWithParameters = std::move(_runEntryPoint);
		}

		ThreadWithParameters(const EventCore::Callback<_RetType(_ArgsType...)>& _runEntryPoint, _ArgsType... _arg) : Thread(EventCore::CallbackWrapper(&SelfType::runEntryPointWithParams, this))
		{
			runEntryPointWithParameters = _runEntryPoint;
			userParams = std::tuple<_ArgsType...>(std::forward<_ArgsType>(_arg)...);
		}

		ThreadWithParameters(EventCore::Callback<_RetType(_ArgsType...)>&& _runEntryPoint, _ArgsType... _arg) : Thread(EventCore::CallbackWrapper(&SelfType::runEntryPointWithParams, this))
		{
			runEntryPointWithParameters = std::move(_runEntryPoint);
			userParams = std::tuple<_ArgsType...>(std::forward<_ArgsType>(_arg)...);
		}

		ITK_INLINE void setParameters(_ArgsType... _arg)
		{
			userParams = std::tuple<_ArgsType...>(std::forward<_ArgsType>(_arg)...);
		}
	};

}