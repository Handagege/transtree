#include <iostream>
#include <MultiWayTree.h>
#include <stdio.h> 
#include <set>


using namespace std;


void test1()
{
        CMultiWayTree mtreeIns(new mtreeNode("0000000000000000"),4,50000);
        string rmid = "0100000000000123";
        string rmid2 = "0100000000001123";
        mtreeIns.insertQueryMid(rmid,rmid);
        //mtreeIns.insertQueryMid(rmid2,rmid);
        char kmid[20];
        for(int i = 1; i<50000; ++i){
                sprintf(kmid, "%016d", i);
                cout << "insert fn : " << kmid << endl;
                mtreeIns.insertFirehoseMid(string(kmid),rmid,rmid);
        }
        sleep(10);
        sprintf(kmid, "%016d", 100000);
        mtreeIns.insertFirehoseMid(string(kmid),rmid,rmid);
        for(int i = 1; i<10; ++i){
                sleep(0.1);
                sprintf(kmid, "%016d", i);
                cout << "insert fn : " << kmid << endl;
                mtreeIns.insertFirehoseMid(string(kmid),rmid,rmid);
        }
}


void  test3()
{
        vector<string*> vec;
        char kmid[20];
        for(int i = 1; i<10; ++i){
                sprintf(kmid, "%016d", i);
                cout << "insert fn : " << kmid << endl;
                vec.push_back(new string(kmid));
        }
        for(int i = 1; i<10; ++i){
                cout << *vec[i-1] << endl;
        }
        for(int i = 1; i<10; ++i){
                cout << "delete fn : " << vec[i-1] << endl;
                delete vec[i-1];
        }
        cout << vec.size() << endl;
        for(int i = 1; i<10; ++i){
                cout << vec[i-1] << endl;
        }
        for(int i = 1; i<10; ++i){
                cout << *vec[i-1] << endl;
        }

}

int main()
{
        test3();
        return 1;
}
