#include "mtreeWorkInterface.h"

DYN_WORK(mtreeWorkInterface);

CMultiWayTree mtreeWorkInterface::mtreeIns = CMultiWayTree(new mtreeNode("0000000000000000"),4,500);

int mtreeWorkInterface::work_core(json_object *req_json, 
		char* &p_out_string, int& n_out_len, int64_t req_id){
	int result = 1;

        Json::Value req_root, resp_root;
        if(!JsonConv::ObjC2Cpp(req_root, req_json))             
        {                       
                LOG_ERROR("MGR; failed to convert the request in json!");
                return -1;                      
        }

        manager(req_root,resp_root);
        strcpy(p_out_string, JsonConv::CppObj2String(resp_root).c_str());               
        n_out_len = strlen(p_out_string);

	return result;
}


void mtreeWorkInterface::manager(const Json::Value& req_root,Json::Value& resp_root)
{
        string cmd = req_root["cmd"].asString();
        resp_root["cmd"] = cmd;
        string rmid = req_root["body"]["rmid"].asString();
        string kmid = req_root["body"]["kmid"].asString();
        if( cmd == "insertqn" )
        {
                Json::Value req_filter_root,rmids;
                req_filter_root["cmd"] = "add";
                rmids.append(rmid);
                req_filter_root["mids"] = rmids;
                pushToFilterList(req_filter_root);
                cout << "@insert query node < rmid : " << rmid << " | kmid : " << kmid << " >" << endl;
                mtreeIns.insertQueryMid(kmid,rmid);
                resp_root["status"] = "done";
        }
        else if( cmd == "insertfn" )
        {
                string pmid = req_root["body"]["pmid"].asString();
                cout << "@insert firehose node < rmid : " << rmid << " | pmid : " << 
                pmid << " | kmid : " << kmid << " >" << endl;
                mtreeIns.insertFirehoseMid(kmid,pmid,rmid);
                resp_root["status"] = "done";
        }
        else if( cmd == "deleteqn" )
        {
                Json::Value req_filter_root,rmids;
                req_filter_root["cmd"] = "del";
                rmids.append(rmid);
                req_filter_root["mids"] = rmids;
                pushToFilterList(req_filter_root);
                cout << "@delete query node < rmid : " << rmid << " | kmid : " << kmid << " >" << endl;
                mtreeIns.destoryQueryMid(kmid,rmid);
                resp_root["status"] = "done";
        }
        else if( cmd == "analysis" )
        {
                printTree();
                resp_root["status"] = "done";
        }
        else
        {
                cout << "no method point to : " << cmd << endl;
                resp_root["status"] = "undone";
                resp_root["info"] = "can\'t find the command";
        }
        //printTree();

}


void mtreeWorkInterface::pushToFilterList(Json::Value req_filter_root)
{
        char* p_result;
        char split_char, second_split_char;
        WooDbInterface* pFilterDbInterface = (WooDbInterface*)GetDbInterface("EXAMPLE_WOO_DB");
        int filter_result = pFilterDbInterface->s_get(0, JsonConv::CppObj2String(req_filter_root).c_str(),
                p_result, split_char, second_split_char);
        if(filter_result == 1)
        {
                cout << "@" << req_filter_root["cmd"].asString() << "root in filter list ... done" << endl;
                cout << "\'pushToFilterList\' result : " << p_result << endl;
        }
        else
                cout << "!some error occur on communicate with filter server" << endl;
}


void mtreeWorkInterface::printTree()
{
        cout << "*****print tree " << "[ size : " << sizeof(mtreeIns) << "bit ] *******" << endl;
        mtreeIns.preOrder(mtreeIns.getRoot());
        cout << "****************************" << endl << endl;
}


