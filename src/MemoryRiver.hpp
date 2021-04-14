//
// Created by Libro on 2021/4/9.
//

#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

#include <fstream>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

//the first info is for the head of empty node chain
//the second info is for the number of empty node in the chain
template<class T, unsigned int info_len = 2>
class MemoryRiver {
private:
    fstream file;
    string file_name;
    unsigned int sizeofT = sizeof(T);
public:
    MemoryRiver() = default;

    MemoryRiver(string file_name) : file_name(file_name) {}

    void initialise(string FN = "") {
        if (FN != "") file_name = FN;
        file.open(file_name, std::ios::out);
        unsigned int tmp = sizeof(unsigned int) * info_len;
        file.write(reinterpret_cast<char *>(&tmp), sizeof(unsigned int));
        tmp = 0;
        for (int i = 1; i < info_len; ++i)
            file.write(reinterpret_cast<char *>(&tmp), sizeof(unsigned int));
        file.close();
    }

    void get_info(unsigned int &tmp, unsigned int n) {
        if (n > info_len) return;
        file.open(file_name);
        file.seekg(sizeof(unsigned int) * (n - 1));
        file.read(reinterpret_cast<char *>(&tmp), sizeof(unsigned int));
        file.close();
    }

    void write_info(unsigned int tmp, unsigned int n) {
        if (n > info_len) return;
        file.open(file_name);
        file.seekp(sizeof(unsigned int) * (n - 1));
        file.write(reinterpret_cast<char *>(&tmp), sizeof(unsigned int));
        file.close();
    }

    unsigned int write(T &t) {
        file.open(file_name);
        unsigned int pos, num, r_index;
        file.seekg(0);
        file.read(reinterpret_cast<char *>(&pos), sizeof(unsigned int));
        r_index = pos;
        file.read(reinterpret_cast<char *>(&num), sizeof(unsigned int));
        //no empty node exists
        if (!num) {
            file.seekp(0);
            pos += sizeofT + sizeof(unsigned int);
            file.write(reinterpret_cast<char *>(&pos), sizeof(unsigned int));
            file.seekp(pos - sizeofT - sizeof(unsigned int));
            file.write(reinterpret_cast<char *>(&num), sizeof(unsigned int));
            file.write(reinterpret_cast<char *>(&t), sizeofT);
        }
        else {
            --num;
            file.seekp(pos);
            file.read(reinterpret_cast<char *>(&pos), sizeof(unsigned int));
            file.write(reinterpret_cast<char *>(&t), sizeofT);
            file.seekp(0);
            file.write(reinterpret_cast<char *>(&pos), sizeof(unsigned int));
            file.write(reinterpret_cast<char *>(&num), sizeof(unsigned int));
        }
        file.close();
        return r_index;
    }

    void update(T &t, const unsigned int &index) {
        file.open(file_name);
        file.seekp(index + sizeof(unsigned int));
        file.write(reinterpret_cast<char *>(&t), sizeofT);
        file.close();
    }

    void read(T &t, const unsigned int &index) {
        file.open(file_name);
        file.seekg(index + sizeof(unsigned int));
        file.read(reinterpret_cast<char *>(&t), sizeofT);
        file.close();
    }

    void Delete(unsigned int index) {
        unsigned int a, num;
        file.open(file_name);
        file.read(reinterpret_cast<char *>(&a), sizeof(unsigned int));
        file.read(reinterpret_cast<char *>(&num), sizeof(unsigned int));
        ++num;
        file.seekg(index);
        file.write(reinterpret_cast<char *>(&a), sizeof(unsigned int));
        file.seekp(0);
        file.write(reinterpret_cast<char *>(&index), sizeof(unsigned int));
        file.write(reinterpret_cast<char *>(&num), sizeof(unsigned int));
        file.close();
    }
};


#endif //BPT_MEMORYRIVER_HPP
