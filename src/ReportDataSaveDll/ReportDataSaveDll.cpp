// ReportDataSaveDll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ReportDataSaveDll.h"
#include "ConfigFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CReportDataSaveDllApp

BEGIN_MESSAGE_MAP(CReportDataSaveDllApp, CWinApp)
	//{{AFX_MSG_MAP(CReportDataSaveDllApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportDataSaveDllApp construction

CReportDataSaveDllApp::CReportDataSaveDllApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CReportDataSaveDllApp object

CReportDataSaveDllApp theApp;

BOOL CReportDataSaveDllApp::InitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class

	return CWinApp::InitInstance();
}

int CReportDataSaveDllApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_bShutDown=1;	

	return CWinApp::ExitInstance();
}

bool OpenDB(int Type)
{
	HRESULT hr;
	COleVariant v;
	COleVariant  vTemp;
	CConfigFile Config;

	char str[64];
	CString temp;
	CString m_strSQLServerName;
	CString m_szDatabaseName;
	CString m_szHisDBName;
	CString  strsql;

	//从配置文件中读取参数	
	HINSTANCE hInst = AfxGetInstanceHandle();//获取当前应用程序的实例
	char szPath[255]= "";
	GetModuleFileName(hInst,szPath,255);//获取应用程序的名称(包括路径)
	CString str1 = CString(szPath);
	CString szExeDir = str1.Left(str1.ReverseFind('\\'));//分离路经与文件名，获取应用程序的路径

	CString strPath ;
	strPath.Format(TEXT("%s\\%s"), szExeDir, CONFIG_FILE_NAME);

	if(Config.Open(strPath,FALSE)==FALSE)	//打开配置文件
		return FALSE;
	else									//读配置数据
	{	
		if(Config.OpenSection("db")==FALSE)    	//打开段
		{
			return FALSE;
		}		
		if(Config.ReadString("server",str,20))
		{
			m_strSQLServerName = str ;			    
		}
		else                              
		{
			return FALSE;
		}
		if(Config.ReadString("paramdb",str,20))
		{
			m_szDatabaseName = str ;
		}
		else      
		{
			return FALSE;
		}
		if(Config.ReadString("historydb",str,20))
		{
			m_szHisDBName = str ;
		}
		else      
		{
			return FALSE;
		}
	}

	if(Type==0||Type==1)
	{
		try
		{
			hr = m_pConnection.CreateInstance( __uuidof( Connection ) );
			if (SUCCEEDED(hr))
			{
				CString strConnection;
				strConnection.Format("Provider=SQLOLEDB.1;UID=sa;PWD=;Database=%s;Server=%s",m_szDatabaseName,m_strSQLServerName);//CString("chucg");//ServerName;
				hr = m_pConnection->Open(_bstr_t(strConnection),_bstr_t(""),_bstr_t(""),adConnectUnspecified);
				if (SUCCEEDED(hr))
				{
				}
				else
				{
					AfxMessageBox("Cann't Connect ParaDb! Please Check Configuration File And Network Configuration!");
					return FALSE;
				}
			}
		}
		catch( _com_error &e )
		{
			// Get info from _com_error
			_bstr_t bstrSource(e.Source());
			_bstr_t bstrDescription(e.Description());
			return FALSE;
		}
		catch(...)
		{
			return FALSE;
		}

		m_pRecordset.CreateInstance(__uuidof(Recordset));
	}

	if(Type==0||Type==2)
	{
		try
		{
			hr = m_pConnectionHis.CreateInstance( __uuidof( Connection ) );
			if (SUCCEEDED(hr))
			{
				CString strConnection;
				strConnection.Format("Provider=SQLOLEDB.1;UID=sa;PWD=;Database=%s;Server=%s",m_szHisDBName,m_strSQLServerName);//CString("chucg");//ServerName;
				hr = m_pConnectionHis->Open(_bstr_t(strConnection),_bstr_t(""),_bstr_t(""),adConnectUnspecified);
				if (SUCCEEDED(hr))
				{
				}
				else
				{
					AfxMessageBox("Cann't Connect ParaDb! Please Check Configuration File And Network Configuration!");
					return FALSE;
				}
			}
		}
		catch( _com_error &e )
		{
			// Get info from _com_error
			_bstr_t bstrSource(e.Source());
			_bstr_t bstrDescription(e.Description());
			return FALSE;
		}
		catch(...)
		{
			return FALSE;
		}

		m_pRecordsetHis.CreateInstance(__uuidof(Recordset));
	}

	return true;
}

bool ConnectToRtdb()
{
	//创建接口
	try
	{
		m_IRtDB.CreateInstance(__uuidof(DcapRtDBCom));	
	}
	catch(...)
	{	
		return FALSE;
	}

	m_IRtDBBz =true;
	return true;
}

bool CreateReportTable(int iYear,int iMonth,int iDate)
{
	HRESULT	hr;	
	CString strSql,strTable;
	bool TableSign=false; 
	strTable.Format("RPT_CUR_%d_%02d_%02d",iYear,iMonth,iDate);

	strSql.Format("Select * from %s",strTable);
	if(m_pRecordsetHis->State)
		m_pRecordsetHis->Close();
	COleVariant v;
	V_VT(&v) = VT_DISPATCH;
	V_DISPATCH(&v) = (IDispatch*)m_pConnectionHis;
	V_DISPATCH(&v)->AddRef(); 
	try
	{		
		hr=m_pRecordsetHis->Open((_variant_t)strSql,v, adOpenDynamic, adLockOptimistic, adCmdUnknown);
		if(SUCCEEDED(hr))
		{
		} 
		else
		{
			TableSign=true;
		}
	}
	catch( ... )
	{
		TableSign=true;
	}
	if(m_pRecordsetHis->State)
		m_pRecordsetHis->Close();

	if(TableSign)
	{

		strSql.Format("CREATE TABLE [dbo].[%s] \
					  ([BoxID] [int] NOT NULL, \
					  [BoxIndex] [int] NOT NULL, \
					  [DataID]  [int] NOT NULL, \
					  [DataType] [int] NOT NULL, \
					  [DataHour]  [int] NOT NULL,\
					  [DataMinute]  [int] NOT NULL, \
					  [DataValue] [float] NOT NULL, \
					  [ValueType]  [int] NOT NULL)",
					  strTable);
		//	AfxMessageBox(strSql);

		// 	strSql.Format("Create Table CURVE_2009_03 (StationID int NOT NULL,DataStationIndex int NOT NULL,DataID int NOT NULL,DataType int NOT NULL,DataDay int NOT NULL,TimeSpace int NOT NULL,DataValue image NULL)");


		try
		{		
			hr=m_pConnectionHis->Execute(_bstr_t(strSql), NULL, adExecuteNoRecords);
			if(SUCCEEDED(hr))
			{
			} 
			else
			{
				return false;
			}
		}
		catch(_com_error e)
		{
			CString stemp=e.ErrorMessage();
			return false;
		}	
	}
	return true;
}


void InitReportTable()
{
	CString strSql,strTable;
	HRESULT hr;
	bool InitSign=0;
	CTime tm;
	tm = CTime::GetCurrentTime();
	int Year,Month,Date;
	Year = tm.GetYear();
	Month = tm.GetMonth();
	Date = tm.GetDay();


	strTable.Format("RPT_CUR_%d_%02d_%02d",Year,Month,Date);		
	if(m_pRecordset->State)
		m_pRecordset->Close();
	COleVariant v;
	V_VT(&v) = VT_DISPATCH;
	V_DISPATCH(&v) = (IDispatch*)m_pConnection;
	V_DISPATCH(&v)->AddRef(); 
	strSql.Format("select * from %s ",strTable);
	try{
		hr=m_pRecordset->Open((_variant_t)strSql,v, adOpenDynamic, adLockOptimistic, adCmdUnknown);
		if(SUCCEEDED(hr))
		{
		} 
		else
		{
			InitSign=1;
		}
	}
	catch(...)
	{
		InitSign=1;
	}   

	if(m_pRecordset->State)
		m_pRecordset->Close();

	if(InitSign)
	{
		CreateReportTable(Year,Month,Date);
	}

}


