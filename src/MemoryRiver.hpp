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
template<class T, size_t info_len = 2>
class MemoryRiver {
private:
    fstream file;
    string file_name;
    size_t sizeofT = sizeof(T);
public:
    MemoryRiver(string file_name) : file_name(file_name) {}

    void initialise() {
        file.open(file_name, std::ios::out);
        size_t tmp = sizeof(size_t) * info_len;
        file.write(reinterpret_cast<char *>(&tmp), sizeof(size_t));
        tmp = 0;
        for (int i = 1; i < info_len; ++i)
            file.write(reinterpret_cast<char *>(&tmp), sizeof(size_t));
        file.close();
    }

    void get_info(size_t &tmp, size_t n) {
        if (n > info_len) return;
        file.open(file_name);
        file.seekg(sizeof(size_t) * (n - 1));
        file.read(reinterpret_cast<char *>(&tmp), sizeof(size_t));
        file.close();
    }

    void write_info(size_t tmp, size_t n) {
        if (n > info_len) return;
        file.open(file_name);
        file.seekp(sizeof(size_t) * (n - 1));
        file.write(reinterpret_cast<char *>(&tmp), sizeof(size_t));
        file.close();
    }

    void write(const T &t, const size_t &index) {
        file.open(file_name);
        size_t pos, num;
        file.seekg(0);
        file.read(reinterpret_cast<char *>(&pos), sizeof(size_t));
        file.read(reinterpret_cast<char *>(&num), sizeof(size_t));
        //no empty node exists
        if (!num) {
            file.seekp(0);
            pos += sizeofT + sizeof(size_t);
            file.write(reinterpret_cast<char *>(&pos), sizeof(size_t));
            file.seekp(pos - sizeofT - sizeof(size_t));
            file.write(reinterpret_cast<char *>(&num), sizeof(size_t));
            file.write(reinterpret_cast<char *>(&t), sizeofT);
        }
        else {
            --num;
            file.seekp(pos);
            file.read(reinterpret_cast<char *>(&pos), sizeof(size_t));
            file.write(reinterpret_cast<char *>(&t), sizeofT);
            file.seekp(0);
            file.write(reinterpret_cast<char *>(&pos), sizeof(size_t));
            file.write(reinterpret_cast<char *>(&num), sizeof(size_t));
        }
        file.close();
    }

    void update(const T &t, const size_t &index) {
        file.open(file_name);
        file.seekp(index + sizeof(size_t));
        file.write(reinterpret_cast<char *>(&t), sizeofT);
        file.close();
    }

    void read(T &t, const size_t &index) {
        file.open(file_name);
        file.seekg(index + sizeof(size_t));
        file.read(reinterpret_cast<char *>(&t), sizeofT);
        file.close();
    }

    void Delete(size_t index) {
        size_t a, num;
        file.open(file_name);
        file.read(reinterpret_cast<char *>(&a), sizeof(size_t));
        file.read(reinterpret_cast<char *>(&num), sizeof(size_t));
        ++num;
        file.seekg(index);
        file.write(reinterpret_cast<char *>(&a), sizeof(size_t));
        file.seekp(0);
        file.write(reinterpret_cast<char *>(&index), sizeof(size_t));
        file.write(reinterpret_cast<char *>(&num), sizeof(size_t));
        file.close();
    }
};


#endif //BPT_MEMORYRIVER_HPP
