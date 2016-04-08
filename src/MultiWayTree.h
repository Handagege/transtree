// 文件名：CMultiWayTree.h
// 说明：转发链路管理接口声明
// 最后更新：
//      v1.0 - 2016.03.24 by zhanghan(zhanghan5@staff.weibo.com)
/////////////////////////////////////////////////////////////////


#ifndef _MULTI_WAY_TREE_H
#define _MULTI_WAY_TREE_H


#include "defType.h"
#include <iostream>
#include <set>
#include <string>
#include <algorithm>
#include <time.h>
#include "pthread.h"

using namespace std;

//函数对象，适配set容器class Compare
template <class T>
struct nodeComp : binary_function <T,T,bool>{
        bool operator() (const T& x, const T& y) const 
        {return x->mid < y->mid;}
};

//节点类
class mtreeNode {
public:
        //mtreeNode(){}
        //virtual ~mtreeNode(){}

        //构造函数，需提供mid，没有默认构造函数
        mtreeNode(string tmid,int32_t thierarchy = -1,bool tpruneFlag = false)
        {
                mid = tmid;
                hierarchy = thierarchy;
                pruneFlag = tpruneFlag;
                utime = time(NULL);
        }

        // 函数名：
        // 说明：标准节点信息打印
        // 参数：
        //      [OUT] 
        //      [IN] 
        // 返回值:
        void print()
        {
                cout << "< " << mid << " > ---- [ hierarchy : "  << hierarchy << "| isPruned : " 
                << boolalpha << pruneFlag << "| childrenNum : " << sChildren.size() <<" ]" << 
                "---- update time : " << utime << endl;
        }

        void updateTime() { utime = time(NULL); }

        string mid;             //
        bool pruneFlag;         //剪枝标记位
        int32_t hierarchy;      //所在树层级
        time_t utime;           //更新时间
        set<mtreeNode *, nodeComp<mtreeNode *> > sChildren;     //一级转发节点
};


typedef set<mtreeNode *, nodeComp<mtreeNode *> > SnodeContainer;
typedef SnodeContainer::iterator SnodeIter;
typedef pair<SnodeIter, bool> SnodePair;


class CMultiWayTree
{
public:
        CMultiWayTree();

        //根节点构造
        CMultiWayTree(mtreeNode* root, int32_t upperHierarchy=4, uint32_t upperChildrenNum=8);
        virtual ~CMultiWayTree();

        // 函数名：
        // 说明：获得根节点（-1层级）
        // 参数：
        // 返回值: 根节点指针
        mtreeNode* getRoot(){ return m_root; }

        // 函数名：
        // 说明: 获得所有原创根节点（0层级）
        // 返回值: SnodeContainer set容器对象
        SnodeContainer getRootNodes(){ return m_root->sChildren; }

        // 函数名：
        // 说明：插入监控微博节点，包括原创和转发（1层级）
        // 参数：
        //      [OUT] 
        //      [IN] originMid -- 原创节点微博
        //      [IN] queryMid -- 查询监控微博（可以和原创节点相同）
        // 返回值:
        void insertQueryMid(const string& queryMid, const string& originMid);

        // 函数名：
        // 说明：销毁监控微博节点
        // 参数：
        //      [IN] originMid -- 原创节点微博
        //      [IN] queryMid -- 查询监控微博（可以和原创节点相同）
        // 返回值:
        void destoryQueryMid(const string& queryMid, const string& originMid);

        // 函数名：
        // 说明：销毁原创微博节点，在没有监控节点挂载在其下时
        // 参数：
        //      [IN] originMid -- 原创节点微博
        // 返回值:
        void destoryRootMid(const string& originMid);

        // 函数名：
        // 说明：依需求对转发树进行统计计算，**定义未实现**
        // 参数：同上述
        // 返回值:
        void analysisQueryMid(const string& queryMid, const string& originMid);

        //处理firehose-weibo数据的操作
        // 函数名：
        // 说明：插入firehose-weibo节点
        // 参数：
        //      [IN] originMid -- 原创微博
        //      [IN] parentMid -- 父微博（被转发）
        //      [IN] keyMid -- 该微博
        // 返回值:
        void insertFirehoseMid(const string& keyMid, const string& parentMid, const string& originMid);

        //基本树操作
        // 函数名：
        // 说明：前序遍历示例，可根据此思路实现analysis
        // 参数：
        //      [IN] keyNode -- 遍历起始节点
        // 返回值:
        void preOrder(mtreeNode* keyNode);

        // 函数名：
        // 说明：析构时释放整个树的堆存储空间
        void destoryTotalTree(){ destoryKeyNode(m_root); }
private:
        // 函数名：
        // 说明：搜索获得节点位置
        // 参数：
        //      [OUT] result -- 命中节点集合
        //      [IN] queryMid -- 待查询微博节点（用被转发节点查询挂载点）
        //      [IN] originMid -- 原创微博节点
        // 返回值:
        void search(const string& queryMid, const string& originMid, vector<mtreeNode*>& result);

        // 函数名：
        // 说明：在某一监控节点子树中搜索获得节点位置
        //一条查询链路只可能存在一个挂载点
        // 参数：
        //      [IN] queryMid -- 待查询微博节点（用被转发节点查询挂载点）
        //      [IN] routeNode -- 起始微博节点
        //      [OUT] singleResult -- 某一路径下的唯一命中节点
        // 返回值:
        void searchRoute(const string& queryMid, mtreeNode* routeNode, mtreeNode*& singleResult);

        // 函数名：
        // 说明：根据父子关系插入
        // 参数：
        //      [OUT] pair类型 <first：节点所处迭代器，second：是否存在该节点>
        //              判断是否需要delete新创建的节点内存
        //      [IN] parentNode -- 父节点
        //      [IN] keyNode -- 该节点
        // 返回值: second -- true 成功插入 false -- 存在该mid的节点
        SnodePair insert(mtreeNode* parentNode,mtreeNode* keyNode)
        {
                return parentNode->sChildren.insert(keyNode);
        }

        // 函数名：
        // 说明：根据父节点查找微博挂载点
        // 参数：同上述
        // 返回值: 节点所处迭代器 *it 得到节点数据指针
        SnodeIter getIter(mtreeNode* parentNode,const string& keyMid);
        //获取origin mid存储迭代器

        // 函数名：
        // 说明：查找原创微博所在节点
        // 参数：同上述
        // 返回值: 节点所处迭代器 *it 得到节点数据指针
        SnodeIter getOriginIter(const string& originMid);

        // 函数名：
        // 说明：对节点是否需要剪枝进行检查
        // 参数：
        //      [IN] node -- 待检查节点
        // 返回值: true 需要剪枝 false 不需要
        bool checkBranch(mtreeNode* node);
        //剪枝策略

        // 函数名：
        // 说明：剪枝方法
        // 参数：
        //      [IN] node -- 剪枝节点
        // 返回值:
        void prune(mtreeNode* node);

        //多叉树的销毁
        // 函数名：
        // 说明：销毁指定节点的子树（包含该节点）
        // 参数：
        //      [IN] keyNode -- 遍历起始节点
        // 返回值:
        void destoryKeyNode(mtreeNode* keyNode);

        mtreeNode* m_root;                      //根节点
        int32_t m_upperHierarchy;               //最大层级数
        uint32_t m_upperChildrenNum;            //最大孩子树
        uint32_t m_upperQnNumPerOn;             //原创节点下最多挂载监控节点数

        pthread_mutex_t m_mutex;
};
#endif /* _MULTI_WAY_TREE_H */