bool ReadDataInfoTable()
{
	HRESULT	hr;
	int DataNum=0;
	CString strSql,str_getycnum,str_getymnum,str_getyxnum,str_getgznum;
	strSql.Format("select *  from CR_DataInfo where StatInGroupId=1 order by DataId ");
	str_getycnum.Format("select COUNT(*) as nCount from CR_DataInfo where DataType=1 and StatInGroupId=1");//配置了报表的遥测个数
	str_getymnum.Format("select COUNT(*) as nCount from CR_DataInfo where DataType=2 and StatInGroupId=1");//配置了报表的遥脉个数
	str_getyxnum.Format("select COUNT(*) as nCount from CR_DataInfo where DataType=3 and StatInGroupId=1");//配置了报表的遥信s个数
	str_getgznum.Format("select COUNT(*) as nCount from CR_DataInfo where DataType=8 and StatInGroupId=1");
	VARIANT_BOOL bEOF=true;
	_variant_t  vTemp;
	if(m_pRecordset->State)
		m_pRecordset->Close();
	COleVariant v;
	V_VT(&v) = VT_DISPATCH;
	V_DISPATCH(&v) = (IDispatch*)m_pConnection;
	V_DISPATCH(&v)->AddRef(); 
	try
	{
		hr=m_pRecordset->Open((_variant_t)str_getycnum,v, adOpenDynamic, adLockOptimistic, adCmdUnknown);
		if(SUCCEEDED(hr))
		{
			m_pRecordset->MoveFirst();
			vTemp = m_pRecordset->GetCollect(_variant_t("nCount"));
			m_icfgycnum = vTemp.intVal;
		} 
		else
		{
			return false;
		}

	}
	catch(...)
	{
		return false;

	}

	try
	{
		if(m_pRecordset->State)
			m_pRecordset->Close();
		hr=m_pRecordset->Open((_variant_t)str_getymnum,v, adOpenDynamic, adLockOptimistic, adCmdUnknown);
		if(SUCCEEDED(hr))
		{
			m_pRecordset->MoveFirst();
			vTemp = m_pRecordset->GetCollect(_variant_t("nCount"));
			m_icfgymnum = vTemp.intVal;
		} 
		else
		{
			return false;
		}

	}
	catch(...)
	{
		return false;

	}

	try
	{
		if(m_pRecordset->State)
			m_pRecordset->Close();

		hr=m_pRecordset->Open((_variant_t)str_getyxnum,v, adOpenDynamic, adLockOptimistic, adCmdUnknown);
		if(SUCCEEDED(hr))
		{
			m_pRecordset->MoveFirst();
			vTemp = m_pRecordset->GetCollect(_variant_t("nCount"));
			m_icfgyxnum = vTemp.intVal;
		} 
		else
		{
			return false;
		}

	}
	catch(...)
	{
		return false;

	}

	try
	{
		if(m_pRecordset->State)
			m_pRecordset->Close();

		hr=m_pRecordset->Open((_variant_t)str_getgznum,v, adOpenDynamic, adLockOptimistic, adCmdUnknown);
		if(SUCCEEDED(hr))
		{
			m_pRecordset->MoveFirst();
			vTemp = m_pRecordset->GetCollect(_variant_t("nCount"));
			m_icfggznum = vTemp.intVal;
		} 
		else
		{
			return false;
		}

	}
	catch(...)
	{
		return false;

	}


	if((m_icfgycnum+m_icfgymnum+m_icfgyxnum+m_icfggznum)>0)
	{


		try
		{	if(m_pRecordset->State)
		m_pRecordset->Close();
		hr=m_pRecordset->Open((_variant_t)strSql,v, adOpenDynamic, adLockOptimistic, adCmdUnknown);
		if(SUCCEEDED(hr))
		{
		} 
		else
		{
			return false;
		}
		}
		catch(...)
		{
			return false;
		}



		try
		{
			m_pRecordset->MoveFirst();
			m_pRecordset->get_EndOfFile(&bEOF) ;
			if(!bEOF)
			{
				m_pRecordset->MoveFirst(); 
				while(!bEOF)
				{
					DataNum++;
					m_pRecordset->MoveNext();	
					m_pRecordset->get_EndOfFile(&bEOF);
				}
			}
			else
			{
				return false;
			}
		}
		catch(...)
		{
			return false;
		}

		m_iDataInfoSum = DataNum;
		m_pDataInfo = new DataInfo[m_iDataInfoSum];
		memset(m_pDataInfo,0,sizeof(DataInfo)*m_iDataInfoSum);

		if(DataNum!=0)
		{
			m_pRecordset->MoveFirst();
			for(int i=0;i<DataNum;i++)
			{
				try
				{		
					vTemp=m_pRecordset->GetCollect(_variant_t("BoxID"));
					m_pDataInfo[i].BoxID=vTemp.intVal;

					vTemp=m_pRecordset->GetCollect(_variant_t("BoxIndex"));
					m_pDataInfo[i].BoxIndex=vTemp.intVal;


					vTemp=m_pRecordset->GetCollect(_variant_t("DataType"));
					m_pDataInfo[i].DataType=vTemp.intVal;	

					vTemp=m_pRecordset->GetCollect(_variant_t("DataId"));
					m_pDataInfo[i].DataId=vTemp.intVal;	

					vTemp=m_pRecordset->GetCollect(_variant_t("StatInGroupId"));
					m_pDataInfo[i].StatInGroupId=vTemp.intVal;

					vTemp=m_pRecordset->GetCollect(_variant_t("Param1"));
					strcpy(m_pDataInfo[i].Param1,(char*)_bstr_t(vTemp));

					vTemp=m_pRecordset->GetCollect(_variant_t("Param2"));
					strcpy(m_pDataInfo[i].Param2,(char*)_bstr_t(vTemp));

					m_pDataInfo[i].IsFirstSave = 1;


					m_pRecordset->MoveNext();   
				}
				catch(...)
				{
					return false;
				}
			}
		}
	}
	if(m_pRecordset->State)
		m_pRecordset->Close(); 

	return true;
}

//bool InitDataInfoGroup(int Pos)
//{
//	HRESULT	hr;
//	int DataNum=0;
//	CString strSql;
//	strSql.Format("select StatInGroupId,Param1,Param2 from T_DataInfo where DataType=%d and DataId=%d \
//		and StatInGroupId!=2 and StatInGroupId!=3 and StatInGroupId!=7",m_pDataInfo[Pos].DataType,m_pDataInfo[Pos].DataId);
//	VARIANT_BOOL bEOF=true;
//	_variant_t  vTemp;
//	if(m_pRecordset->State)
//		m_pRecordset->Close();
//	COleVariant v;
//	V_VT(&v) = VT_DISPATCH;
//	V_DISPATCH(&v) = (IDispatch*)m_pConnection;
//	V_DISPATCH(&v)->AddRef(); 
//	try
//	{		
//		hr=m_pRecordset->Open((_variant_t)strSql,v, adOpenDynamic, adLockOptimistic, adCmdUnknown);
//		if(SUCCEEDED(hr))
//		{
//		} 
//		else
//		{
//			return false;
//		}
//	}
//	catch(...)
//	{
//		return false;
//	}
//	
//	try
//	{
//		m_pRecordset->MoveFirst();
//		m_pRecordset->get_ISEOF(&bEOF) ;
//		if(!bEOF)
//		{
//			m_pRecordset->MoveFirst(); 
//			while(!bEOF)
//			{
//				DataNum++;
//				m_pRecordset->MoveNext();	
//				m_pRecordset->get_ISEOF(&bEOF);
//			}
//		}
//		else
//		{
//			return false;
//		}
//	}
//	catch(...)
//	{
//		return false;
//	}
//	
//    m_pDataInfo[Pos].GroupNum = DataNum;
//    m_pDataInfo[Pos].pStatInGroupId = new int[DataNum];
//	memset(m_pDataInfo[Pos].pStatInGroupId,0,sizeof(int)*DataNum);
//
//	if(DataNum!=0)
//	{
//		m_pRecordset->MoveFirst();
//        for(int i=0;i<DataNum;i++)
//		{
//			try
//			{	
//				vTemp=m_pRecordset->GetCollect(_variant_t("Param1"));
//				char Para[100];
//				strcpy(Para,(char*)_bstr_t(vTemp));	
//
//				vTemp=m_pRecordset->GetCollect(_variant_t("StatInGroupId"));
//				int iTemp=vTemp.intVal;
//
//				if(iTemp==1)
//				{
//                    m_pDataInfo[Pos].pStatInGroupId[i]=iTemp;
//					m_pDataInfo[Pos].Param[0]=atoi(Para);
//				}
//				else
//				{
//					CString strTemp=Para;
//					if(strTemp=="Yes")
//					{
//                        m_pDataInfo[Pos].pStatInGroupId[i]=iTemp;
//					}
//				}
//					
//				m_pRecordset->MoveNext();   
//			}
//			catch(...)
//			{
//				return false;
//			}
//		}
//	}
//
//	if(m_pRecordset->State)
//		m_pRecordset->Close(); 
//
//	return true;
//}

