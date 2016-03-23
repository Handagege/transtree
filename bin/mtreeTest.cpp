#include <MultiWayTree.h>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <cassert>
#include "json/json.h"
#include <unistd.h>


using namespace std;


void printTree(CMultiWayTree& ins)
{
        cout << endl << "*****print tree " << "[size : " << sizeof(ins) << "bit ] *******" << endl;
        ins.preOrder(ins.getRoot());
        cout << "************" << endl << endl;
}


void testCreateTree()
{
        cout << string(20,'-') << endl;
        cout << "test tree create" << endl;
        cout << string(20,'-') << endl;
        CMultiWayTree ins;
        printTree(ins);
}


void manager(CMultiWayTree& ins, Json::Value& v)
{
        string cmd = v["cmd"].asString();
        string rmid = v["rmid"].asString();
        string kmid = v["kmid"].asString();
        if( cmd == "insertqn" )
        {
                cout << "@insert query node < rmid : " << rmid << " | kmid : " << kmid << " >" << endl;
                ins.insertQueryMid(kmid,rmid);
        }
        else if( cmd == "insertfn" )
        {
                string pmid = v["pmid"].asString();
                cout << "@insert firehose node < rmid : " << rmid << " | pmid : " << 
                pmid << " | kmid : " << kmid << " >" << endl;
                ins.insertFirehoseMid(kmid,pmid,rmid);
        }
        else if( cmd == "deleteqn" )
        {
                cout << "@delete query node < rmid : " << rmid << " | kmid : " << kmid << " >" << endl;
                ins.destoryQueryMid(kmid,rmid);
        }
        else
        {
                cout << "no method point to : " << cmd << endl;
        }
        printTree(ins);

}


void testInsertNode(char* inputFile)
{
        CMultiWayTree ins;
        printTree(ins);
        cout << string(20,'-') << endl;
        cout << "test tree insert node" << endl;
        cout << string(20,'-') << endl;
        ifstream ifs;
        ifs.open(inputFile);
        assert(ifs.is_open());
                 
        Json::Reader reader;
        Json::Value root;
        if (!reader.parse(ifs, root, false))
        {
                return;
        }
        for(int i = 0; i<root.size(); ++i){
                manager(ins,root[i]);
                sleep(2);
        }
}


void mtreeBaseTest(char* inputFile)
{
        //testCreateTree();
        testInsertNode(inputFile);
}


int main(int argc,char *argv[])
{
        //mtreeNode temp("1000000000000000");
        //cout << sizeof(temp) << endl;
        //cout << sizeof(temp.sChildren) << endl;
        //cout << sizeof(new mtreeNode("1000000000000002")) << endl;
        //temp.sChildren.insert(new mtreeNode("1000000000000001"));
        //cout << sizeof(temp) << endl;
        //cout << sizeof(temp.sChildren) << endl;

        mtreeBaseTest(argv[1]);
        return 1;
}
