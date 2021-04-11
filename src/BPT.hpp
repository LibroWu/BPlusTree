//
// Created by Libro on 2021/4/9.
//

#ifndef BPT_BPT_HPP
#define BPT_BPT_HPP

#include "MemoryRiver.hpp"
#include <cstring>
#include <vector>
#include <algorithm>

using std::lower_bound;
//do not support 
template<class T, size_t M, size_t L, class Compare=std::less<T>>
class BPT {
private:
#define halfM (M-M/2)
#define halfL (L-L/2)
    //have branches
    //like light penetrates the crystal
    class crystalNode {
    public:
        bool is_leaf;
        size_t pre, next, parent, number;
        T Fence[M - 1];
        size_t child[M];

        crystalNode() : next(0), pre(0), parent(0), number(0), is_leaf(0) {}
    };

    class indexNode {
    public:
        size_t next, pre, parent, number;
        T v[L];
        size_t index[L];

        indexNode() : next(0), pre(0), parent(0) {}

        //true for split
        bool insert(const T &t, const size_t &ind){
            //todo
            size_t pos=lower_bound(v,v+number,t)-v;
            for (size_t i = number; i > pos; --i) {
                v[i]=v[i-1];
                index[i]=index[i-1];
            }
            v[pos]=t;
            index[pos]=ind;
            ++number;
            if (number>L) {
                indexNode newInd;
                for (size_t i = 0; i < halfL; ++i) {
                    newInd.index[i]=index[i+L/2];
                }
            }
        }
    };



    //the third info is for the position of the root
    MemoryRiver<crystalNode, 3> crystalMemory;
    MemoryRiver<indexNode, 2> indexMemory;

    bool sub_insert(){

    }

public:
    BPT() {}

    void initialise(const string &crystalFN, const string &indexFN) {
        crystalMemory.initialise(crystalFN);
        indexMemory.initialise(indexFN);
    }

    void insert(const T &t, const size_t &index) {
        size_t root_pos;
        indexMemory.get_info(root_pos, 3);
        crystalNode root;
        if (root_pos == 0) {
            indexNode Beg;
            Beg.index[0] = index;
            Beg.v[Beg.number++] = t;
            root.child[root.number++] = indexMemory.write(Beg);
            root.is_leaf = true;
            crystalMemory.write_info(crystalMemory.write(root), 3);
        }
        else {
            crystalMemory.read(root,root_pos);
            T* ptr=lower_bound(root.Fence,root.Fence+root.number-1,t);
            size_t num=ptr-root.Fence;
            if (root.is_leaf) {
                indexNode ind;
                indexMemory.read(ind,root.child[num]);


            }
            else {

            }
        }
    }

    void Delete(const T &t) {

    }

    std::vector<size_t> *Find(const T &t) {

    }
};


#endif //BPT_BPT_HPP
