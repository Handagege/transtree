#include <MultiWayTree.h>


CMultiWayTree::CMultiWayTree()
{
        m_root = new mtreeNode("0000000000000000");
        m_upperHierarchy = 4;
        m_upperChildrenNum = 3;
        pthread_mutex_init(&m_mutex,NULL);
        cout << "----upper hierarchy : " << m_upperHierarchy << endl;
        cout << "----upper children num : " << m_upperChildrenNum << endl;
}


CMultiWayTree::CMultiWayTree(mtreeNode* root, int32_t upperHierarchy, uint32_t upperChildrenNum)
{
        m_root = root;
        m_upperHierarchy = upperHierarchy;
        m_upperChildrenNum = upperChildrenNum;
        pthread_mutex_init(&m_mutex,NULL);
}


CMultiWayTree::~CMultiWayTree()
{
        destoryTotalTree();
        pthread_mutex_destroy(&m_mutex);
}

/////////////////////////////////////////////////////////////////////
////operator

void CMultiWayTree::insertQueryMid(const string& queryMid, const string& originMid)
{
        mtreeNode* tqueryNode = new mtreeNode(originMid,0);
        if( insert(m_root,tqueryNode).second == false )
                delete tqueryNode;
        SnodeIter origin_it = getOriginIter(originMid);
        insert(*origin_it,new mtreeNode(queryMid,(*origin_it)->hierarchy+1));
}


void CMultiWayTree::destoryQueryMid(const string& queryMid, const string& originMid)
{
        SnodeIter origin_it = getOriginIter(originMid);
        if( origin_it != m_root->sChildren.end() )
        {
                SnodeIter query_it = getIter(*origin_it,queryMid);
                if( query_it != (*origin_it)->sChildren.end() )
                {
                        destoryKeyNode(*query_it);
                        (*origin_it)->sChildren.erase(query_it);
                }
                if((*origin_it)->sChildren.empty())
                {
                        destoryKeyNode(*origin_it);
                        m_root->sChildren.erase(origin_it);
                }
        }
}


void CMultiWayTree::destoryRootMid(const string& originMid)
{
        SnodeIter origin_it = getOriginIter(originMid);
        if( origin_it != m_root->sChildren.end() )
        {
                destoryKeyNode(*origin_it);
                m_root->sChildren.erase(origin_it);
        }
}


void CMultiWayTree::analysisQueryMid(const string& queryMid, const string& originMid){}


void CMultiWayTree::insertFirehoseMid(const string& keyMid, const string& parentMid, const string& originMid)
{
        pthread_mutex_lock(&m_mutex);
        vector<mtreeNode*> parentVec;
        search(parentMid,originMid,parentVec);
        vector<mtreeNode*>::iterator vm_it = parentVec.begin();
        while(vm_it != parentVec.end())
        {
                int32_t keyMidHierarchy = (*vm_it)->hierarchy + 1;
                bool keyMidPruneFlag = (keyMidHierarchy >= m_upperHierarchy);
                insert(*vm_it,new mtreeNode(keyMid,keyMidHierarchy,keyMidPruneFlag));
                if( checkBranch(*vm_it) )
                        prune(*vm_it);
                ++vm_it;
        }
        pthread_mutex_unlock(&m_mutex);
}


void CMultiWayTree::preOrder(mtreeNode* keyNode)
{
        if(keyNode != NULL)
        {
                keyNode->print();
                for(SnodeIter sn_it = keyNode->sChildren.begin();sn_it != keyNode->sChildren.end();++sn_it)
                        preOrder(*sn_it);
        }
}


void CMultiWayTree::getSubChildren(mtreeNode* keyNode, vector<mtreeNode*>& result)
{
        if(keyNode != NULL)
        {
                result.push_back(keyNode);
                for(SnodeIter sn_it = keyNode->sChildren.begin();sn_it != keyNode->sChildren.end();++sn_it)
                        getSubChildren(*sn_it,result);
        }
}


bool CMultiWayTree::getSubChildren(const string& queryMid, const string& originMid, vector<mtreeNode*>& result)
{
        SnodeIter origin_it = getOriginIter(originMid);
        if( origin_it != m_root->sChildren.end() )
        {
                SnodeIter query_it = getIter(*origin_it,queryMid);
                if( query_it != (*origin_it)->sChildren.end() )
                        getSubChildren(*query_it,result);
                else
                {
                        return false;
                }
        }
        else
        {
                return false;
        }
        return true;
}


void CMultiWayTree::searchRoute(const string& queryMid, mtreeNode* routeNode, mtreeNode*& singleResult)
{
        if(routeNode != NULL && singleResult == NULL)
        {
                //routeNode->print();
                SnodeIter route_it = getIter(routeNode,queryMid);
                if(route_it != routeNode->sChildren.end())
                {
                        //已经找到
                        singleResult = *route_it;
                }
                else{
                        SnodeIter sn_it = routeNode->sChildren.begin();
                        for(;sn_it != routeNode->sChildren.end();++sn_it)
                                searchRoute(queryMid,*sn_it,singleResult);
                }
        }
}


void CMultiWayTree::search(const string& queryMid, const string& originMid, vector<mtreeNode*>& result)
{
        SnodeIter origin_it = getOriginIter(originMid);
        if( origin_it != m_root->sChildren.end() )
        {
                //更新原创根节点时间
                (*origin_it)->updateTime();
                SnodeIter route_it = (*origin_it)->sChildren.begin();
                while(route_it != (*origin_it)->sChildren.end()){
                        if(queryMid == (*route_it)->mid){
                                if( !((*route_it)->pruneFlag) )
                                        result.push_back(*route_it);
                        }
                        else{
                                mtreeNode* singleResult = NULL;
                                searchRoute(queryMid,*route_it,singleResult);
                                if(singleResult != NULL && !(singleResult->pruneFlag))
                                {
                                        result.push_back(singleResult);
                                }
                        }
                        ++route_it;
                }
                for(vector<mtreeNode*>::iterator it = result.begin(); it != result.end(); ++it)
                {
                        (*it)->print();
                }
        }
}


SnodeIter CMultiWayTree::getIter(mtreeNode* parentNode,const string& keyMid)
{
        mtreeNode* tempNode = new mtreeNode(keyMid);
        SnodeIter result = parentNode->sChildren.find(tempNode);
        delete tempNode;
        return result;
}


SnodeIter CMultiWayTree::getOriginIter(const string& originMid)
{
        return getIter(m_root,originMid);
}


bool CMultiWayTree::checkBranch(mtreeNode* node)
{
        return node->sChildren.size() >= m_upperChildrenNum || 
                node->hierarchy >= m_upperHierarchy;
}


void CMultiWayTree::prune(mtreeNode* node)
{
        SnodeIter sn_it = node->sChildren.begin();
        while(sn_it != node->sChildren.end())
        {
                destoryKeyNode(*sn_it);
                ++sn_it;
        }
        node->sChildren.clear();
        node->pruneFlag = true;
}


void CMultiWayTree::destoryKeyNode(mtreeNode* keyNode)
{
        cout << "DELETE : " ;
        keyNode->print();
        if(keyNode != NULL)
        {
                SnodeIter i = keyNode->sChildren.begin();
                while(i != keyNode->sChildren.end())
                {
                        destoryKeyNode(*i);
                        ++i;
                }
                delete keyNode;
        }
}
