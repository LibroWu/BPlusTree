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

//do not support duplicate key
//if wanting to support,make the chain doubly linked
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
        size_t number;
        T Fence[M];
        size_t child[M + 1];

        crystalNode() : number(0), is_leaf(0) {}
    };

    class indexNode {
    public:
        size_t next, pre, number;
        T v[L + 1];
        size_t index[L + 1];

        indexNode() : next(0), pre(0) {}

        //not nullptr if split
        indexNode *insert(const T &t, const size_t &ind) {
            size_t pos = lower_bound(v, v + number, t) - v;
            for (size_t i = number; i > pos; --i) {
                v[i] = v[i - 1];
                index[i] = index[i - 1];
            }
            v[pos] = t;
            index[pos] = ind;
            ++number;
            indexNode *newInd = nullptr;
            if (number > L) {
                //todo have a bug here, reach L
                //todo if reserve an empty storage in the end, then the coding would be much more enjoyable
                //todo I choose to enjoy the coding
                //todo why do I tag todo? Because of the highlight.
                newInd = new indexNode;
                for (size_t i = 0; i < halfL; ++i) {
                    newInd->index[i] = index[i + L / 2 + 1];
                    newInd->v[i] = v[i + L / 2 + 1];
                }
                newInd->next = next;
                newInd->number = halfL;
                number = L / 2 + 1;
            }
            return newInd;
        }
    };


    //the third info is for the position of the root
    MemoryRiver<crystalNode, 3> crystalMemory;
    MemoryRiver<indexNode, 2> indexMemory;

    struct Pair {
        T t;
        size_t pos;
    };

    Pair *sub_insert(const T &t, const size_t &index, const size_t &pos) {
        Pair *ptr = nullptr;
        crystalNode sub_root;
        crystalMemory.read(sub_root, pos);
        size_t num = lower_bound(sub_root.Fence, sub_root.Fence + sub_root.number - 1, t) - sub_root.Fence;
        if (sub_root.is_leaf) {
            indexNode ind;
            indexMemory.read(ind, sub_root.child[num]);
            indexNode *tmp = ind.insert(t, index);
            if (tmp == nullptr)
                indexMemory.update(ind, sub_root.child[num]);
            else {
                ind.next = indexMemory.write(*tmp);
                indexMemory.update(ind, sub_root.child[num]);
                for (int i = sub_root.number; i > num + 1; --i) {
                    sub_root.child[i] = sub_root.child[i - 1];
                    sub_root.Fence[i - 1] = sub_root.Fence[i - 2];
                }
                sub_root.child[num + 1] = ind.next;
                sub_root.Fence[num] = tmp->v[0];
                ++sub_root.number;
                //the sub_root splits
                if (sub_root.number > M) {
                    crystalNode another, newRoot;
                    for (int i = 0; i < halfM; ++i)
                        another.child[i] = sub_root.child[i + M / 2 + 1];
                    for (int i = 0; i < halfM - 1; ++i)
                        another.Fence[i] = sub_root.Fence[i + M / 2 + 1];
                    newRoot.number = 2;
                    newRoot.Fence[0] = sub_root.Fence[M / 2];
                    another.number = halfM;
                    sub_root.number = M / 2 + 1;
                    newRoot.child[0] = pos;
                    newRoot.is_leaf = false;
                    sub_root.is_leaf = true;
                    another.is_leaf = true;
                    crystalMemory.update(sub_root, pos);
                    newRoot.child[1] = crystalMemory.write(another);
                    crystalMemory.write_info(crystalMemory.write(newRoot), 3);
                } else crystalMemory.update(sub_root, pos);
                delete tmp;
            }
        } else {
            Pair* tmp = sub_insert(t,index,sub_root.child[num]);
            if (tmp) {
                for (int i = 0; i < ; ++i) {

                }
                if (sub_root.number>M) {

                    ptr=new Pair;
                }
            }
        }
        return ptr;
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
        } else {
            Pair *tmp = sub_insert(t, index, root_pos);
            if (tmp) {
                crystalNode newRoot;
                newRoot.child[0] = root_pos;
                newRoot.child[1] = tmp->pos;
                newRoot.Fence[0] = tmp->t;
                newRoot.number=2;
                crystalMemory.write_info(crystalMemory.write(newRoot),3);
                delete tmp;
            }
        }
    }

    void Delete(const T &t) {

    }

    std::vector<size_t> *Find(const T &t) {

    }
};


#endif //BPT_BPT_HPP
