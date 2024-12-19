#ifndef _InteratorResult_
#define _InteratorResult_

namespace sn3DCore
{
	enum ResultType
	{
		NOCHANGE,
		TOOTH_SELECTED_CHANGED,
		TOOTH_LOCATION_CHANGED,
		TOOTH_COLLISION,
		TOOTH_ATTACHMENT_CHANGED,//附件时附件改变
		ATTACHMENT_TOOTH_CHANGED,//附件时牙齿改变
		EXTRACT_TOOTH_CHANGED,//拔牙时牙齿改变
		IPR_TOOTH_CHANGED,//去釉时牙齿改变
		TOOTH_NEXT//切分时移到下一颗牙齿
	};
	enum CurrentOpt
	{
		NO_OPT,
		TREATMENT,
		SEGMENT
	};
	struct InteratorResult
	{
		ResultType resultType;
		CurrentOpt currentOperation;
		int toothIndex;
		bool bUper;
		InteratorResult()
		{
			resultType = NOCHANGE;
			currentOperation = NO_OPT;
			bUper = false;
			toothIndex = 0;
		}
	};
}

#endif
