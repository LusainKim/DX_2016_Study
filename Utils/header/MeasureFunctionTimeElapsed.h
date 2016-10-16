#pragma once

#define use_Measure_Time_Elapsed true

struct function_invoke_return_late_for_measure_time_elapsed
{	
	template<
		  class _Callable
		, class _OutputFn
		, class... _Types>
		static auto _Call(_Callable&& _Obj, _OutputFn&& _Out, _Types&&... _Args)
	{	
		auto startTime = std::chrono::high_resolution_clock::now();

		auto retval = _STD invoke(
			  _STD forward<_Callable>(_Obj)
			, _STD forward<_Types>(_Args)...
		);

		auto du = std::chrono::high_resolution_clock::now() - (startTime);

		_STD invoke(_STD forward<_OutputFn>(_Out), du);

		return retval;
	}
};

struct function_invoke_no_return_for_measure_time_elapsed
{	
	template<
		  class _Callable
		, class _OutputFn
		, class... _Types>
		static auto _Call(_Callable&& _Obj, _OutputFn&& _Out, _Types&&... _Args)
	{	
		auto startTime = std::chrono::high_resolution_clock::now();

		_STD invoke(
			  _STD forward<_Callable>(_Obj)
			, _STD forward<_Types>(_Args)...
		);

		auto du = std::chrono::high_resolution_clock::now() - (startTime);

		_STD invoke(_STD forward<_OutputFn>(_Out), du);
	}
};

template<
	  typename returnType
	, bool _Is_void = is_void<returnType>::value
> 
struct InvokerMeasureFunc;

template<typename Ty>
struct InvokerMeasureFunc<Ty, true>
	: function_invoke_no_return_for_measure_time_elapsed
{
};

template<typename Ty>
struct InvokerMeasureFunc<Ty, false>
	: function_invoke_return_late_for_measure_time_elapsed
{
};

/// <summary>
/// <para>함수의 실행 시간을 측정하는 함수입니다. </para>
/// <para>use_Measure_Time_Elapsed 전처리기로 기능을 사용하지 않을 수 있습니다. </para>
/// <para>시간을 측정하는 함수는 인자로 duration〈long long, std::nano〉 타입의 인자를 받습니다. </para>
/// </summary>
///	<param name = "outputFunc"> 시간을 측정하는 함수입니다. 시간을 알고 싶다면 원하는 시간 열거자를 사용하세요.  </param>
///	<param name = "Function"> 실제 함수입니다. 반환값이 있을 경우, 해당 함수의 반환값으로 반환합니다. </param>
///	<param name = "args"> 실제 함수의 인자입니다. </param>
/// <remarks> 
///	<para>시간을 측정하는 함수는 인자를 duration 하나만을 가지는 함수입니다. 	  </para>
///	<para>실제 함수의 반환값이 void일 경우 반환값을 받으려는 시도를 하지 마십시오. </para>
///	<para>오버로드된 함수의 경우, 함수 본문에서 타입을 구체화시켜야 합니다. </para>
///	<para>예를 들어, 서명이 int(*)(int) 와 void(*)(std::string) 인 오버로드된 함수가 있을 경우, </para>
///	<para>static_cast 를 사용하여 함수의 타입을 확정지어야 합니다. </para>
///	<para>번거로울 경우, MeasureBlockTimeElapsed 함수를 사용하십시오. </para>
///	</remarks>
/// <returns> 실제 함수의 반환값이 있다면 그것을 반환합니다. 실제 함수의 반환값이 없을 경우 컴파일 에러가 발생합니다. </returns>
template<typename FnOutput, typename FnFunc, typename... Args> inline
constexpr auto MeasureFunctionTimeElapsed(FnOutput&& outputFunc, FnFunc&& Function, Args&&... args) noexcept
{
#if use_Measure_Time_Elapsed
	return (InvokerMeasureFunc<result_of<FnFunc &(Args...)>::type>::_Call(
		_STD forward<FnFunc>(Function), _STD forward<FnOutput>(outputFunc), _STD forward<Args>(args)...)
		);
#else
	return (_STD forward<FnFunc>(Function)(_STD forward<Args>(args)...));
#endif
}

/// <summary>
/// <para>람다로 감싼 블럭의 실행 시간을 측정하는 함수입니다. </para>
/// <para>use_Measure_Time_Elapsed 전처리기로 기능을 사용하지 않을 수 있습니다. </para>
/// <para>시간을 측정하는 함수는 인자로 duration〈long long, std::nano〉 타입의 인자를 받습니다. </para>
/// </summary>
///	<param name = "outputFunc"> 시간을 측정하는 함수입니다. 시간을 알고 싶다면 원하는 시간 열거자를 사용하세요.  </param>
///	<param name = "Function"> 실제 동작하는 블럭을 람다로 감싼 인자입니다. 반환값은 존재하지 않습니다. </param>
/// <remarks> 
///	<para>시간을 측정하는 함수는 인자를 duration 하나만을 가지는 함수입니다. </para>
///	<para><param>Function</param> 인자는 람다를 사용하여 구현하도록 의도했습니다. </para>
///	<para>블럭에서 결과를 반환해야 할 경우, 람다 캡쳐 기능을 사용하세요. </para>
///	<para>반환값을 받으려는 시도를 하지 마십시오. </para>
///	</remarks>
/// <returns> 반환값은 없습니다. </returns>
template<typename FnOutput, typename FnFunc> inline
void MeasureBlockTimeElapsed(FnOutput&& outputFunc, FnFunc&& Function) noexcept
{
#if use_Measure_Time_Elapsed

	auto startTime = std::chrono::high_resolution_clock::now();

	_STD forward<FnFunc>(Function)();

	auto du = std::chrono::high_resolution_clock::now() - (startTime);

	_STD invoke(_STD forward<FnOutput>(outputFunc), du);

#else
	_STD forward<FnFunc>(Function)();
#endif
}