//bool ReadSubTimeBoundTable(int DataPos,int TimeBoundPos,int SubTimeBoundPos)
//{ 
//	HRESULT	hr;
//	int DataNum=0;
//	CString strSql;
//	strSql.Format("select H_Bgn,M_Bgn,H_End,M_End from T_SubTimeBound where Id=%d",m_pDataInfo[DataPos].pTimeBound[TimeBoundPos].pSubTimeBound[SubTimeBoundPos].ID);
//	VARIANT_BOOL bEOF=true;
//	_variant_t  vTemp;
//	if(m_pRecordset->State)
//		m_pRecordset->Close();
//	COleVariant v;
//	V_VT(&v) = VT_DISPATCH;
//	V_DISPATCH(&v) = (IDispatch*)m_pConnection;
//	V_DISPATCH(&v)->AddRef(); 
//	try
//	{		
//		hr=m_pRecordset->Open((_variant_t)strSql,v, adOpenDynamic, adLockOptimistic, adCmdUnknown);
//		if(SUCCEEDED(hr))
//		{
//		} 
//		else
//		{
//			return false;
//		}
//	}
//	catch(...)
//	{
//		return false;
//	}
//	
//	try
//	{
//		m_pRecordset->MoveFirst();
//		m_pRecordset->get_ISEOF(&bEOF) ;
//		if(!bEOF)
//		{
//			m_pRecordset->MoveFirst(); 
//			while(!bEOF)
//			{
//				DataNum++;
//				m_pRecordset->MoveNext();	
//				m_pRecordset->get_ISEOF(&bEOF);
//			}
//		}
//		else
//		{
//			return false;
//		}
//	}
//	catch(...)
//	{
//		return false;
//	}
//	
//	if(DataNum!=0)
//	{
//		m_pRecordset->MoveFirst();
//		for(int i=0;i<DataNum;i++)
//		{
//			try
//			{	
//                vTemp=m_pRecordset->GetCollect(_variant_t("H_Bgn"));
//				m_pDataInfo[DataPos].pTimeBound[TimeBoundPos].pSubTimeBound[SubTimeBoundPos].HourBegin=vTemp.intVal;
//
//				vTemp=m_pRecordset->GetCollect(_variant_t("H_End"));
//				m_pDataInfo[DataPos].pTimeBound[TimeBoundPos].pSubTimeBound[SubTimeBoundPos].HourEnd=vTemp.intVal;
//
//				vTemp=m_pRecordset->GetCollect(_variant_t("M_Bgn"));
//				m_pDataInfo[DataPos].pTimeBound[TimeBoundPos].pSubTimeBound[SubTimeBoundPos].MinuteBegin=vTemp.intVal;
//
//				vTemp=m_pRecordset->GetCollect(_variant_t("M_End"));
//				m_pDataInfo[DataPos].pTimeBound[TimeBoundPos].pSubTimeBound[SubTimeBoundPos].MinuteEnd=vTemp.intVal;
//
//				m_pRecordset->MoveNext();   
//			}
//			catch(...)
//			{
//				return false;
//			}
//		}
//	}
//	
//	if(m_pRecordset->State)
//		m_pRecordset->Close(); 
//
//	return true;
//}











bool InitDB()
{
	if(!OpenDB(0)) return false;

	if(!ReadDataInfoTable()) return false;



	return true;
}

double ReadValue(BYTE TableType, WORD BoxID, WORD BoxIndex)
{
	int i=0,j=0;
	for(i=0;i<m_iBoxNum;i++)
	{
		if(m_BoxPara[i].Box.BoxID==BoxID)
		{
			switch(TableType)
			{
			case 1:
				for(j=0;j<m_BoxPara[i].Box.YCDataCount;j++)
				{
					if(m_BoxPara[i].m_YcData[j].BoxIndex==BoxIndex)
						return m_BoxPara[i].m_YcData[j].Value;
				}
				break;
			case 2:
				for(j=0;j<m_BoxPara[i].Box.DDDataCount;j++)
				{
					if(m_BoxPara[i].m_YmData[j].BoxIndex==BoxIndex)
						return m_BoxPara[i].m_YmData[j].Value;
				}
				break;
			case 3:
				for(j=0;j<m_BoxPara[i].Box.YXDataCount;j++)
				{
					if(m_BoxPara[i].m_YxData[j].BoxIndex==BoxIndex)
						return m_BoxPara[i].m_YxData[j].Value;
				}
				break;
			case 4:
				for(j=0;j<m_BoxPara[i].Box.GZDataCount;j++)
				{
					if(m_BoxPara[i].m_GzData[j].BoxIndex==BoxIndex)
						return m_BoxPara[i].m_GzData[j].Value;
				}
				break;
			case 5:
				for(j=0;j<m_BoxPara[i].Box.XBDataCount;j++)
				{
					if(m_BoxPara[i].m_XbData[j].BoxIndex==BoxIndex)
						return m_BoxPara[i].m_XbData[j].Value;
				}
				break;
			default:
				break;
			}
		}
	} 
	return 0;
}

DWORD ReadState(BYTE TableType, WORD BoxID, WORD BoxIndex)
{
	int i=0,j=0;
	for(i=0;i<m_iBoxNum;i++)
	{
		if(m_BoxPara[i].Box.BoxID==BoxID)
		{
			switch(TableType)
			{
			case 1:
				for(j=0;j<m_BoxPara[i].Box.YCDataCount;j++)
				{
					if(m_BoxPara[i].m_YcData[j].BoxIndex==BoxIndex)
						return m_BoxPara[i].m_YcData[j].Status;
				}
				break;
			case 2:
				for(j=0;j<m_BoxPara[i].Box.DDDataCount;j++)
				{
					if(m_BoxPara[i].m_YmData[j].BoxIndex==BoxIndex)
						return m_BoxPara[i].m_YmData[j].Status;
				}
				break;
			case 3:
				for(j=0;j<m_BoxPara[i].Box.YXDataCount;j++)
				{
					if(m_BoxPara[i].m_YxData[j].BoxIndex==BoxIndex)
						return m_BoxPara[i].m_YxData[j].Status;
				}
				break;
			case 4:
				for(j=0;j<m_BoxPara[i].Box.GZDataCount;j++)
				{
					if(m_BoxPara[i].m_GzData[j].BoxIndex==BoxIndex)
						return m_BoxPara[i].m_GzData[j].Status;
				}
				break;
			case 5:
				for(j=0;j<m_BoxPara[i].Box.XBDataCount;j++)
				{
					if(m_BoxPara[i].m_XbData[j].BoxIndex==BoxIndex)
						return m_BoxPara[i].m_XbData[j].Status;
				}
				break;
			default:
				break;
			}
		}
	} 
	return 0;
}

bool InitBoxPara()
{
	try
	{
		int nDataCount=0;
		COleVariant pvDataArray;		
		VariantClear(&pvDataArray);	
		m_IRtDB->GetBoxPara(&nDataCount, &pvDataArray);
		DcapBox *pData ;
		pData = (DcapBox *)pvDataArray.parray->pvData;		
		m_BoxPara = new BoxPara[nDataCount];
		memset(m_BoxPara,0,sizeof(BoxPara)*nDataCount);
		m_iBoxNum = nDataCount;	
		for(int i=0;i<m_iBoxNum;i++)
		{
			memcpy(&(m_BoxPara[i].Box),&(pData[i]),sizeof(DcapBox));
			m_BoxPara[i].m_YcData=new DcapData[m_BoxPara[i].Box.YCDataCount];
			memset(m_BoxPara[i].m_YcData,0,sizeof(DcapData)*m_BoxPara[i].Box.YCDataCount);
			m_BoxPara[i].m_YxData=new DcapData[m_BoxPara[i].Box.YXDataCount];
			memset(m_BoxPara[i].m_YxData,0,sizeof(DcapData)*m_BoxPara[i].Box.YXDataCount);
			m_BoxPara[i].m_YmData=new DcapData[m_BoxPara[i].Box.DDDataCount];
			memset(m_BoxPara[i].m_YmData,0,sizeof(DcapData)*m_BoxPara[i].Box.DDDataCount);
			m_BoxPara[i].m_GzData=new DcapData[m_BoxPara[i].Box.GZDataCount];
			memset(m_BoxPara[i].m_GzData,0,sizeof(DcapData)*m_BoxPara[i].Box.GZDataCount);
			m_BoxPara[i].m_XbData=new DcapData[m_BoxPara[i].Box.XBDataCount];
			memset(m_BoxPara[i].m_XbData,0,sizeof(DcapData)*m_BoxPara[i].Box.XBDataCount);
		}

		VariantClear(&pvDataArray);	
	}
	catch(...)
	{
		return 0;
	}  
	return 1;
}

