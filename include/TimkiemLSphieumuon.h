#ifndef HASHPHIEUMUON_H
#define HASHPHIEUMUON_H

#include "Phieumuon.h"

#define PM_HASH_SIZE 1000
typedef struct HashNodePM {
    PhieuMuonNode *LSphieumuon;
    struct HashNodePM *next;     // 
} HashNodePM;
typedef struct {
    HashNodePM *mang[PM_HASH_SIZE];
} PhieuMuonMang;

// 3. Khai báo các hàm giao tiếp
void InitPhieuMuonMang(PhieuMuonMang *map);
int HashMaThe(const char *mathe);
void InsertPhieuMuonMang(PhieuMuonMang *map, PhieuMuonNode *pmNode);
void BuildMap(PhieuMuonMang *map, PhieuMuonNode *head);
HashNodePM* Layphantu(PhieuMuonMang *map, const char *mathe);

#endif