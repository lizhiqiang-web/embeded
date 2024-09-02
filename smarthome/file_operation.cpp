#include "file_operation.h"
bool file_add(people p){
    int n;
    FILE*f=fopen(FPATH,"wr");
    if(f==NULL){
        perror("open faulse");
        return 0;
    }
    fseek(f,0,SEEK_SET);
    fscanf(f,"%d",n);
    fseek(f,0,SEEK_END);
    fprintf(f,"%s%s%s",p.id,p.nmae,p.tele_num);
     n++;
    fseek(f,0,SEEK_SET);
    fprintf(f,"%d",n);
}
bool file_del(int k){
    people p;
    int n;
    FILE*f=fopen(FPATH,"wr");
    if(f==NULL){
        perror("open faulse");
        return 0;
    }
    fseek(f,0,SEEK_SET);
    fscanf(f,"%d",n);
    fseek(f,sizeof(int)+k*sizeof(people),SEEK_SET);
    for(int i=0;i<n-k-1;i++){
        fscanf(f,"%s%s%s",p.id,p.nmae,p.tele_num);
        fseek(f,-(sizeof(people)),SEEK_CUR);
        fprintf(f,"%s%s%s",p.id,p.nmae,p.tele_num);
        fseek(f,sizeof(people),SEEK_CUR);
    }
    n--;
   fseek(f,0,SEEK_SET);
   fprintf(f,"%d",n);
    return 1;

}
people& file_fine(int k){
    people p;
    FILE*f=fopen(FPATH,"wr");
    if(f==NULL){
        perror("open faulse");
        return NULL;
    }
    fseek(f,sizeof(int)+k*sizeof(people),SEEK_SET);
    fscanf(f,"%s%s%s",p.id,p.nmae,p.tele_num);
    return &p;
}
bool file_check(people k){
    people p;
    int n;
    FILE*f=fopen(FPATH,"wr");
    if(f==NULL){
        perror("open faulse");
        return 0;
    }
    fscanf(f,"%d",n);
    for(int i=0;i<n;i++){
        fscanf(f,"%s%s%s",p.id,p.nmae,p.tele_num);
        if(strcmp(p.id,k.id))
            return 1;
    }
    return  0;
}
