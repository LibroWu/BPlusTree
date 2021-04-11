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
        size_t pre, next, parent, number;
        T Fence[M];
        size_t child[M + 1];

        crystalNode() : next(0), pre(0), parent(0), number(0), is_leaf(0) {}
    };

    class indexNode {
    public:
        size_t next, pre, parent, number;
        T v[L + 1];
        size_t index[L + 1];

        indexNode() : next(0), pre(0), parent(0) {}

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

    bool sub_insert() {

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
            crystalMemory.read(root, root_pos);
            size_t num = lower_bound(root.Fence, root.Fence + root.number - 1, t) - root.Fence;
            if (root.is_leaf) {
                indexNode ind;
                indexMemory.read(ind, root.child[num]);
                indexNode *tmp = ind.insert(t, index);
                if (tmp == nullptr)
                    indexMemory.update(ind, root.child[num]);
                else {
                    ind.next = indexMemory.write(*tmp);
                    indexMemory.update(ind, root.child[num]);
                    for (int i = root.number; i > num + 1; --i) {
                        root.child[i] = root.child[i - 1];
                        root.Fence[i - 1] = root.Fence[i - 2];
                    }
                    root.child[num + 1] = ind.next;
                    root.Fence[num] = tmp->v[0];
                    ++root.number;
                    //todo
                    if (root.number > M) {
                        crystalNode another, newRoot;
                        for (int i = 0; i < halfM; ++i) {
                            another.child[i] = root.child[i + M / 2 + 1];
                        }
                        another.number = halfM;
                        root.number = M / 2 + 1;
                    } else crystalMemory.update(root, root_pos);
                }
                delete tmp;
            } else {

            }
        }
    }

    void Delete(const T &t) {

    }

    std::vector<size_t> *Find(const T &t) {

    }
};


#endif //BPT_BPT_HPP
