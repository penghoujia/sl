// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__50487208_4BF1_4B50_9CBF_2B03B4939963__INCLUDED_)
#define AFX_STDAFX_H__50487208_4BF1_4B50_9CBF_2B03B4939963__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

/*
#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT*/

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#import "c:\program files\common files\system\ado\msado15.dll" no_namespace rename("EOF","EndOfFile")
          
#define  IMPORT_RTDB_INTERFACE
#include "..\include\common.h"

#define EVENT_NUM  100

struct SubTimeBound
{
	WORD  ID;			//时段Id
	int	  HourBegin;    //开始时间（小时）
	int	  MinuteBegin;  //开始时间（分） 
	int	  HourEnd;      //结束时间（小时）
	int	  MinuteEnd;    //结束时间（分）
};

struct TimeBound
{
	WORD           TimeBoundID;			     //时段Id
	char           SubTimeBoundInfo[100];    //子时段信息 
	int            SubTimeBoundNum;          //子时段个数
    SubTimeBound   *pSubTimeBound;           //子时段数据
};


struct  DataInfo
{
	int  BoxID;
	int  BoxIndex;
	int  DataType; //1:遥测   2:遥脉 3:遥信  4:故障 6:档位
	int  DataId;   //对应相应数据的ID号
	int  StatInGroupId;//与DMConfigModule的InGroupId一致
	char Param1[100];  
	char Param2[100];
	BYTE IsFirstSave;
	CTime lastsavetime;

}; 

struct DcapBox
{
	WORD BoxID;
	char BoxName[100];
	int YCDataCount;
	int DDDataCount;
	int YXDataCount;
	int GZDataCount;
	int XBDataCount;
	int DZDataCount;
	int TTDataCount;
	int StationID;
	int CommID;
	int SpaceID;
	DWORD Addr;
	int ModeID;
	int PT;
	int CT;
};

struct DcapData
{
	BYTE TableType;	             //1:YC,2:DD,3:YX,4:GZ
	WORD BoxID;
	WORD BoxIndex;
	WORD SpaceID;
	WORD SpaceIndex;
	double Value;
	DWORD Status;
};

struct NewDcapData
{
	BYTE TableType;	             //1:YC,2:DD,3:YX,4:GZ
	WORD BoxID;
	WORD BoxIndex;
	WORD DataID;
	WORD SpaceID;
	WORD SpaceIndex;
	double Value;
	DWORD Status;
	bool IsFirstSave;
	CTime lastsavetime;
	int saveinterval;
	double lastvalue;//专用于遥脉量，用于计算电度
	bool IsSave;
};

struct BoxPara
{
    DcapBox Box;
    DcapData* m_YcData;
    DcapData* m_YxData;
    DcapData* m_YmData;
    DcapData* m_GzData;
    DcapData* m_XbData;
};

struct DcapSOEData
{
 	char SOEType[10];	//SOE类型，"YX"为遥信变位，"YCU"为遥测越上限，"YCD"为遥测越下限
	WORD BoxID;			//机箱号
	WORD BoxIndex;		//机箱内序号
	WORD SpaceID;		//间隔号，对于"YX"类型，将根据机箱号、机箱内序号得到此值
	WORD SpaceIndex;	//间隔内序号，对于"YX"类型，将根据机箱号、机箱内序号得到此值
	short Year;			//年
	BYTE Month;			//月
	BYTE Day;			//日
	BYTE Hour;			//时
	BYTE Minute;		//分
	BYTE Second;		//秒
	short MSecond;		//毫秒
	double Value;		//值，对于"YX"类型：0为合到分，其他为分到合；对于"YCU"、"YCD"类型，为越限值
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__50487208_4BF1_4B50_9CBF_2B03B4939963__INCLUDED_)
