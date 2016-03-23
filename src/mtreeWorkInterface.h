#ifndef _MTREE_WORK_INTERFACE_
#define _MTREE_WORK_INTERFACE_

#include "work_interface.h"
#include "MultiWayTree.h"
#include "ini_file.h"
#include "woo/log.h"
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <iostream>
#include "json.h"
#include <time.h>
#include "JsonConv.h"

class mtreeWorkInterface : public WorkInterface{
	public:
		mtreeWorkInterface(DbCompany*& p_db_company, int interface_id):
		WorkInterface(p_db_company, interface_id){
		}
		~mtreeWorkInterface(){
		}

		int work_core(json_object *req_json, char* &p_out_string, int& n_out_len, int64_t req_id);

        private:
                void manager(const Json::Value& req_root,Json::Value& resp_root);
                void printTree();
                
                CMultiWayTree mtreeIns;
			
};
#endif
