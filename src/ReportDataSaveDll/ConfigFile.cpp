// ConfigFile.cpp: implementation of the CConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConfigFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigFile::CConfigFile()
{
	IsOpened=FALSE;
	SectionChanged=FALSE;
	FileChanged=FALSE;
	BufFile=NULL;
	BufSection=NULL;
}

CConfigFile::~CConfigFile()
{
	if(IsOpened) Close();
}

CConfigFile::CConfigFile(LPCTSTR filename, BOOL Write)
{
	IsOpened=FALSE;
	Open(filename,Write);
}


BOOL CConfigFile::Open(LPCTSTR filename, BOOL Write)
{
	if(IsOpened) Close();

	IsOpened=FALSE;
	SectionChanged=FALSE;
	FileChanged=FALSE;

	BufFile=new char[FILELENGTHMAX+16];
	BufSection=new char[SECTIONLENGTHMAX+16];

	if(BufFile && BufSection)
	{
		m_FileName=filename;
		ReadOnly=!Write;
		LoadFile();
	}
	return IsOpened;
}

void CConfigFile::Close()
{
	if(FileChanged) SaveFile();
	IsOpened=FALSE;
	if(BufFile) delete[] BufFile;
	if(BufSection) delete[] BufSection;
}


BOOL CConfigFile::LoadFile()
{
	BufFile[0]=0;
	BufSection[0]=0;

	UINT nOpenFlag;
	if(ReadOnly) nOpenFlag=CFile::modeRead|CFile::typeText;
	else nOpenFlag=CFile::modeNoTruncate|CFile::modeReadWrite|CFile::modeCreate|CFile::typeText;
	CStdioFile file;
	if(file.Open(m_FileName,nOpenFlag))
	{
		DWORD fl=file.GetLength();
		if(fl>FILELENGTHMAX) fl=FILELENGTHMAX;
		if(fl) fl=file.Read(BufFile,fl);
		BufFile[fl]=0;
		file.Close();
		IsOpened=TRUE;
	}
	else
	{
		IsOpened=FALSE;
	}
	SectionChanged=FALSE;
	FileChanged=FALSE;

	return IsOpened;
}


BOOL CConfigFile::IsFileOpened()
{
	return IsOpened;
}


BOOL CConfigFile::Clear()
{
	if(!IsOpened) return FALSE;
	if(ReadOnly) return FALSE;

	if(BufFile[0])	FileChanged=TRUE;
	BufFile[0]=0;
	BufSection[0]=0;
	SectionChanged=FALSE;
//	FileChanged=FALSE;

	return TRUE;
}


