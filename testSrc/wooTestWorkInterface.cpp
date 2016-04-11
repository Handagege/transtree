#include "wooTestWorkInterface.h"

DYN_WORK(wooTestWorkInterface);

int wooTestWorkInterface::work_core(json_object *req_json, 
		char* &p_out_string, int& n_out_len, int64_t req_id){
	int result = 0;
        WooDbInterface* pFilterDbInterface = (WooDbInterface*)GetDbInterface("EXAMPLE_WOO_DB");
        Json::Value req_root, req_filter_root;
        if(!JsonConv::ObjC2Cpp(req_root, req_json))
        {
                LOG_ERROR("MGR; failed to convert the request in json!");
                return -1;
        }

        
        cout << "[ cmd : " << req_root["cmd"].asString() << " mids : ";
        for(uint32_t i=0; i<req_root["mids"].size();++i)
                cout << req_root["mids"][i].asString() << " ";
        cout << "]" << endl;

        req_filter_root["cmd"] = req_root["cmd"];
        req_filter_root["mids"] = req_root["mids"];
        char split_char, second_split_char;
        char* p_result;
        int filter_result = pFilterDbInterface->s_get(0, JsonConv::CppObj2String(req_filter_root).c_str(),
                p_result, split_char, second_split_char);
        strcpy(p_out_string, p_result);

        cout << p_out_string << endl;
        n_out_len = strlen(p_out_string);
	return result;
}