bool UpdateValue(int BoxID, int Type, DcapData *pDataValue)
{
	CString str;
	if(!m_IRtDBBz)
		ConnectToRtdb();
	if(m_IRtDBBz)
	{	
		try
		{
			WORD nDataCount;
			COleVariant pvDataArray;		
			VariantClear(&pvDataArray);	
			m_IRtDB->GetDataByBoxID(BoxID,Type,&nDataCount, &pvDataArray);
			DcapData *pData ;
			pData = (DcapData *)pvDataArray.parray->pvData;		
			memcpy(pDataValue,pData,sizeof(DcapData) * nDataCount);
			VariantClear(&pvDataArray);	
		}
		catch( _com_error &e )
		{
			// Get info from _com_error
			_bstr_t bstrSource(e.Source());
			_bstr_t bstrDescription(e.Description());
			return 0;
		}
		catch(...)
		{
			m_IRtDBBz = false;
			str.Format("Refresh RtDB Error,*** Unhandled Exception ***");
			return 0;
		}
	}
	else
	{
		return 0;
	}
	return 1;    
}

bool UpdateRtdbData(int BoxPos)
{
	UpdateValue(m_BoxPara[BoxPos].Box.BoxID,1,m_BoxPara[BoxPos].m_YcData);
	UpdateValue(m_BoxPara[BoxPos].Box.BoxID,3,m_BoxPara[BoxPos].m_YxData);
	UpdateValue(m_BoxPara[BoxPos].Box.BoxID,2,m_BoxPara[BoxPos].m_YmData);
	UpdateValue(m_BoxPara[BoxPos].Box.BoxID,4,m_BoxPara[BoxPos].m_GzData);
	UpdateValue(m_BoxPara[BoxPos].Box.BoxID,5,m_BoxPara[BoxPos].m_XbData);
	return 1;
}

void RefreshData()
{
	for(int i=0;i<m_iBoxNum;i++)
	{
		UpdateRtdbData(i);
	}
}

//bool CreateReportTable(int TableType, CString TableName)
//{
//	HRESULT hr;
//	CString strSql;
//	if(TableType!=2)
//	{
//	strSql.Format("CREATE TABLE [dbo].[%s] \
//		([Value] [float] NOT NULL, \
//		[Year] [tinyint] NOT NULL, \
//		[Month]  [tinyint] NOT NULL, \
//		[Day] [tinyint] NOT NULL, \
//		[Hour] [tinyint] NOT NULL,\
//		[Minute] [tinyint] NOT NULL)",
//		TableName);
//	}
//	else
//	{
//      strSql.Format("CREATE TABLE [dbo].[%s] \
//		([Value] [float] NOT NULL, \
//		[ValueType] [tinyint] NOT NULL, \
//		[Year] [tinyint] NOT NULL, \
//		[Month]  [tinyint] NOT NULL, \
//		[Day] [tinyint] NOT NULL, \
//		[Hour] [tinyint] NOT NULL,\
//		[Minute] [tinyint] NOT NULL)",
//		TableName);
//	}
//	try
//	{		
//		hr=m_pConnectionHis->Execute(_bstr_t(strSql), NULL, adExecuteNoRecords);
//		if(SUCCEEDED(hr))
//		{
//		} 
//		else
//		{
//			return false;
//		}
//	}
//	catch(...)
//	{
//		return false;
//	}	
//	return true;
//}

bool CreateReportStatTable(CString TableName)
{
	HRESULT hr;
	CString strSql;
	strSql.Format("CREATE TABLE [dbo].[%s] \
				  ([Value] [float] NOT NULL, \
				  [StatInGroupId] [tinyint] NOT NULL, \
				  [TimeBoundId]  [tinyint] NOT NULL, \
				  [Year] [tinyint] NOT NULL, \
				  [Month] [tinyint] NOT NULL,\
				  [Day] [tinyint] NOT NULL)",
				  TableName);
	try
	{		
		hr=m_pConnectionHis->Execute(_bstr_t(strSql), NULL, adExecuteNoRecords);
		if(SUCCEEDED(hr))
		{
		} 
		else
		{
			return false;
		}
	}
	catch(...)
	{
		return false;
	}	
	return true;
}





bool SaveReportData(int BoxID,int BoxIndex,int ID,int Type,int Year,int Month,int Day,int Hour,int Minute,float Value,int valuetype)
{
	CString strSql,strTable;
	strTable.Format("RPT_CUR_%d_%02d_%02d",Year,Month,Day);
	VARIANT_BOOL bEOF=true;
	HRESULT hr;

	if(m_pRecordsetHis->State)
		m_pRecordsetHis->Close();
	COleVariant v;
	V_VT(&v) = VT_DISPATCH;
	V_DISPATCH(&v) = (IDispatch*)m_pConnectionHis;
	V_DISPATCH(&v)->AddRef(); 
	strSql.Format("select * from %s where BoxID=%d and BoxIndex=%d and DataID=%d and ValueType=%d and DataHour=%d and DataMinute=%d",strTable,BoxID,BoxIndex,ID,valuetype,Hour,Minute);


	try{
		hr=m_pRecordsetHis->Open((_variant_t)strSql,v, adOpenDynamic, adLockOptimistic, adCmdUnknown);
		if(SUCCEEDED(hr))
		{
		} 
		else
		{
			return false;
		}
	}
	catch(...)
	{

		if(m_pRecordsetHis->State)
			m_pRecordsetHis->Close();

		return false;
	}

	try
	{
		m_pRecordsetHis->MoveFirst();
		m_pRecordsetHis->get_EndOfFile(&bEOF) ;
	}
	catch(...)
	{
	}

	m_pConnectionHis->BeginTrans();

	try{	
		if(bEOF)
		{

			m_pRecordsetHis->AddNew();
			m_pRecordsetHis->PutCollect("BoxID",_variant_t((long)BoxID));
			m_pRecordsetHis->PutCollect("BoxIndex",_variant_t((long)(BoxIndex)));
			m_pRecordsetHis->PutCollect("DataID",_variant_t((long)ID));
			m_pRecordsetHis->PutCollect("DataType",_variant_t((long)Type));
			m_pRecordsetHis->PutCollect("DataHour",_variant_t((long)Hour));
			m_pRecordsetHis->PutCollect("DataMinute",_variant_t((long)Minute));
			m_pRecordsetHis->PutCollect("DataValue",_variant_t((float)Value));
			m_pRecordsetHis->PutCollect("ValueType",_variant_t((long)valuetype));

			m_pRecordsetHis->Update();
			m_pRecordsetHis->Close();
			m_pConnectionHis->CommitTrans();
		}
		else
		{
			m_pRecordsetHis->Delete(adAffectCurrent);
			m_pRecordsetHis->AddNew();
			m_pRecordsetHis->PutCollect("BoxID",_variant_t((long)BoxID));
			m_pRecordsetHis->PutCollect("BoxIndex",_variant_t((long)(BoxIndex)));
			m_pRecordsetHis->PutCollect("DataID",_variant_t((long)ID));
			m_pRecordsetHis->PutCollect("DataType",_variant_t((long)Type));
			m_pRecordsetHis->PutCollect("DataHour",_variant_t((long)Hour));
			m_pRecordsetHis->PutCollect("DataMinute",_variant_t((long)Minute));
			m_pRecordsetHis->PutCollect("DataValue",_variant_t((float)Value));
			m_pRecordsetHis->PutCollect("ValueType",_variant_t((long)valuetype)); 
			m_pRecordsetHis->Update();
			m_pRecordsetHis->Close();
			m_pConnectionHis->CommitTrans();
		}
	}
	catch(...)
	{
		m_pConnectionHis->RollbackTrans();
		return false;
	}

	if(m_pRecordsetHis->State)
		m_pRecordsetHis->Close();



	return true;
}

