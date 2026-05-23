#include "TimkiemLSphieumuon.h"
#include <stdlib.h>
#include <string.h>
void InitPhieuMuonMap(PhieuMuonMang *map) {
    for (int i = 0; i < PM_HASH_SIZE; i++) {
        map->mang[i] = NULL;
    }
}
int HashMaThe(const char *mathe) {
    int id = atoi(mathe);
    return id % PM_HASH_SIZE;
}
void InsertPhieuMuonMang(PhieuMuonMang *map, PhieuMuonNode *pmNode) {
    if (pmNode == NULL) return;
    int index = HashMaThe(pmNode->mathe);
    HashNodePM *newNode = (HashNodePM*)malloc(sizeof(HashNodePM));
    newNode->LSphieumuon = pmNode;
    newNode->next = map->mang[index];
    map->mang[index] = newNode;
}
void BuildMap(PhieuMuonMang *map, PhieuMuonNode *head) {
    InitPhieuMuonMap(map); 
    PhieuMuonNode *hientai = head;
    while (hientai != NULL) {
        InsertPhieuMuonToMap(map, hientai);
        hientai = hientai->next;
    }
}
HashNodePM* Layphantu(PhieuMuonMang *map, const char *mathe) {
    int index = HashMaThe(mathe);
    return map->mang[index]; 
}
