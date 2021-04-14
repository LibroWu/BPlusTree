//
// Created by Libro on 2021/4/9.
//

#ifndef BPT_BPT_HPP
#define BPT_BPT_HPP
#define debugs

#ifdef debugs

#include <iostream>

using std::cin;
using std::cout;
#endif

#include "MemoryRiver.hpp"
#include <cstring>
#include <vector>
#include <algorithm>

using std::lower_bound;

//do not support duplicate key
//if wanting to support,make the chain doubly linked
template<class T, int M, int L, class Compare=std::less<T>>
class BPT {
private:
#define halfM (M-M/2)
#define halfL (L-L/2)

    //have branches
    //like light penetrates the crystal
    class crystalNode {
    public:
        bool is_leaf;
        int number;
        T Fence[M + 1];
        int child[M + 2];

        crystalNode() : number(0), is_leaf(0) {}

#ifdef debugs

        void print() {
            cout << "\n----" << number << "----\n";
            for (int i = 0; i < number - 1; ++i) {
                cout << Fence[i] << ' ';
            }
            cout << "\n|||||||||||||||||||||||\n";
            for (int i = 0; i < number; ++i) {
                cout << child[i] << ' ';
            }
            cout << "\n-----------------\n";
        }

#endif
    };

    class indexNode {
    public:
        int next, pre, number;
        T v[L + 1];
        int index[L + 1];

        indexNode() : number(0), next(0), pre(0) {}

        //not nullptr if split
        indexNode *insert(const T &t, const int &ind) {
            int pos = lower_bound(v, v + number, t) - v;
            for (int i = number; i > pos; --i) {
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
                newInd = new indexNode;
                for (int i = 0; i < halfL; ++i) {
                    newInd->index[i] = index[i + L / 2 + 1];
                    newInd->v[i] = v[i + L / 2 + 1];
                }
                newInd->next = next;
                newInd->number = halfL;
                number = L / 2 + 1;
            }
            return newInd;
        }

#ifdef debugs

        void print() {
            cout << "\n***" << number << "***\n";
            for (int i = 0; i < number; ++i) {
                cout << "---" << v[i] << ' ' << index[i] << "---\n";
            }
            cout << "\n******\n";
        }

#endif
    };


    //the third info is for the position of the root
    MemoryRiver<crystalNode, 3> crystalMemory;
    MemoryRiver<indexNode, 2> indexMemory;

    Compare cmp;

    struct Pair {
        T t;
        int pos;
    };

    Pair *sub_insert(const T &t, const int &index, const int &pos) {
        Pair *ptr = nullptr;
        crystalNode sub_root;
        crystalMemory.read(sub_root, pos);
        int num = lower_bound(sub_root.Fence, sub_root.Fence + sub_root.number - 1, t) - sub_root.Fence;
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
                if (sub_root.number > M + 1) {
                    crystalNode another, n;
                    for (int i = 0; i < halfM + 1; ++i)
                        another.child[i] = sub_root.child[i + M / 2 + 1];
                    for (int i = 0; i < halfM; ++i)
                        another.Fence[i] = sub_root.Fence[i + M / 2 + 1];
                    ptr = new Pair;
                    ptr->t = sub_root.Fence[M / 2];
                    another.number = halfM + 1;
                    sub_root.number = M / 2 + 1;
                    sub_root.is_leaf = true;
                    another.is_leaf = true;
                    crystalMemory.update(sub_root, pos);
                    ptr->pos = crystalMemory.write(another);
                }
                else crystalMemory.update(sub_root, pos);
                delete tmp;
            }
        }
        else {
            Pair *tmp = sub_insert(t, index, sub_root.child[num]);
            if (tmp) {
                for (int i = sub_root.number; i > num + 1; --i) {
                    sub_root.child[i] = sub_root.child[i - 1];
                    sub_root.Fence[i - 1] = sub_root.Fence[i - 2];
                }
                sub_root.Fence[num] = tmp->t;
                sub_root.child[num + 1] = tmp->pos;
                sub_root.number++;
                if (sub_root.number > M + 1) {
                    crystalNode another, n;
                    for (int i = 0; i < halfM + 1; ++i)
                        another.child[i] = sub_root.child[i + M / 2 + 1];
                    for (int i = 0; i < halfM; ++i)
                        another.Fence[i] = sub_root.Fence[i + M / 2 + 1];
                    ptr = new Pair;
                    ptr->t = sub_root.Fence[M / 2];
                    another.number = halfM + 1;
                    sub_root.number = M / 2 + 1;
                    sub_root.is_leaf = false;
                    another.is_leaf = false;
                    crystalMemory.update(sub_root, pos);
                    ptr->pos = crystalMemory.write(another);
                }
                else crystalMemory.update(sub_root, pos);
                delete tmp;
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

    void insert(const T &t, const int &index) {
        int root_pos;
        crystalMemory.get_info(root_pos, 3);
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
            Pair *tmp = sub_insert(t, index, root_pos);
            if (tmp) {
                crystalNode newRoot;
                newRoot.child[0] = root_pos;
                newRoot.child[1] = tmp->pos;
                newRoot.Fence[0] = tmp->t;
                newRoot.number = 2;
                crystalMemory.write_info(crystalMemory.write(newRoot), 3);
                delete tmp;
            }
        }
    }

    void Delete(const T &t) {

    }

    int Find(const T &t) {
        int pos, num;
        crystalMemory.get_info(pos, 3);
        crystalNode tmp;
        while (pos > 0) {
            crystalMemory.read(tmp, pos);
            num = lower_bound(tmp.Fence, tmp.Fence + tmp.number - 1, t) - tmp.Fence;
            if (tmp.is_leaf) {
                indexNode ind;
                indexMemory.read(ind, tmp.child[num]);
                int N = lower_bound(ind.v, ind.v + ind.number, t) - ind.v;
                if (ind.v[N] == t) return ind.index[N];
                //not found
                return 0;
            }
            pos = tmp.child[num];
        }
        return 0;
    }

#ifdef debugs

    void print() {
        int l = 0, r = 0, que[10000];
        bool flag[10000] = {0};
        crystalMemory.get_info(que[r++], 3);
        int pos;
        while (l < r) {
            pos = que[l++];
            if (flag[l - 1]) {
                indexNode t;
                indexMemory.read(t, pos);
                t.print();
                cout<<'\n';
            }
            else {
                crystalNode t;
                crystalMemory.read(t, pos);
                t.print();
                cout<<'\n';
                for (int i = 0; i < t.number; ++i) {
                    flag[r]=t.is_leaf;
                    que[r++] = t.child[i];
                }
            }
        }
    }

#endif
};


#endif //BPT_BPT_HPP
