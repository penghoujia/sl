// ConfigFile.h: interface for the CConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGFILE_H__BE2A0060_0CE7_11D5_8DDB_00404500215B__INCLUDED_)
#define AFX_CONFIGFILE_H__BE2A0060_0CE7_11D5_8DDB_00404500215B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define	FILELENGTHMAX		262144
#define	SECTIONLENGTHMAX	16384
#define	NAMELENGTHMAX		64
#define	STRINGLENGTHMAX		256


class CConfigFile
{
public:
	CConfigFile ();
	CConfigFile (LPCTSTR filename, BOOL Write=FALSE);

	~CConfigFile ();

private:
	char *BufFile;
	CString m_FileName;
	BOOL ReadOnly;
	BOOL IsOpened;
	BOOL FileChanged;

	char *BufSection;
	char SectionName[NAMELENGTHMAX+3];
	BOOL SectionChanged;

	char s1[NAMELENGTHMAX+20],s2[NAMELENGTHMAX+20];

	BOOL  LoadFile ();
	BOOL  SaveFile ();
	char* NextLine (char* buf);
	char* NextValue (char* buf);
	char* NextSection (char* buf);
	DWORD atodw (char* str);

public:
	BOOL IsFileOpened ();

	BOOL Open (LPCTSTR filename, BOOL Write=FALSE);
	void Close ( );

	BOOL Clear ();

	BOOL OpenSection (LPCTSTR name);
	BOOL CloseSection ();
	BOOL ClearSection ();

	BOOL GetSectionName (CString& str, int& nStart);
	BOOL FormatNameString (CString& strName);

	BOOL WriteBlankLine ();
	BOOL WriteInt (LPCTSTR name, int value);
	BOOL WriteInts (LPCTSTR name, int* value, int n);
	BOOL WriteDWORD (LPCTSTR name, DWORD value);
	BOOL WriteDWORDs (LPCTSTR name, DWORD* value, int n);
	BOOL WriteString (LPCTSTR name, LPCTSTR str);

	BOOL ReadInt (LPCTSTR name, int* value);
	BOOL ReadInt (LPCTSTR name, int* value, int default_data);
	BOOL ReadInts (LPCTSTR name, int* value, int n);
	BOOL ReadInts (LPCTSTR name, int* value, int n, int default_data);
	BOOL ReadDWORD (LPCTSTR name, DWORD* value);
	BOOL ReadDWORD (LPCTSTR name, DWORD* value, DWORD default_value);
	BOOL ReadDWORDs (LPCTSTR name, DWORD* value, int n);
	BOOL ReadDWORDs (LPCTSTR name, DWORD* value, int n, DWORD default_value);
	BOOL ReadString (LPCTSTR name, LPTSTR str, int nMaxBytes, int nIndex=0, char Delimiters=0);
	BOOL ReadString (LPCTSTR name, CString& str, int nIndex=0, char Delimiters=0);
};

#endif // !defined(AFX_CONFIGFILE_H__BE2A0060_0CE7_11D5_8DDB_00404500215B__INCLUDED_)