bool SaveReportDataYM(int Type,int ID,int Year,int Month,int Day,int Hour,int Minute,float Value,int ValueType)
{
	if(m_pConnectionHis->State != adStateOpen) 
	{
		m_pConnectionHis->Close();
		if(m_pRecordsetHis->State)
			m_pRecordsetHis->Close();
		OpenDB(2);
	}
	CString strSql,strTable;
	strTable.Format("T_CUR_%d_%d",Type,ID);
	VARIANT_BOOL bEOF=true;
	HRESULT hr;

	if(m_pRecordsetHis->State)
		m_pRecordsetHis->Close();
	COleVariant v;
	V_VT(&v) = VT_DISPATCH;
	V_DISPATCH(&v) = (IDispatch*)m_pConnectionHis;
	V_DISPATCH(&v)->AddRef(); 
	strSql.Format("select * from %s where ValueType=%d and Year=%d and Month=%d and Day=%d and Hour=%d and Minute=%d",strTable,ValueType,Year-2000,Month,Day,Hour,Minute);
	try{
		hr=m_pRecordsetHis->Open((_variant_t)strSql,v, adOpenDynamic, adLockOptimistic, adCmdUnknown);
		if(SUCCEEDED(hr))
		{
		} 
		else
		{
			return false;
		}
	}
	catch(...)
	{
		m_pConnectionHis->Close();
		if(m_pRecordsetHis->State)
			m_pRecordsetHis->Close();
		OpenDB(2);
		return false;
	}

	try
	{
		m_pRecordsetHis->MoveFirst();
		m_pRecordsetHis->get_EndOfFile(&bEOF) ;
	}
	catch(...)
	{
	}

	m_pConnectionHis->BeginTrans();

	try{	
		if(bEOF)
		{
			m_pRecordsetHis->AddNew();
			m_pRecordsetHis->PutCollect("ValueType",_variant_t((long)ValueType));
			m_pRecordsetHis->PutCollect("Year",_variant_t((long)(Year-2000)));
			m_pRecordsetHis->PutCollect("Month",_variant_t((long)Month));
			m_pRecordsetHis->PutCollect("Day",_variant_t((long)Day));
			m_pRecordsetHis->PutCollect("Hour",_variant_t((long)Hour));
			m_pRecordsetHis->PutCollect("Minute",_variant_t((long)Minute));
			m_pRecordsetHis->PutCollect("Value",_variant_t((float)Value));
			m_pRecordsetHis->Update();
			m_pRecordsetHis->Close();
			m_pConnectionHis->CommitTrans();
		}
		else
		{
			m_pRecordsetHis->Delete(adAffectCurrent);
			m_pRecordsetHis->AddNew();
			m_pRecordsetHis->PutCollect("ValueType",_variant_t((long)ValueType));
			m_pRecordsetHis->PutCollect("Year",_variant_t((long)(Year-2000)));
			m_pRecordsetHis->PutCollect("Month",_variant_t((long)Month));
			m_pRecordsetHis->PutCollect("Day",_variant_t((long)Day));
			m_pRecordsetHis->PutCollect("Hour",_variant_t((long)Hour));
			m_pRecordsetHis->PutCollect("Minute",_variant_t((long)Minute));
			m_pRecordsetHis->PutCollect("Value",_variant_t((float)Value));
			m_pRecordsetHis->Update();
			m_pRecordsetHis->Close();
			m_pConnectionHis->CommitTrans();
		}
	}
	catch(...)
	{
		m_pConnectionHis->RollbackTrans();
	}

	if(m_pRecordsetHis->State)
		m_pRecordsetHis->Close();

	return true;
}

//bool SaveReportStatData(int Type,int ID,int Year,int Month,int Day,int StatInGroupID,int TimeBoundID,float Value)
//{
//	if(m_pConnectionHis->State != adStateOpen) 
//	{
//		m_pConnectionHis->Close();
//		if(m_pRecordsetHis->State)
//			m_pRecordsetHis->Close();
//		OpenDB(2);
//	}
//    CString strSql,strTable;
//	strTable.Format("T_STAT_%d_%d",Type,ID);
//	VARIANT_BOOL bEOF=true;
//	HRESULT hr;
//	bool ErrorSign=0;
//	
//	if(m_pRecordsetHis->State)
//		m_pRecordsetHis->Close();
//	COleVariant v;
//	V_VT(&v) = VT_DISPATCH;
//	V_DISPATCH(&v) = (IDispatch*)m_pConnectionHis;
//	V_DISPATCH(&v)->AddRef(); 
//	strSql.Format("select * from %s where Year=%d and Month=%d and Day=%d and StatInGroupId=%d and TimeBoundId=%d",strTable,Year-2000,Month,Day,StatInGroupID,TimeBoundID);
//	try{
//		hr=m_pRecordsetHis->Open((_variant_t)strSql,v, adOpenDynamic, adLockOptimistic, adCmdUnknown);
//		if(SUCCEEDED(hr))
//		{
//		} 
//		else
//		{
//			return false;
//		}
//	}
//	catch(...)
//	{
//		m_pConnectionHis->Close();
//		if(m_pRecordsetHis->State)
//			m_pRecordsetHis->Close();
//		OpenDB(2);
//		return false;
//	}
//		
//	try
//	{
//		m_pRecordsetHis->MoveFirst();
//		m_pRecordsetHis->get_EndOfFile(&bEOF) ;
//	}
//	catch(...)
//	{
//	}
//			
//	m_pConnectionHis->BeginTrans();
//	
//	try{	
//		if(bEOF)
//		{
//			m_pRecordsetHis->AddNew();
//			m_pRecordsetHis->PutCollect("Year",_variant_t((long)(Year-2000)));
//			m_pRecordsetHis->PutCollect("Month",_variant_t((long)Month));
//			m_pRecordsetHis->PutCollect("Day",_variant_t((long)Day));
//			m_pRecordsetHis->PutCollect("StatInGroupId",_variant_t((long)StatInGroupID));
//			m_pRecordsetHis->PutCollect("TimeBoundId",_variant_t((long)TimeBoundID));
//			m_pRecordsetHis->PutCollect("Value",_variant_t((float)Value));
//			m_pRecordsetHis->Update();
//			m_pRecordsetHis->Close();
//			m_pConnectionHis->CommitTrans();
//		}
//		else
//		{
//			m_pRecordsetHis->Delete(adAffectCurrent);
//			m_pRecordsetHis->AddNew();
//			m_pRecordsetHis->PutCollect("Year",_variant_t((long)(Year-2000)));
//			m_pRecordsetHis->PutCollect("Month",_variant_t((long)Month));
//			m_pRecordsetHis->PutCollect("Day",_variant_t((long)Day));
//			m_pRecordsetHis->PutCollect("StatInGroupId",_variant_t((long)StatInGroupID));
//			m_pRecordsetHis->PutCollect("TimeBoundId",_variant_t((long)TimeBoundID));
//			m_pRecordsetHis->PutCollect("Value",_variant_t((float)Value));
//			m_pRecordsetHis->Update();
//			m_pRecordsetHis->Close();
//			m_pConnectionHis->CommitTrans();
//		}
//	}
//	catch(...)
//	{
//		m_pConnectionHis->RollbackTrans();
//	}
//				
//	if(m_pRecordsetHis->State)
//		m_pRecordsetHis->Close();
//
//    return true;
//}


BOOL GetEvent(bool Sign)
{
	int nDataCount;
	int iPos=m_iEventPos;
	VARIANT pvDataArray;
	try
	{
		m_IRtDB->GetSOE(&m_iEventPos, &nDataCount, &pvDataArray);	
	}
	catch(...)
	{
		return 0;
	}
	if((iPos!=(int)m_iEventPos)&&m_iEventPos!=0&&Sign)
	{
		if(nDataCount>EVENT_NUM) nDataCount=EVENT_NUM;

		DcapSOEData * pData = (DcapSOEData *)pvDataArray.parray->pvData;
		memcpy(m_pSoeData,pData,sizeof(DcapSOEData)*nDataCount);

		m_iSoeSum=nDataCount;
		VariantClear(&pvDataArray);		
		return 1;
	}
	VariantClear(&pvDataArray);
	return 0;
}

void ClearEvent()
{	
	memset(m_pSoeData,0,sizeof(DcapSOEData)*EVENT_NUM);
	m_iSoeSum=0;
}

