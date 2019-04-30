#ifndef _SKIPLIST_H__
#define _SKIPLIST_H__
#include<time.h>
#include<iostream>
#include<vector>
#include<cstdio>
#include<cstdlib>
using namespace std;
#define MAXLEVE 16                                             //跳表的最大层数

template<typename K,typename V>
struct SkipNode                                               //跳表的节点类型
{
   K _key;
   V _value;
   size_t _sz;                                                //表示该节点的层数
   vector<SkipNode<K,V> *> _pleve;                            //存放每一层的指针
   SkipNode(K key=K(),V value=V(),size_t sz=size_t())
       :_key(key)
        ,_value(value)
        ,_sz(sz)
    {
        _pleve.resize(0);
        for(size_t i=0;i<sz;i++)
        {
            _pleve.push_back(NULL);
        }
    }

   ~SkipNode()
   {
        _key=-1;
        _value=-1;
        _sz=-1;
        _pleve.clear();
   }
};

template<typename K,typename V>
class SkipList                                               //跳表类
{
public:
    typedef SkipNode<K,V> Node;
    SkipList();
    void Insert(K key,V value);
    bool Find(K key,V& value);
    bool Erase(K key);
    void Print();
    int GetLeve();                                            //返回跳表的最大层数
    size_t Size();
    ~SkipList();
private:
    int Random();                                             //产生随机层数的函数
protected:
    SkipList(SkipList<K,V> &);                                //防拷贝
    SkipList<K,V>& operator=(SkipList<K,V>);                  //防赋值
private:
    Node *_head;
    int _maxLeve;                                             //记录跳表的最大层数
    int _size;                                                //记录跳表最底层元素的个数
};
#endif    //_SKIPLIST_H__


template<typename K,typename V>
size_t SkipList<K,V>::Size()
{
    return _size;
}

template<typename K,typename V>
int SkipList<K,V>::GetLeve()
{
    return _maxLeve;
}

template<class K,class V>
int SkipList<K,V>::Random()
{
    int leve=1;
    while(rand()%2&&leve<=MAXLEVE)                 //产生0或1,1的话leve++,最后平均下来leve的值趋向与正态分布
    {
        leve++;
    }
    return leve;
}

template<typename K,typename V>
SkipList<K,V>::SkipList()
{
    _maxLeve=1;
    _size=0;
    _head=new Node(-1,"",MAXLEVE);
}

template<typename K,typename V>
void SkipList<K,V>::Insert(K key,V value)
{
    int i=_maxLeve-1;
    int j=0;
    V temp;
    Node* cur=_head;                                     //指向跳表的起点
    Node* s[MAXLEVE];
    if(Find(key,temp))
    {
        Erase(key);
    }


    while(i>=0)                                     //用来保存每层向下跳转位置的前驱
    {
        while(cur->_pleve[i])
        {
            if(key>=cur->_pleve[i]->_key)
            {
                cur=cur->_pleve[i];
            }
            else
                break;
        }
        s[j++]=cur;
        i--;
    }

    i=0;
    int leve=Random();                                    //产生一个随机层数
    _maxLeve<leve?_maxLeve=leve:_maxLeve;                 //更新跳表的最大层数
    Node* newNode=new Node(key,value,leve);                  //创建一个节点
    for(i=0;i<leve;i++)
    {
        if(i<j)
        {
            newNode->_pleve[i]=s[j-i-1]->_pleve[i];
            s[j-i-1]->_pleve[i]=newNode;
        }
        else
        {
            _head->_pleve[i]=newNode;
        }
    }
    _size++;

}

template<typename K,typename V>
bool SkipList<K,V>::Find(K key,V& value)
{
    int i=_maxLeve-1;
    Node* cur=_head;                                     //指向跳表的起点
    while(i>=0)
    {
        while(cur->_pleve[i])
        {
            if(key>=cur->_pleve[i]->_key)
            {
                cur=cur->_pleve[i];
            }
            else
                break;
        }
        i--;
    }
    i=0;
    if(cur->_key==key)                                      //最好采用仿函数进行比较
    {
        value=cur->_value;
        return true;
    }
    return false;
}

template<typename K,typename V>
bool SkipList<K,V>::Erase(K key)
{
    int i=_maxLeve-1;
    int j=0;
    Node* cur=_head;                                     //指向跳表的起点
    Node* s[MAXLEVE];                                     //用来保存每层向下跳转位置的前驱
    while(i>=0)
    {
        while(cur->_pleve[i])
        {
            if(key>cur->_pleve[i]->_key)
            {
                cur=cur->_pleve[i];
            }
            else
                break;
        }
        s[j++]=cur;
        i--;
    }

    if(cur->_pleve[0]&&cur->_pleve[0]->_key==key)
    {
        cur=cur->_pleve[0];
        int leve=cur->_sz;
        Node *del=NULL;
        del=cur;
        for(i=0;i<leve;i++)                                //将该节点的每层都进行删除
        {
            s[j-i-1]->_pleve[i]=cur->_pleve[i];
        }
        delete del;

        while(_maxLeve>1)                                    //如果某一层只有头结点，则删除整层
        {
            if(NULL==_head->_pleve[_maxLeve-1])
                _maxLeve--;
            else
                break;
        }
        if(_size>0)
            _size--;
    }
    return false;
}

template<typename K,typename V>
void SkipList<K,V>::Print()
{
    int i=_maxLeve-1;
    while(i>=0)
    {
        Node* cur=_head;
        printf("this is %d leve:",i+1);
        while(cur)
        {
            cout<<cur->_key<<":"<<"["<<cur->_value<<"]"<<" ";
            cur=cur->_pleve[i];
        }
        i--;
        printf("\n");
    }
}

template<typename K,typename V>
SkipList<K,V>::~SkipList()
{
    Node* cur=_head;
    Node* del;
    while(cur)
    {
        del=cur;
        cur=cur->_pleve[0];
        delete del;
    }
    _head=NULL;
}