BOOL CConfigFile::SaveFile()
{
	if(!IsOpened) return FALSE;
	if(ReadOnly) return FALSE;
	if(!FileChanged) return TRUE;

	if(SectionChanged) CloseSection();

	CStdioFile file;
	if(file.Open(m_FileName,CFile::modeWrite|CFile::modeCreate|CFile::typeText))
	{
		file.WriteString(BufFile);
		file.Close();

		FileChanged=FALSE;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


BOOL CConfigFile::GetSectionName(CString& strName, int& nStart)
{
	if(!IsOpened) return FALSE;

	int j,L=strlen(BufFile);
	char ch,name[NAMELENGTHMAX+2]="";
	strName="";

	for(;nStart<L;)
	{
		if(nStart)
		{
			while(BufFile[nStart-1]!='\n')
			{
				nStart++;
				if(nStart>=L) break;
			}
		}
		if(nStart>=L) break;
		nStart++;
		if(BufFile[nStart-1]!='[') continue;
		for(j=0; j<NAMELENGTHMAX+1; j++,nStart++)
		{
			ch=BufFile[nStart];
			if(ch==']')
			{
				break;
			}
			else if(!ch || ch=='\n')
			{
				name[0]=0;
				break;
			}
			else
				name[j]=ch;
		}
		name[j]=0;
		if(ch==']')
		{
			strName=name;
			break;
		}
	}
	if(strName=="") return FALSE;

	return TRUE;
}


BOOL CConfigFile::FormatNameString(CString& strName)
{
	int i,l;
	l=strName.GetLength();
	for(i=0; i<l; i++) if(strName[i]=='[' || strName[i]==']') strName.SetAt(i,'_');
	strName.TrimLeft();
	strName.TrimRight();
	return !!strName.GetLength();
}


BOOL CConfigFile::OpenSection(LPCTSTR name)
{
	if(!IsOpened) return FALSE;
	if(SectionChanged) CloseSection();

	strncpy(s2,name,NAMELENGTHMAX);
	s2[NAMELENGTHMAX]=0;
	sprintf(s1,"[%s]",s2);
	strcpy(SectionName,s1);
	int sum = 0;
	char* section=strstr(BufFile,s1);
	if(section)
	{
		int i,j=NextSection(section)-section;
		if(j>SECTIONLENGTHMAX) j=SECTIONLENGTHMAX;
		section = strlwr(section);
		for(i=0; i<j; i++) 
		{
			if(section[i] != ' ')
			{
				BufSection[sum++] = section[i];
			}
		}
		BufSection[sum]=0;
	}
	else
	{
		if(!ReadOnly)
		{
			strcpy(BufSection,s1);
			strcat(BufSection,"\n");
			SectionChanged=TRUE;
			FileChanged=TRUE;
		}
		else
		{
			BufSection[0]=0;
			SectionChanged=FALSE;
			return FALSE;
		}
	}
	return TRUE;
}


BOOL CConfigFile::CloseSection()
{
	if(!IsOpened) return FALSE;
	if(!SectionChanged)
	{
		BufSection[0]=0;
		return TRUE;
	}

	int j;
	char* pCurrentSection;
	pCurrentSection=strstr(BufFile,SectionName);
	if(pCurrentSection)
	{
		j=pCurrentSection-BufFile;
		char* pNextSection=NextSection(pCurrentSection);
		while(j){if(BufFile[j-1]=='[') j--;	else break;}
		if(j>=2 && BufFile[j-2]!='\n') BufFile[j++]='\n';
		strcpy(BufFile+j,pNextSection);
	}
	j=strlen(BufFile);
	if(j>=2 && BufFile[j-2]!='\n') BufFile[j++]='\n';
	int k=j+strlen(BufSection);
	if(k>FILELENGTHMAX)	k=FILELENGTHMAX;
	strncpy(BufFile+j,BufSection,k-j);
	BufFile[k]=0;

	BufSection[0]=0;
	SectionChanged=FALSE;

	return TRUE;
}

BOOL CConfigFile::ClearSection()
{
	if(!IsOpened) return FALSE;
	if(ReadOnly) return FALSE;
	if(!BufSection[0]) return TRUE;

	sprintf(BufSection,"%s\n",SectionName);
	SectionChanged=TRUE;
	FileChanged=TRUE;
	return TRUE;
}


char* CConfigFile::NextLine(char* buf)
{
	int i;

	for(i=0; buf[i]; i++)
	{
		if(buf[i]=='\n')
		{
			i++;
			break;
		}
	}
	return buf+i;
}

char* CConfigFile::NextValue(char* buf)
{
	char* p=buf;

	for(;;)
	{
		p=NextLine(p);
		if(p[0]!='\n') break;
	}
	return p;
}

char* CConfigFile::NextSection(char* buf)
{
	char* p=buf;

	for(;;)
	{
		p=NextLine(p);
		if(!p[0] || p[0]=='[') break;
	}
	return p;
}

DWORD CConfigFile::atodw(char* str)
{
	DWORD x=0;
	for(int i=0; str[i];i++)
	{
		if(str[i]>='0' && str[i]<='9')		x=(x<<4)+(str[i]-'0');
		else if(str[i]>='A' && str[i]<='F')	x=(x<<4)+(str[i]-'A'+10);
		else if(str[i]>='a' && str[i]<='f')	x=(x<<4)+(str[i]-'a'+10);
		else break;
	}
	return x;
}


BOOL CConfigFile::ReadInt (LPCTSTR name, int* value)
{
	if(!IsOpened) return FALSE;
	if(!BufSection[0]) return FALSE;

	strncpy(s1,name,NAMELENGTHMAX);
	s1[NAMELENGTHMAX]=0;
	strcat(s1,"=");
	char* svalue=strstr(BufSection,s1);
	if(svalue)
	{
		svalue+=strlen(s1);
		while(svalue[0]==0x20 || svalue[0]=='\t') svalue++;
		int i=0;
		if(svalue[0]=='+' || svalue[0]=='-')
		{
			s2[0]=svalue[0];
			i++;
		}
		for(;i<12;i++)
		{
			if(svalue[i]>='0' && svalue[i]<='9') s2[i]=svalue[i];
			else break;
		}
		s2[i]=0;
		*value=atoi(s2);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CConfigFile::ReadInt (LPCTSTR name, int* value, int default_value)
{
	*value=default_value;
	return ReadInt (name, value);
}


BOOL CConfigFile::ReadInts (LPCTSTR name, int* value, int n)
{
	if(!IsOpened) return FALSE;
	if(!BufSection[0]) return FALSE;

	int i,j;
	if(n<1) n=1;
	if(n>32) n=32;

	strncpy(s1,name,NAMELENGTHMAX);
	s1[NAMELENGTHMAX]=0;
	strcat(s1,"=");
	char* svalue=strstr(BufSection,s1);
	if(svalue)
	{
		svalue+=strlen(s1);
		for(j=0;j<n;j++)
		{
			while(svalue[0]==0x20 || svalue[0]=='\t') svalue++;
			i=0;
			if(svalue[0]=='+' || svalue[0]=='-')
			{
				s2[0]=svalue[0];
				i++;
			}
			for(;i<12;i++)
			{
				if(svalue[i]>='0' && svalue[i]<='9') s2[i]=svalue[i];
				else break;
			}
			s2[i]=0;
			value[j]=atoi(s2);
			svalue+=i;
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CConfigFile::ReadInts (LPCTSTR name, int* value, int n, int default_value)
{
	if(n<1) n=1;
	if(n>32) n=32;
	for(int j=0;j<n;j++) value[j]=default_value;
	return ReadInts(name,value,n);
}


BOOL CConfigFile::ReadDWORD (LPCTSTR name, DWORD* value)
{
	if(!IsOpened) return FALSE;
	if(!BufSection[0]) return FALSE;

	strncpy(s1,name,NAMELENGTHMAX);
	s1[NAMELENGTHMAX]=0;
	strcat(s1,"=");
	char* svalue=strstr(BufSection,s1);
	if(svalue)
	{
		svalue+=strlen(s1);
		while(svalue[0]==0x20 || svalue[0]=='\t') 
			svalue++;

		if(svalue[0]=='0' && (svalue[1]=='x' || svalue[1]=='X')) 
			svalue+=2;

		int i = 0;
		for(i=0;i<8;i++)
		{
			if(   (svalue[i]>='0' && svalue[i]<='9')
				||(svalue[i]>='A' && svalue[i]<='F')
				||(svalue[i]>='a' && svalue[i]<='f')
		      )
				s2[i]=svalue[i];
			else
				break;
		}
		s2[i]=0;
		*value=atodw(s2);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CConfigFile::ReadDWORD (LPCTSTR name, DWORD* value, DWORD default_value)
{
	*value=default_value;
	return ReadDWORD(name,value);
}


BOOL CConfigFile::ReadDWORDs (LPCTSTR name, DWORD* value, int n)
{
	int i,j;
	if(n<1) n=1;
	if(n>32) n=32;

	if(!IsOpened) return FALSE;
	if(!BufSection[0]) return FALSE;

	strncpy(s1,name,NAMELENGTHMAX);
	s1[NAMELENGTHMAX]=0;
	strcat(s1,"=");
	char* svalue=strstr(BufSection,s1);
	if(svalue)
	{
		svalue+=strlen(s1);
		for(j=0;j<n;j++)
		{
			while(svalue[0]==0x20 || svalue[0]=='\t') svalue++;
			if(svalue[0]=='0' && (svalue[1]=='x' || svalue[1]=='X')) svalue+=2;
			for(i=0;i<8;i++)
			{
				if(   (svalue[i]>='0' && svalue[i]<='9')
					||(svalue[i]>='A' && svalue[i]<='F')
					||(svalue[i]>='a' && svalue[i]<='f')
				  )
					s2[i]=svalue[i];
				else break;
			}
			s2[i]=0;
			value[j]=atodw(s2);
			svalue+=i;
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CConfigFile::ReadDWORDs (LPCTSTR name, DWORD* value, int n, DWORD default_value)
{
	if(n<1) n=1;
	if(n>32) n=32;
	for(int j=0;j<n;j++) value[j]=default_value;
	return ReadDWORDs(name,value,n);
}


BOOL CConfigFile::ReadString(LPCTSTR name, LPTSTR str, int nMaxBytes, int nIndex, char Delimiters)
{
	str[nMaxBytes]=0;
	if(!IsOpened) 
		return FALSE;
	if(!BufSection[0])
		return FALSE;

	int n=nMaxBytes;
	if(n>STRINGLENGTHMAX) 
		n=STRINGLENGTHMAX;

	strncpy(s1,name,NAMELENGTHMAX);
	s1[NAMELENGTHMAX]=0;
	strcat(s1,"=");
	char* svalue=strstr(BufSection,s1);
	if(svalue)
	{
		char *pnext=NextValue(svalue);
		svalue+=strlen(s1);
		for(int j=0; j<=nIndex; j++)
		{
			while(*svalue==0x20 || *svalue=='\t') 
				svalue++;
			int i = 0;
			for(i=0; i<n; i++)
			{
				if(svalue+i>=pnext || svalue[i]=='\n' || svalue[i]==Delimiters) break;
				str[i]=svalue[i];
			}
			svalue+=i;
			if(i==n) while(*svalue!=0 && *svalue!='\n' && *svalue!=Delimiters) svalue++;
			if(*svalue=='\n' || *svalue==Delimiters) svalue++;
			while(i) 
			{
				if(str[i-1]==' ') 
					i--; 
				else 
					break;
			}
			str[i]=0;
		}
		return TRUE;
	}
	else
	{
		str[0]=0;
		return FALSE;
	}
}

BOOL CConfigFile::ReadString(LPCTSTR name, CString& str, int nIndex, char Delimiters)
{
	char s[STRINGLENGTHMAX+16];
	if(ReadString(name,s,STRINGLENGTHMAX,nIndex,Delimiters))
	{
		str=s;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


BOOL CConfigFile::WriteBlankLine()
{
	if(!IsOpened) return FALSE;
	if(ReadOnly) return FALSE;
	if(!BufSection[0]) return FALSE;

	strcpy(s1,"\n");
	if(strlen(BufSection)+strlen(s1)<SECTIONLENGTHMAX)
	{
		strcat(BufSection,s1);
		SectionChanged=TRUE;
		FileChanged=TRUE;
		return TRUE;
	}
	else return FALSE;
}


BOOL CConfigFile::WriteInt(LPCTSTR name, int value)
{
	if(!IsOpened) return FALSE;
	if(ReadOnly) return FALSE;
	if(!BufSection[0]) return FALSE;

	strncpy(s1,name,NAMELENGTHMAX);
	s1[NAMELENGTHMAX]=0;
	strcat(s1,"=");
	char* svalue=strstr(BufSection,s1);
	sprintf(s1+strlen(s1),"%d\n",value);
	if(svalue) strcpy(svalue,NextValue(svalue));
	SectionChanged=TRUE;
	FileChanged=TRUE;
	if(strlen(BufSection)+strlen(s1)<SECTIONLENGTHMAX)
	{
		strcat(BufSection,s1);
		return TRUE;
	}
	else return FALSE;
}

BOOL CConfigFile::WriteDWORD(LPCTSTR name, DWORD value)
{
	if(!IsOpened) return FALSE;
	if(ReadOnly) return FALSE;
	if(!BufSection[0]) return FALSE;

	strncpy(s1,name,NAMELENGTHMAX);
	s1[NAMELENGTHMAX]=0;
	strcat(s1,"=");
	char* svalue=strstr(BufSection,s1);
	sprintf(s1+strlen(s1),"%lx\n",value);
	if(svalue) strcpy(svalue,NextValue(svalue));
	SectionChanged=TRUE;
	FileChanged=TRUE;
	if(strlen(BufSection)+strlen(s1)<SECTIONLENGTHMAX)
	{
		strcat(BufSection,s1);
		return TRUE;
	}
	else return FALSE;
}

BOOL CConfigFile::WriteInts(LPCTSTR name, int* value, int n)
{
	if(!IsOpened) return FALSE;
	if(ReadOnly) return FALSE;
	if(!BufSection[0]) return FALSE;

	int j;
	if(n<1) n=1;
	if(n>32) n=32;

	char* p=new char[NAMELENGTHMAX+n*24];
	if(!p) return FALSE;
	strncpy(s1,name,NAMELENGTHMAX);
	s1[NAMELENGTHMAX]=0;
	strcat(s1,"=");
	strcpy(p,s1);
	for(j=0;j<n;j++) sprintf(p+strlen(p),"%d ",value[j]);
	p[strlen(p)-1]='\n';
	char* svalue=strstr(BufSection,s1);
	if(svalue) strcpy(svalue,NextValue(svalue));
	SectionChanged=TRUE;
	FileChanged=TRUE;
	if(strlen(BufSection)+strlen(p)<SECTIONLENGTHMAX)
	{
		strcat(BufSection,p);
		delete[] p;
		return TRUE;
	}
	else
	{
		delete[] p;
		return FALSE;
	}
}

BOOL CConfigFile::WriteDWORDs(LPCTSTR name, DWORD* value, int n)
{
	if(!IsOpened) return FALSE;
	if(ReadOnly) return FALSE;
	if(!BufSection[0]) return FALSE;

	int j;
	if(n<1) n=1;
	if(n>32) n=32;

	char* p=new char[NAMELENGTHMAX+n*20];
	if(!p) return FALSE;
	strncpy(s1,name,NAMELENGTHMAX);
	s1[NAMELENGTHMAX]=0;
	strcat(s1,"=");
	strcpy(p,s1);
	for(j=0;j<n;j++) sprintf(p+strlen(p),"%lx ",value[j]);
	p[strlen(p)-1]='\n';
	char* svalue=strstr(BufSection,s1);
	if(svalue) strcpy(svalue,NextValue(svalue));
	SectionChanged=TRUE;
	FileChanged=TRUE;
	if(strlen(BufSection)+strlen(p)<SECTIONLENGTHMAX)
	{
		strcat(BufSection,p);
		delete[] p;
		return TRUE;
	}
	else
	{
		delete[] p;
		return FALSE;
	}
}

BOOL CConfigFile::WriteString(LPCTSTR name, LPCTSTR str)
{
	if(!IsOpened) return FALSE;
	if(ReadOnly) return FALSE;
	if(!BufSection[0]) return FALSE;

	int n=STRINGLENGTHMAX;

	char s[STRINGLENGTHMAX+6];
	strncpy(s,str,n);
	s[n]=0;
	if(s[strlen(s)-1]!='\n') strcat(s,"\n");
	strncpy(s1,name,NAMELENGTHMAX);
	s1[NAMELENGTHMAX]=0;
	strcat(s1,"=");
	char* svalue=strstr(BufSection,s1);
	if(svalue) strcpy(svalue,NextValue(svalue));
	SectionChanged=TRUE;
	FileChanged=TRUE;
	if(strlen(BufSection)+strlen(s)<SECTIONLENGTHMAX)
	{
		strcat(BufSection,s1);
		strcat(BufSection,s);
		return TRUE;
	}
	else return FALSE;
}