bool SaveRpt()
{

	CTime tm;
	int j = 0;
	int savenum=0;
	tm = CTime::GetCurrentTime();
	bool bResult=false;
	CString sTemp;
	InitReportTable();

	RefreshRealData();

	int i  = 0;
	for(i = 0;i<m_icfgycnum;i++)  //遥测
	{
		bResult = false;

		if(m_pRealYc[i].IsFirstSave==0)//不是第一次存盘

		{

			CTimeSpan TmInterval = tm - m_pRealYc[i].lastsavetime;

			LONG minutes = TmInterval.GetHours()*60+TmInterval.GetMinutes();

			if(minutes == m_pRealYc[i].saveinterval)
			{
				if(!m_pRealYc[i].IsSave)
				{
					m_pRealYc[i].IsSave = true;

					bResult = SaveReportData(m_pRealYc[i].BoxID,
						m_pRealYc[i].BoxIndex,
						m_pRealYc[i].DataID,
						1,
						tm.GetYear(),
						tm.GetMonth(),
						tm.GetDay(),
						tm.GetHour(),
						tm.GetMinute(),
						(float)m_pRealYc[i].Value,0);

					m_pRealYc[i].lastsavetime = CTime(tm.GetYear(),tm.GetMonth(),tm.GetDay(),tm.GetHour(),tm.GetMinute(),0);

					sTemp.Format("%d年%02d月%02d日%02d时%02d分%02d秒保存遥测报表数据:数据ID=%d,装置ID=%d,装置内序号=%d",
						tm.GetYear(),
						tm.GetMonth(),
						tm.GetDay(),
						tm.GetHour(),
						tm.GetMinute(),
						tm.GetSecond(),	
						m_pRealYc[i].DataID,
						m_pRealYc[i].BoxID,
						m_pRealYc[i].BoxIndex);

				}
			}
			else
			{
				m_pRealYc[i].IsSave = false;
			}


		}
		else//第一次存盘
		{

			if(tm.GetMinute()%m_pRealYc[i].saveinterval==0)
			{
				m_pRealYc[i].IsFirstSave = false;
				if(!m_pRealYc[i].IsSave)
				{
					m_pRealYc[i].IsSave = true;	
					bResult = SaveReportData(m_pRealYc[i].BoxID,
						m_pRealYc[i].BoxIndex,
						m_pRealYc[i].DataID,
						1,
						tm.GetYear(),
						tm.GetMonth(),
						tm.GetDay(),
						tm.GetHour(),
						tm.GetMinute(),
						(float)m_pRealYc[i].Value,0);


					sTemp.Format("%d年%02d月%02d日%02d时%02d分%02d秒保存遥测报表数据:数据ID=%d,装置ID=%d,装置内序号=%d",
						tm.GetYear(),
						tm.GetMonth(),
						tm.GetDay(),
						tm.GetHour(),
						tm.GetMinute(),
						tm.GetSecond(),

						m_pRealYc[i].DataID,
						m_pRealYc[i].BoxID,
						m_pRealYc[i].BoxIndex);

					
					m_pRealYc[i].lastsavetime = CTime(tm.GetYear(),tm.GetMonth(),tm.GetDay(),tm.GetHour(),tm.GetMinute(),0);
				}	
			}
			else
			{
				m_pRealYc[i].IsSave = false;

			}


		}


	}


	for( i = 0;i<m_icfgymnum;i++)  //遥脉
	{
		bResult = false;

		if(m_pRealYm[i].IsFirstSave==0)//不是第一次存盘

		{
			CTimeSpan TmInterval = tm - m_pRealYm[i].lastsavetime;
			if(TmInterval.GetHours()*60+TmInterval.GetMinutes()==m_pRealYm[i].saveinterval)
			{
				if(!m_pRealYm[i].IsSave)
				{
					m_pRealYm[i].IsSave = true;

					bResult = SaveReportData(m_pRealYm[i].BoxID,
						m_pRealYm[i].BoxIndex,
						m_pRealYm[i].DataID,
						2,
						tm.GetYear(),
						tm.GetMonth(),
						tm.GetDay(),
						tm.GetHour(),
						tm.GetMinute(),
						(float)m_pRealYm[i].Value,1);//表底


					sTemp.Format("%d年%02d月%02d日%02d时%02d分%02d秒保存遥脉报表数据(表底):数据ID=%d,装置ID=%d,装置内序号=%d",
						tm.GetYear(),
						tm.GetMonth(),
						tm.GetDay(),
						tm.GetHour(),
						tm.GetMinute(),
						tm.GetSecond(),	
						m_pRealYm[i].DataID,
						m_pRealYm[i].BoxID,
						m_pRealYm[i].BoxIndex);




					bResult = SaveReportData(m_pRealYm[i].BoxID,
						m_pRealYm[i].BoxIndex,
						m_pRealYm[i].DataID,
						2,
						tm.GetYear(),
						tm.GetMonth(),
						tm.GetDay(),
						tm.GetHour(),
						tm.GetMinute(),
						(float)(m_pRealYm[i].Value-m_pRealYm[i].lastvalue),0);//电度

					sTemp.Format("%d年%02d月%02d日%02d时%02d分%02d秒保存遥脉报表数据(电度):数据ID=%d,装置ID=%d,装置内序号=%d",
						tm.GetYear(),
						tm.GetMonth(),
						tm.GetDay(),
						tm.GetHour(),
						tm.GetMinute(),
						tm.GetSecond(),	
						m_pRealYm[i].DataID,
						m_pRealYm[i].BoxID,
						m_pRealYm[i].BoxIndex);


					m_pRealYm[i].lastvalue = m_pRealYm[i].Value;
					m_pRealYm[i].lastsavetime = CTime(tm.GetYear(),tm.GetMonth(),tm.GetDay(),tm.GetHour(),tm.GetMinute(),0);

				}


			}
			else
			{
				m_pRealYm[i].IsSave = false;
			}


		}
		else//第一次存盘
		{

			if(tm.GetMinute()%m_pRealYm[i].saveinterval==0)
			{
				if(!m_pRealYm[i].IsSave)
				{
					m_pRealYm[i].IsFirstSave = false;
					m_pRealYm[i].IsSave = true;
					bResult = SaveReportData(m_pRealYm[i].BoxID,
						m_pRealYm[i].BoxIndex,
						m_pRealYm[i].DataID,
						2,
						tm.GetYear(),
						tm.GetMonth(),
						tm.GetDay(),
						tm.GetHour(),
						tm.GetMinute(),
						(float)m_pRealYm[i].Value,1);


					sTemp.Format("%d年%02d月%02d日%02d时%02d分%02d秒保存遥脉报表数据(表底):数据ID=%d,装置ID=%d,装置内序号=%d",
						tm.GetYear(),
						tm.GetMonth(),
						tm.GetDay(),
						tm.GetHour(),
						tm.GetMinute(),
						tm.GetSecond(),	
						m_pRealYm[i].DataID,
						m_pRealYm[i].BoxID,
						m_pRealYm[i].BoxIndex);



					m_pRealYm[i].lastvalue = m_pRealYm[i].Value;

					bResult = SaveReportData(m_pRealYm[i].BoxID,
						m_pRealYm[i].BoxIndex,
						m_pRealYm[i].DataID,
						2,
						tm.GetYear(),
						tm.GetMonth(),
						tm.GetDay(),
						tm.GetHour(),
						tm.GetMinute(),
						(float)(m_pRealYm[i].Value-m_pRealYm[i].lastvalue),0);//电度

					sTemp.Format("%d年%02d月%02d日%02d时%02d分%02d秒保存遥脉报表数据(电度):数据ID=%d,装置ID=%d,装置内序号=%d",
						tm.GetYear(),
						tm.GetMonth(),
						tm.GetDay(),
						tm.GetHour(),
						tm.GetMinute(),
						tm.GetSecond(),	
						m_pRealYm[i].DataID,
						m_pRealYm[i].BoxID,
						m_pRealYm[i].BoxIndex);




					m_pRealYm[i].lastsavetime = CTime(tm.GetYear(),tm.GetMonth(),tm.GetDay(),tm.GetHour(),tm.GetMinute(),0);
				}	
			}
			else
			{
				m_pRealYm[i].IsSave = false;
			}

		}


	}

	for( i = 0;i<m_icfgyxnum;i++)  //遥信
	{
		bResult = false;

		if(m_pRealYx[i].IsFirstSave==0)//不是第一次存盘

		{
			CTimeSpan TmInterval = tm - m_pRealYx[i].lastsavetime;
			if(TmInterval.GetHours()*60+TmInterval.GetMinutes()==m_pRealYx[i].saveinterval)
			{	if(!m_pRealYx[i].IsSave)
			{
				m_pRealYx[i].IsSave = true;
				bResult = SaveReportData(m_pRealYx[i].BoxID,
					m_pRealYx[i].BoxIndex,
					m_pRealYx[i].DataID,
					3,
					tm.GetYear(),
					tm.GetMonth(),
					tm.GetDay(),
					tm.GetHour(),
					tm.GetMinute(),
					(float)m_pRealYx[i].Value,0);


				sTemp.Format("%d年%02d月%02d日%02d时%02d分%02d秒保存报表数据:数据ID=%d,装置ID=%d,装置内序号=%d",
					tm.GetYear(),
					tm.GetMonth(),
					tm.GetDay(),
					tm.GetHour(),
					tm.GetMinute(),
					tm.GetSecond(),	
					m_pRealYx[i].DataID,
					m_pRealYx[i].BoxID,
					m_pRealYx[i].BoxIndex);



				m_pRealYx[i].lastsavetime = CTime(tm.GetYear(),tm.GetMonth(),tm.GetDay(),tm.GetHour(),tm.GetMinute(),0);
			}

			}
			else
			{
				m_pRealYx[i].IsSave = false;
			}


		}
		else//第一次存盘
		{

			if(tm.GetMinute()%m_pRealYx[i].saveinterval==0)
			{
				m_pRealYx[i].IsFirstSave = false;

				if(!m_pRealYx[i].IsSave)
				{
					m_pRealYx[i].IsSave = true;
					bResult = SaveReportData(m_pRealYx[i].BoxID,
						m_pRealYx[i].BoxIndex,
						m_pRealYx[i].DataID,
						3,
						tm.GetYear(),
						tm.GetMonth(),
						tm.GetDay(),
						tm.GetHour(),
						tm.GetMinute(),
						(float)m_pRealYx[i].Value,0);


					sTemp.Format("%d年%02d月%02d日%02d时%02d分%02d秒保存报表数据:数据ID=%d,装置ID=%d,装置内序号=%d",
						tm.GetYear(),
						tm.GetMonth(),
						tm.GetDay(),
						tm.GetHour(),
						tm.GetMinute(),
						tm.GetSecond(),	
						m_pRealYx[i].DataID,
						m_pRealYx[i].BoxID,
						m_pRealYx[i].BoxIndex);

					m_pRealYx[i].lastsavetime = CTime(tm.GetYear(),tm.GetMonth(),tm.GetDay(),tm.GetHour(),tm.GetMinute(),0);
				}	
			}
			else
			{
				m_pRealYx[i].IsSave = false;
			}

		}


	}

	for( i = 0;i<m_icfggznum;i++)  //遥信
	{
		bResult = false;

		if(m_pRealGz[i].IsFirstSave==0)//不是第一次存盘

		{
			CTimeSpan TmInterval = tm - m_pRealGz[i].lastsavetime;
			if(TmInterval.GetHours()*60+TmInterval.GetMinutes()==m_pRealGz[i].saveinterval)
			{	if(!m_pRealGz[i].IsSave)
			{
				m_pRealGz[i].IsSave = true;
				bResult = SaveReportData(m_pRealGz[i].BoxID,
					m_pRealGz[i].BoxIndex,
					m_pRealGz[i].DataID,
					8,
					tm.GetYear(),
					tm.GetMonth(),
					tm.GetDay(),
					tm.GetHour(),
					tm.GetMinute(),
					(float)m_pRealGz[i].Value,0);


				sTemp.Format("%d年%02d月%02d日%02d时%02d分%02d秒保存报表数据:数据ID=%d,装置ID=%d,装置内序号=%d",
					tm.GetYear(),
					tm.GetMonth(),
					tm.GetDay(),
					tm.GetHour(),
					tm.GetMinute(),
					tm.GetSecond(),	
					m_pRealGz[i].DataID,
					m_pRealGz[i].BoxID,
					m_pRealGz[i].BoxIndex);


				m_pRealGz[i].lastsavetime = CTime(tm.GetYear(),tm.GetMonth(),tm.GetDay(),tm.GetHour(),tm.GetMinute(),0);
			}

			}
			else
			{
				m_pRealGz[i].IsSave = false;
			}


		}
		else//第一次存盘
		{

			if(tm.GetMinute()%m_pRealGz[i].saveinterval==0)
			{
				m_pRealGz[i].IsFirstSave = false;
				if(!m_pRealGz[i].IsSave)
				{


					m_pRealGz[i].IsSave = true;
					bResult = SaveReportData(m_pRealGz[i].BoxID,
						m_pRealGz[i].BoxIndex,
						m_pRealGz[i].DataID,
						8,
						tm.GetYear(),
						tm.GetMonth(),
						tm.GetDay(),
						tm.GetHour(),
						tm.GetMinute(),
						(float)m_pRealGz[i].Value,0);


					sTemp.Format("%d年%02d月%02d日%02d时%02d分%02d秒保存报表数据:数据ID=%d,装置ID=%d,装置内序号=%d",
						tm.GetYear(),
						tm.GetMonth(),
						tm.GetDay(),
						tm.GetHour(),
						tm.GetMinute(),
						tm.GetSecond(),	
						m_pRealGz[i].DataID,
						m_pRealGz[i].BoxID,
						m_pRealGz[i].BoxIndex);


					m_pRealGz[i].lastsavetime = CTime(tm.GetYear(),tm.GetMonth(),tm.GetDay(),tm.GetHour(),tm.GetMinute(),0);
				}	
			}
			else
			{
				m_pRealGz[i].IsSave = false;
			}

		}


	}

	return true;
}

