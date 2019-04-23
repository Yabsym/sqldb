//
// Created by wsl on 19-4-15.
//
#include "dmap.h"

//构造函数
DBMap::DBMap(size_t s) : Object(DBMAP_OBJECT) {
    rehash = -1;
    ht_temp = new HashTable(0);
    ht = new HashTable(s);
}

DBMap::~DBMap() {
    std::cout << "delete DBMap" << "\n";
    delete ht;
    delete ht_temp;
}

HashEntry::HashEntry(DBString *key, Object *value) {
    this->key = key;
    this->value = value;
}

HashEntry::~HashEntry() {
    std::cout << "delete HashEntry" << std::endl;
    delete key;
    delete value;
}

HashTable::HashTable(size_t s) {
    used = 0;
    size = s;
    entrys = new HashEntry *[size];
    for (int i = 0; i < size; i++) {
        entrys[i] = nullptr;
    }
}

HashTable::~HashTable() {
    std::cout << "delete ht" << std::endl;
    for (int i = 0; (i < size) && (used > 0); i++) {
        if (entrys[i] != nullptr) {
            delete entrys[i];
            used -= 1;
        }
    }
}

//成员函数
size_t DBMap::hashFunc(std::string key) {
    return hasher(key) & (ht->size - 1);
}


// 向哈希表中添加数据的时候, 如果有Key则只改变对应的value
void DBMap::insert(std::string key, Object *value) {
    rehashCheck();

    size_t index = hashFunc(key);
    DBString *key_p = new DBString(key);

    HashTable *htp = rehash == -1 ? ht : ht_temp;

    if (htp->entrys[index] == nullptr) {
        htp->entrys[index] = new HashEntry(key_p, value);
    } else {
        HashEntry *p = htp->entrys[index];
        HashEntry *prev = p;
        while (p != nullptr) {
            // check key
            if (p->key->buff == key) {
                delete p->value;
                p->value = value;
                return;
            }
            prev = p;
            p = p->next;
        }
        prev->next = new HashEntry(key_p, value);
    }
    // used+1
    htp->used += 1;
}

void DBMap::traversal() {
    std::cout << "size: " << ht->size << " used: " << ht->used << std::endl;
    for (int i = 0; i < ht->size; i++) {
        if (ht->entrys[i] == nullptr) {
            std::cout << i << "---> NULL" << std::endl;
        } else {
            std::cout << i;
            auto *p = ht->entrys[i];
            while (p != nullptr) {
                std::cout << "---> " << "( " << p->key->buff << ", " << p->value->type << " )";
                p = p->next;
            }
            std::cout << std::endl;
        }
    }
}


// 字典 get(key) 操作
std::string DBMap::get(std::string key) {
    size_t index = hashFunc(key);
    if (ht->entrys[index] == nullptr) {
        return nullptr;
    } else {
        auto *p = ht->entrys[index];
        while (p != nullptr) {
            if (p->key->buff == key) {
                return p->value->values();
            } else {
                p = p->next;
            }
        }
        return "";
    }
}

std::string DBMap::values() {
    return "Map values";
}

// get all the key of the dict
void DBMap::keys() {
    int i = 0;
    int num = 1;
    for (; i < ht->size; i++) {
        if (ht->entrys[i] != nullptr) {
            auto *p = ht->entrys[i];
            while (p != nullptr) {
                std::cout << num++ << "): " << p->key->buff << std::endl;
                p = p->next;
            }
        }
    }
}

//rehash check
void DBMap::rehashCheck() {
    if ((ht->used / (ht->size * 1.0)) >= 0.75) {
        std::cout<<"\n冲突因子大于0.75, 即将重新哈希...."<<std::endl;
        rehash = 0;
        rehashFunction();
    }
}

//重新哈希
void DBMap::rehashFunction() {
    if (rehash != 0) {
        return;
    }

    size_t resize = ht->size < 5000 ? (ht->size * 4) : (ht->size) * 2;
    ht_temp->entrys = new HashEntry *[resize];
    ht_temp->size = resize;
    std::cout<<"正在重新哈希, resize: "<<resize<<std::endl;

    for (int i = 0; i < ht_temp->size; i++) {
        ht_temp->entrys[i] = nullptr;

        if (i < (ht->size) && (ht->entrys[i] != nullptr)) {
            ht_temp->entrys[i] = ht->entrys[i];
            ht_temp->used += 1;
            ht->entrys[i] = nullptr;
        }
    }

    ht->size = 0;
    ht->used = 0;

    auto *temp = ht;
    ht = ht_temp;
    ht_temp = temp;
    rehash = -1;
}

bool DBMap::has_key(std::string key) {
    size_t index = hashFunc(key);
    if(ht->entrys[index]==nullptr){
        return false;
    }
    return true;
}