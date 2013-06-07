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
	WORD  ID;			//ʱ��Id
	int	  HourBegin;    //��ʼʱ�䣨Сʱ��
	int	  MinuteBegin;  //��ʼʱ�䣨�֣� 
	int	  HourEnd;      //����ʱ�䣨Сʱ��
	int	  MinuteEnd;    //����ʱ�䣨�֣�
};

struct TimeBound
{
	WORD           TimeBoundID;			     //ʱ��Id
	char           SubTimeBoundInfo[100];    //��ʱ����Ϣ 
	int            SubTimeBoundNum;          //��ʱ�θ���
    SubTimeBound   *pSubTimeBound;           //��ʱ������
};


struct  DataInfo
{
	int  BoxID;
	int  BoxIndex;
	int  DataType; //1:ң��   2:ң�� 3:ң��  4:���� 6:��λ
	int  DataId;   //��Ӧ��Ӧ���ݵ�ID��
	int  StatInGroupId;//��DMConfigModule��InGroupIdһ��
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
	double lastvalue;//ר����ң���������ڼ�����
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
 	char SOEType[10];	//SOE���ͣ�"YX"Ϊң�ű�λ��"YCU"Ϊң��Խ���ޣ�"YCD"Ϊң��Խ����
	WORD BoxID;			//�����
	WORD BoxIndex;		//���������
	WORD SpaceID;		//����ţ�����"YX"���ͣ������ݻ���š���������ŵõ���ֵ
	WORD SpaceIndex;	//�������ţ�����"YX"���ͣ������ݻ���š���������ŵõ���ֵ
	short Year;			//��
	BYTE Month;			//��
	BYTE Day;			//��
	BYTE Hour;			//ʱ
	BYTE Minute;		//��
	BYTE Second;		//��
	short MSecond;		//����
	double Value;		//ֵ������"YX"���ͣ�0Ϊ�ϵ��֣�����Ϊ�ֵ��ϣ�����"YCU"��"YCD"���ͣ�ΪԽ��ֵ
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__50487208_4BF1_4B50_9CBF_2B03B4939963__INCLUDED_)