UINT  ReportSaveProcess(LPVOID lParam)
{
	::CoInitialize(NULL);

	while(!m_bShutDown)
	{
		SaveRpt();
		Sleep(500);
	}

	try
	{
		if (m_IRtDB != NULL)
			m_IRtDB.Release();
	}
	catch(...)
	{
	}

	::CoUninitialize();

	return 1;
}

bool InitRealDataBuf()
{
	if(m_icfgycnum>0)
	{

		m_pRealYc = new NewDcapData[m_icfgycnum];
		memset(m_pRealYc,0,sizeof(NewDcapData)*m_icfgycnum);

	}


	if(m_icfgymnum>0)
	{

		m_pRealYm = new NewDcapData[m_icfgymnum];
		memset(m_pRealYm,0,sizeof(NewDcapData)*m_icfgymnum);

	}
	if(m_icfgyxnum>0)
	{

		m_pRealYx = new NewDcapData[m_icfgyxnum];
		memset(m_pRealYx,0,sizeof(NewDcapData)*m_icfgyxnum);

	}

	if(m_icfggznum>0)
	{

		m_pRealGz = new NewDcapData[m_icfggznum];
		memset(m_pRealGz,0,sizeof(NewDcapData)*m_icfggznum);

	}

	COleVariant pvDataArray;
	VariantClear(&pvDataArray);
	int nCount;
	try{

		m_IRtDB->GetRuntimeValue(&nCount,&pvDataArray);
	}
	catch(_com_error e)
	{
		CString str(e.ErrorMessage());
		return false;	
	}
	DcapData *pData = (DcapData*)pvDataArray.parray->pvData;
	int ycn=0,ymn=0,yxn=0,gzn=0;
	for(int i = 0;i<m_iDataInfoSum;i++)
	{
		switch(m_pDataInfo[i].DataType)
		{
		case 1:
			m_pRealYc[ycn].BoxID = m_pDataInfo[i].BoxID;
			m_pRealYc[ycn].BoxIndex = m_pDataInfo[i].BoxIndex;
			m_pRealYc[ycn].DataID = m_pDataInfo[i].DataId;
			m_pRealYc[ycn].TableType = 1;
			m_pRealYc[ycn].IsFirstSave = true;
			m_pRealYc[ycn].saveinterval = atoi(m_pDataInfo[i].Param1);
			ycn++;
			break;
		case 2:

			m_pRealYm[ymn].BoxID = m_pDataInfo[i].BoxID;
			m_pRealYm[ymn].BoxIndex = m_pDataInfo[i].BoxIndex;
			m_pRealYm[ymn].DataID = m_pDataInfo[i].DataId;
			m_pRealYm[ymn].TableType = 2;
			m_pRealYm[ymn].IsFirstSave = true;
			m_pRealYm[ymn].saveinterval = atoi(m_pDataInfo[i].Param1);
			ymn++;
			break;
		case 3:
			m_pRealYx[yxn].BoxID = m_pDataInfo[i].BoxID;
			m_pRealYx[yxn].BoxIndex = m_pDataInfo[i].BoxIndex;
			m_pRealYx[yxn].DataID = m_pDataInfo[i].DataId;
			m_pRealYx[yxn].TableType = 3;
			m_pRealYx[yxn].IsFirstSave = true;
			m_pRealYx[yxn].saveinterval = atoi(m_pDataInfo[i].Param1);
			yxn++;
			break;
		case 8:
			m_pRealGz[gzn].BoxID = m_pDataInfo[i].BoxID;
			m_pRealGz[gzn].BoxIndex = m_pDataInfo[i].BoxIndex;
			m_pRealGz[gzn].DataID = m_pDataInfo[i].DataId;
			m_pRealGz[gzn].TableType = 8;
			m_pRealGz[gzn].IsFirstSave = true;
			m_pRealGz[gzn].saveinterval = atoi(m_pDataInfo[i].Param1);
			gzn++;

			break;
		}

	}

	for(int k=0;k<nCount;k++)
	{	int l=0;
	switch(pData[k].TableType)
	{

	case 1:
		for(l=0;l<m_icfgycnum;l++)
		{
			if((m_pRealYc[l].BoxID==pData[k].BoxID)&&(m_pRealYc[l].BoxIndex==pData[k].BoxIndex))
			{
				m_pRealYc[l].BoxID = pData[k].BoxID;
				m_pRealYc[l].BoxIndex = pData[k].BoxIndex;
				m_pRealYc[l].SpaceID = pData[k].SpaceID;
				m_pRealYc[l].SpaceIndex = pData[k].SpaceIndex;
				m_pRealYc[l].Status = pData[k].Status;
				m_pRealYc[l].TableType = pData[k].TableType;
				m_pRealYc[l].Value = pData[k].Value;
			}

		}
		break;
	case 2:
		for(l=0;l<m_icfgymnum;l++)
		{
			if((m_pRealYm[l].BoxID==pData[k].BoxID)&&(m_pRealYm[l].BoxIndex==pData[k].BoxIndex))
			{
				m_pRealYm[l].BoxID = pData[k].BoxID;
				m_pRealYm[l].BoxIndex = pData[k].BoxIndex;
				m_pRealYm[l].SpaceID = pData[k].SpaceID;
				m_pRealYm[l].SpaceIndex = pData[k].SpaceIndex;
				m_pRealYm[l].Status = pData[k].Status;
				m_pRealYm[l].TableType = pData[k].TableType;
				m_pRealYm[l].Value = pData[k].Value;
			}
		}
		break;
	case 3:
		for(l=0;l<m_icfgyxnum;l++)
		{
			if((m_pRealYx[l].BoxID==pData[k].BoxID)&&(m_pRealYx[l].BoxIndex==pData[k].BoxIndex))
			{
				m_pRealYx[l].BoxID = pData[k].BoxID;
				m_pRealYx[l].BoxIndex = pData[k].BoxIndex;
				m_pRealYx[l].SpaceID = pData[k].SpaceID;
				m_pRealYx[l].SpaceIndex = pData[k].SpaceIndex;
				m_pRealYx[l].Status = pData[k].Status;
				m_pRealYx[l].TableType = pData[k].TableType;
				m_pRealYx[l].Value = pData[k].Value;
			}
		}
		break;

	case 4:
		for(l=0;l<m_icfggznum;l++)
		{
			if((m_pRealGz[l].BoxID==pData[k].BoxID)&&(m_pRealGz[l].BoxIndex==pData[k].BoxIndex))
			{
				m_pRealGz[l].BoxID = pData[k].BoxID;
				m_pRealGz[l].BoxIndex = pData[k].BoxIndex;
				m_pRealGz[l].SpaceID = pData[k].SpaceID;
				m_pRealGz[l].SpaceIndex = pData[k].SpaceIndex;
				m_pRealGz[l].Status = pData[k].Status;
				m_pRealGz[l].TableType = 8;
				m_pRealGz[l].Value = pData[k].Value;
			}
		}

		break;

	}
	}

	VariantClear(&pvDataArray);


	return true;

}

