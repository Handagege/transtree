#include "mtreeWorkInterface.h"

DYN_WORK(mtreeWorkInterface);

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
                cout << "@delete query node < rmid : " << rmid << " | kmid : " << kmid << " >" << endl;
                mtreeIns.destoryQueryMid(kmid,rmid);
                resp_root["status"] = "done";
        }
        else
        {
                cout << "no method point to : " << cmd << endl;
                resp_root["status"] = "undone";
                resp_root["info"] = "can\'t find the command";
        }
        printTree();

}



void mtreeWorkInterface::printTree()
{
        cout << "*****print tree " << "[ size : " << sizeof(mtreeIns) << "bit ] *******" << endl;
        mtreeIns.preOrder(mtreeIns.getRoot());
        cout << "****************************" << endl << endl;
}


