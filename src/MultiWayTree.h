#ifndef _MULTI_WAY_TREE_H
#define _MULTI_WAY_TREE_H


#include "defType.h"
#include <iostream>
#include <set>
#include <string>
#include <algorithm>
#include <time.h>

using namespace std;

template <class T>
struct nodeComp : binary_function <T,T,bool>{
        bool operator() (const T& x, const T& y) const 
        {return x->mid < y->mid;}
};


class mtreeNode {
public:
        //mtreeNode(){}
        //virtual ~mtreeNode(){}

        mtreeNode(string tmid,int32_t thierarchy = -1,bool tpruneFlag = false)
        {
                mid = tmid;
                hierarchy = thierarchy;
                pruneFlag = tpruneFlag;
                utime = time(NULL);
        }

        void print()
        {
                cout << "< " << mid << " > ---- [ hierarchy : "  << hierarchy << "| isPruned : " 
                << boolalpha << pruneFlag << "| childrenNum : " << sChildren.size() <<" ]" << 
                "---- update time : " << utime << endl;
        }

        void updateTime() { utime = time(NULL); }

        string mid;
        bool pruneFlag;
        int32_t hierarchy;
        time_t utime;
        set<mtreeNode *, nodeComp<mtreeNode *> > sChildren;
};


typedef set<mtreeNode *, nodeComp<mtreeNode *> > SnodeContainer;
typedef SnodeContainer::iterator SnodeIter;


class CMultiWayTree
{
public:
        CMultiWayTree();
        CMultiWayTree(mtreeNode* root, int32_t upperHierarchy=4, uint32_t upperChildrenNum=8);
        virtual ~CMultiWayTree();

        mtreeNode* getRoot(){ return m_root; }
        SnodeContainer getRootNodes(){ return m_root->sChildren; }

        //处理查询命令中的微博的操作
        void insertQueryMid(const string& queryMid, const string& originMid);
        void destoryQueryMid(const string& queryMid, const string& originMid);
        void destoryRootMid(const string& originMid);
        void analysisQueryMid(const string& queryMid, const string& originMid);

        //处理firehose-weibo数据的操作
        //插入节点
        void insertFirehoseMid(const string& keyMid, const string& parentMid, const string& originMid);

        //基本树操作
        //前序遍历
        void preOrder(mtreeNode* keyNode);
        void destoryTotalTree(){ destoryKeyNode(m_root); }
private:
        //查找指定mid的存储位置
        void search(const string& queryMid, const string& originMid, vector<mtreeNode*>& result);
        //一条查询链路只可能存在一个挂载点
        void searchRoute(const string& queryMid, mtreeNode* routeNode, mtreeNode*& singleResult);

        void insert(mtreeNode* parentNode,mtreeNode* keyNode){ parentNode->sChildren.insert(keyNode); }

        //根据父节点获得mid存储迭代器
        SnodeIter getIter(mtreeNode* parentNode,const string& keyMid);
        //获取origin mid存储迭代器
        SnodeIter getOriginIter(const string& originMid);

        //剪枝函数
        //检查是否满足剪枝条件
        bool checkBranch(mtreeNode* node);
        //剪枝策略
        void prune(mtreeNode* node);

        //多叉树的销毁
        void destoryKeyNode(mtreeNode* keyNode);

        mtreeNode* m_root;
        int32_t m_upperHierarchy;
        uint32_t m_upperChildrenNum;
};
#endif /* _MULTI_WAY_TREE_H */