bool RefreshRealData()
{

	COleVariant pvDataArray;
	VariantClear(&pvDataArray);
	int nCount;
	try{

		m_IRtDB->GetRuntimeValue(&nCount,&pvDataArray);
	}
	catch(_com_error e)
	{
		CString str(e.ErrorMessage());
		return false;
	}
	DcapData *pData = (DcapData*)pvDataArray.parray->pvData;
	int ycn=0,ymn=0,yxn=0;

	for(int k=0;k<nCount;k++)
	{	int l=0;
	switch(pData[k].TableType)
	{

	case 1:
		for(l=0;l<m_icfgycnum;l++)
		{
			if((m_pRealYc[l].BoxID==pData[k].BoxID)&&(m_pRealYc[l].BoxIndex==pData[k].BoxIndex))
			{
				m_pRealYc[l].BoxID = pData[k].BoxID;
				m_pRealYc[l].BoxIndex = pData[k].BoxIndex;
				m_pRealYc[l].SpaceID = pData[k].SpaceID;
				m_pRealYc[l].SpaceIndex = pData[k].SpaceIndex;
				m_pRealYc[l].Status = pData[k].Status;
				m_pRealYc[l].TableType = pData[k].TableType;
				m_pRealYc[l].Value = pData[k].Value;
			}

		}
		break;
	case 2:
		for(l=0;l<m_icfgymnum;l++)
		{
			if((m_pRealYm[l].BoxID==pData[k].BoxID)&&(m_pRealYm[l].BoxIndex==pData[k].BoxIndex))
			{
				m_pRealYm[l].BoxID = pData[k].BoxID;
				m_pRealYm[l].BoxIndex = pData[k].BoxIndex;
				m_pRealYm[l].SpaceID = pData[k].SpaceID;
				m_pRealYm[l].SpaceIndex = pData[k].SpaceIndex;
				m_pRealYm[l].Status = pData[k].Status;
				m_pRealYm[l].TableType = pData[k].TableType;
				m_pRealYm[l].Value = pData[k].Value;
			}
		}
		break;
	case 3:
		for(l=0;l<m_icfgyxnum;l++)
		{
			if((m_pRealYx[l].BoxID==pData[k].BoxID)&&(m_pRealYx[l].BoxIndex==pData[k].BoxIndex))
			{
				m_pRealYx[l].BoxID = pData[k].BoxID;
				m_pRealYx[l].BoxIndex = pData[k].BoxIndex;
				m_pRealYx[l].SpaceID = pData[k].SpaceID;
				m_pRealYx[l].SpaceIndex = pData[k].SpaceIndex;
				m_pRealYx[l].Status = pData[k].Status;
				m_pRealYx[l].TableType = pData[k].TableType;
				m_pRealYx[l].Value = pData[k].Value;
			}
		}
		break;
	case 4:
		for(l=0;l<m_icfggznum;l++)
		{
			if((m_pRealGz[l].BoxID==pData[k].BoxID)&&(m_pRealGz[l].BoxIndex==pData[k].BoxIndex))
			{
				m_pRealGz[l].BoxID = pData[k].BoxID;
				m_pRealGz[l].BoxIndex = pData[k].BoxIndex;
				m_pRealGz[l].SpaceID = pData[k].SpaceID;
				m_pRealGz[l].SpaceIndex = pData[k].SpaceIndex;
				m_pRealGz[l].Status = pData[k].Status;
				m_pRealGz[l].TableType = 8;
				m_pRealGz[l].Value = pData[k].Value;
			}
		}

		break;
	}
	}

	VariantClear(&pvDataArray);
	return true;
}
extern "C" void __stdcall WaitExit()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_bShutDown = 1;
	if (m_pCommThread != NULL)
	{
		WaitForSingleObject(m_pCommThread->m_hThread,INFINITE);

		m_pCommThread = NULL;
	}

}
extern "C" int __stdcall  Start(HANDLE h)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(!InitDB())
	{
		AfxMessageBox("报表模块初始化数据库出错!");
		return 0;
	}

	if(!ConnectToRtdb())
		return 0;

	if (m_icfgycnum>0 || 
		m_icfgymnum>0 ||
		m_icfgyxnum>0 ||
		m_icfggznum)
	{

		InitRealDataBuf();
		m_pCommThread=::AfxBeginThread(ReportSaveProcess,NULL,THREAD_PRIORITY_NORMAL,0,0,NULL);

	}
	return 1;
}


