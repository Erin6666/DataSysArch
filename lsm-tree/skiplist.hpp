#ifndef _SKIPLIST_H__
#define _SKIPLIST_H__
#include<time.h>
#include<iostream>
#include<vector>
#include<cstdio>
#include<cstdlib>
using namespace std;
#define MAXLEVE 16                                             //�����������

template<typename K,typename V>
struct SkipNode                                               //����Ľڵ�����
{
   K _key;
   V _value;
   size_t _sz;                                                //��ʾ�ýڵ�Ĳ���
   vector<SkipNode<K,V> *> _pleve;                            //���ÿһ���ָ��
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
class SkipList                                               //������
{
public:
    typedef SkipNode<K,V> Node;
    SkipList();
    void Insert(K key,V value);
    bool Find(K key,V& value);
    bool Erase(K key);
    void Print();
    int GetLeve();                                            //���������������
    size_t Size();
    ~SkipList();
private:
    int Random();                                             //������������ĺ���
protected:
    SkipList(SkipList<K,V> &);                                //������
    SkipList<K,V>& operator=(SkipList<K,V>);                  //����ֵ
private:
    Node *_head;
    int _maxLeve;                                             //��¼�����������
    int _size;                                                //��¼������ײ�Ԫ�صĸ���
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
    while(rand()%2&&leve<=MAXLEVE)                 //����0��1,1�Ļ�leve++,���ƽ������leve��ֵ��������̬�ֲ�
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
    Node* cur=_head;                                     //ָ����������
    Node* s[MAXLEVE];
    if(Find(key,temp))
    {
        Erase(key);
    }


    while(i>=0)                                     //��������ÿ��������תλ�õ�ǰ��
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
    int leve=Random();                                    //����һ���������
    _maxLeve<leve?_maxLeve=leve:_maxLeve;                 //���������������
    Node* newNode=new Node(key,value,leve);                  //����һ���ڵ�
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
    Node* cur=_head;                                     //ָ����������
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
    if(cur->_key==key)                                      //��ò��÷º������бȽ�
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
    Node* cur=_head;                                     //ָ����������
    Node* s[MAXLEVE];                                     //��������ÿ��������תλ�õ�ǰ��
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
        for(i=0;i<leve;i++)                                //���ýڵ��ÿ�㶼����ɾ��
        {
            s[j-i-1]->_pleve[i]=cur->_pleve[i];
        }
        delete del;

        while(_maxLeve>1)                                    //���ĳһ��ֻ��ͷ��㣬��ɾ������
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


