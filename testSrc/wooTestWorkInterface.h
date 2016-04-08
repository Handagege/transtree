#ifndef _EXAMPLE_WORK_INTERFACE_HEADER_
#define _EXAMPLE_WORK_INTERFACE_HEADER_

#include "work_interface.h"
#include "ini_file.h"
#include "woo/log.h"
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <iostream>
#include "json.h"
#include <time.h>
#ifdef __GNUC__
#include <ext/hash_map>
#else
#include <hash_map>
#endif
#include "JsonConv.h"

class wooTestWorkInterface : public WorkInterface{
	public:
		wooTestWorkInterface(DbCompany*& p_db_company, int interface_id):
		WorkInterface(p_db_company, interface_id){
		}
		~wooTestWorkInterface(){
		}

	public:
		int work_core(json_object *req_json, char* &p_out_string, int& n_out_len, int64_t req_id);
			
	        // 函数名：GetDbInterface
	        // 说明：获取远程db接口
	        // 参数：
	        //	[IN] sDbName - 远程db名
	        // 返回值：成功返回远程db的对象指针，失败返回NULL
	        DbInterface* GetDbInterface(const char* sDbName)
	        {
	        	// get db company
	        	if(NULL == p_db_company_)
	        	{
	        		LOG_ERROR("db company is null!");
	        		return NULL;
	        	}
	        	// get db interface	
	        	DbInterface* p_db_interface = p_db_company_->get_db_interface(sDbName); 
	        	if(NULL == p_db_interface)
	        		LOG_ERROR("db interface named %s is NULL!", sDbName);
	        	return p_db_interface; 	
	        }

};
#endif
