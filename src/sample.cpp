//
// Created by Libro on 2021/5/4.
//A sample for duplicate key

#include <iostream>
#include "../src/BPT.hpp"

using namespace std;

class ABC {
public:
    long long key, sec;

    ABC(long long key = 0, long long sec = 0) : key(key), sec(sec) {}

    bool operator<(const ABC &other) const {
        return (key < other.key || key == other.key && sec < other.sec);
    }

    bool operator==(const ABC &other) const {
        return (key==other.key && sec==other.sec);
    }
};
ostream& operator<<(ostream& out,const ABC &abc) {
    out<<"***"<<abc.key<<"|||"<<abc.sec<<"***";
    return out;
}
BPT<ABC, long long, 288, 288> bpt;

int main() {
    bpt.initialise("crystal.file", "index.file");
    int n = 100;
    for (int i = 1; i <= n; ++i) {
        bpt.insert(ABC(i, -1), -1);
        for (int j = 1; j <= i; ++j) {
            bpt.insert(ABC(i,j),j+i*100);
        }
    }
    //bpt.print();
    for (int i = 1; i <= n; ++i) {
        vector<long long int>* tmp=bpt.multipleFind(ABC(i,-1));
        for (int j = 0; j < tmp->size(); ++j) {
            cout<<tmp->operator[](j)<<' ';
        }
        cout<<'\n';
        delete tmp;
    }
}


