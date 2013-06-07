// ReportDataSaveDll.h : main header file for the REPORTDATASAVEDLL DLL
//

#if !defined(AFX_REPORTDATASAVEDLL_H__F2228F40_2172_4644_9B66_A9DF8038EC90__INCLUDED_)
#define AFX_REPORTDATASAVEDLL_H__F2228F40_2172_4644_9B66_A9DF8038EC90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CReportDataSaveDllApp
// See ReportDataSaveDll.cpp for the implementation of this class
//
bool    m_bShutDown = false;

IDcapRtDBComPtr		m_IRtDB = NULL;			//实时数据库
bool   m_IRtDBBz=0;
_ConnectionPtr		m_pConnection = NULL;      //数据库
_RecordsetPtr		m_pRecordset = NULL;

_ConnectionPtr		m_pConnectionHis = NULL;      //数据库
_RecordsetPtr		m_pRecordsetHis = NULL;

int				  m_iDataInfoSum = 0;
DataInfo*	      m_pDataInfo = NULL;

BoxPara*          m_BoxPara=NULL;
int               m_iBoxNum=0;

DcapSOEData *     m_pSoeData = NULL;
int               m_iSoeSum = 0;
unsigned long     m_iEventPos = 0;


int m_icfgycnum = 0;//已配置报表存盘的遥测、遥脉、遥信个数
int m_icfgymnum = 0;
int m_icfgyxnum = 0;
int m_icfggznum = 0;
NewDcapData *m_pRealYc = NULL;
NewDcapData *m_pRealYm = NULL;
NewDcapData *m_pRealYx = NULL;
NewDcapData *m_pRealGz = NULL;

HWND g_MainWnd = NULL;

CWinThread  * m_pCommThread = NULL;

class CReportDataSaveDllApp : public CWinApp
{
public:
	CReportDataSaveDllApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReportDataSaveDllApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CReportDataSaveDllApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

bool RefreshRealData();
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTDATASAVEDLL_H__F2228F40_2172_4644_9B66_A9DF8038EC90__INCLUDED_)
