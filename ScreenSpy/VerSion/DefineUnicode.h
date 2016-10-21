

#define VER_Major    0
#define VER_Minor    0
#define VER_Release  0     
#define VER_Build    17  
#define VER_Version    MAKELONG(MAKEWORD(VER_Major, VER_Minor), VER_Release)
#define _Stringizing(v)    #v
#define _VerJoin(a, b, c, d)  _Stringizing(a.b.c.d)
#define _VerJoinBuild(a, b, c, d)  _Stringizing(a.b.c.d)
#define _VerJoinMajor(a, b)  _Stringizing(a.b)
#define STR_BuildDate    TEXT(Define_C::BuildDate())
#define STR_BuildTime    TEXT(__TIME__)
#define STR_BuilDateTime  TEXT(__DATE__) TEXT(" ") TEXT(__TIME__)
#define STR_Version           TEXT(_VerJoin(VER_Major, VER_Minor, VER_Build, VER_Release))
#define STR_VersionBuild           TEXT(_VerJoinBuild(VER_Major, VER_Minor, VER_Build, VER_Release))
#define STR_VersionMajor           TEXT(_VerJoinMajor(VER_Major, VER_Minor))
#define STR_CopyRight           TEXT("CopyRight (C) 2012 - ")
#define STR_AppName    TEXT("·ÂQQ½ØÆÁ³ÌÐò")
#define STR_Author    TEXT("¨tWitch¤Ä_")
#define STR_Corporation    TEXT("¨tWitch¤Ä_Zoom")
#define STR_OriginalFilename    TEXT("QQScreenSpy.exe")
#define STR_Web      TEXT("http://blog.csdn.net/Witch_Soya")
#define STR_Email    TEXT("snifferzhu@qq.com")
#define STR_WebUrl    TEXT("http:") STR_Web
#define STR_EmailUrl    TEXT("mailto:") STR_Email TEXT("?Subject=") STR_AppName
#define STR_Description    TEXT("Ä£·ÂQQ½ØÆÁ³ÌÐò")
#define STR_Copyright    TEXT("CopyRight (C) 2012 - ") STR_Corporation TEXT(",All rights reserved")


class Define_C
{
public:
	static CString BuildDate()
	{
		CString _strDate = __DATE__;
		char * _month[] =  
		{  
			"Jan",  
			"Feb",  
			"Mar",  
			"Apr",  
			"May",  
			"Jun",  
			"Jul",  
			"Aug",  
			"Sep",  
			"Oct",  
			"Nov",  
			"Dec",  
		};  
		int month = 0;  
		for(int i = 0; i < 12; i++)  
		{  
			if(memcmp(_month[i], _strDate.Left(3), 3) == 0)  
			{  
				month = i+1;  
				break;  
			}  
		}  

		CString _strD, _strReturn;
		_strReturn += _strDate.Right(4) + "-";

		_strD.Format("%d",month);
		_strReturn += _strD + "-";

		_strReturn += _strDate.Mid(4,2);
		_strReturn.Replace(" ","");
		return _strReturn;
	}	
};
