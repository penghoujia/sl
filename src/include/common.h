#ifndef COMMON_H
#define COMMON_H

#define CONFIG_FILE_NAME TEXT("config.ini")

//导入实时数据库接口
#ifdef IMPORT_RTDB_INTERFACE
	#import "..\..\bin\rtdb.exe" no_namespace
#endif 



#endif