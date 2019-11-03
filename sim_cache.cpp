#include<bits/stdc++.h> 
using namespace std;
int writebacks=0;
int stringbinaryindextointeger(string indexes){
    if(indexes.length() == 0)
        return 0;
    return stoi(indexes,0,2);
}
struct block{
    int db,sb,vb;
};
struct cachesetl2{
    block* sector=NULL;
    string* tagstore=NULL;
    int counter;
    cachesetl2(int c, int n,int p){
        counter = c;
        tagstore= new string[p];
        for(int i=0;i<n;i++){
            this->tagstore[i]="";
        }
        sector=new block[p];
        for(int i=0;i<p;i++){
            this->sector[i].db=0;
            this->sector[i].vb=0;
            this->sector[i].sb=0;
        }
    }
};
class arrayofcachesetl2{
    public:
    vector<cachesetl2> set;
    arrayofcachesetl2(int l2asso, int n, int p){
        for(int i=0;i<l2asso;i++){
            set.push_back(cachesetl2(i, n , p));
        }
    }
};
struct cachel2{
    int blocksize;
    int l2capacity;
    int l2asso;
    int blocksinasector;
    int numberoftags;
    int numberofcompletesets;
    int l2readsmiss=0;
    int l2reads=0;
    int l2sectormisses=0;
    int cacheblockmiss=0;
    int l2writes=0;
    int l2writemiss=0;
    int l2writeback=0;
    vector<arrayofcachesetl2> completeset;
    cachel2(int capacity,int p,int n,int sizeofblock, int associativity){
        this->blocksize=sizeofblock;
        this->l2capacity=capacity;
        this->blocksinasector=p;
        this->numberoftags=n;
        this->l2asso=associativity;
        numberofcompletesets=l2capacity/(blocksize*p*l2asso);
        for(int i=0;i<numberofcompletesets;i++){
            completeset.push_back(arrayofcachesetl2(this->l2asso, this->numberoftags, this->blocksinasector));
        }
    }
    public:

    string decodetagl2(string binadd){
        int i;
        int numoffsetbits=log2(blocksize);
        int c0=log2(blocksinasector);
        int c1=log2(l2capacity/(blocksinasector*blocksize*l2asso));
        int c2=log2(numberoftags);
        int c3=32-(numoffsetbits+c0+c1+c2);
        char positionblockinsector[c0];
        char setindex[c1];
        char whichtag[c2];
        string tag=binadd.substr(0,c3);
        return tag;
    }
    string indexoftag(string binadd){
        int i;
        int numoffsetbits=log2(blocksize);
        int c0=log2(blocksinasector);
        int c1=log2(l2capacity/(blocksinasector*blocksize*l2asso));
        int c2=log2(numberoftags);
        int c3=32-(numoffsetbits+c0+c1+c2);
        string tag=binadd.substr(0,c3);
        string whichtag=binadd.substr(c3,c2);
        string setindex=binadd.substr(c3+c2,c1);
        string positionblockinsector=binadd.substr(c3+c2+c1,c0);
        return whichtag;
    }
    string indexofset(string binadd){
       int i;
        int numoffsetbits=log2(blocksize);
        int c0=log2(blocksinasector);
        int c1=log2(l2capacity/(blocksinasector*blocksize*l2asso));
        int c2=log2(numberoftags);
        int c3=32-(numoffsetbits+c0+c1+c2);
        string tag=binadd.substr(0,c3);
        string whichtag=binadd.substr(c3,c2);
        string setindex=binadd.substr(c3+c2,c1);
        string positionblockinsector=binadd.substr(c3+c2+c1,c0);
        return setindex;
    }
    string indexofblockwithinsector(string binadd){
       int i;
        int numoffsetbits=log2(blocksize);
        int c0=log2(blocksinasector);
        int c1=log2(l2capacity/(blocksinasector*blocksize*l2asso));
        int c2=log2(numberoftags);
        int c3=32-(numoffsetbits+c0+c1+c2);
        string tag=binadd.substr(0,c3);
        string whichtag=binadd.substr(c3,c2);
        string setindex=binadd.substr(c3+c2,c1);
        string positionblockinsector=binadd.substr(c3+c2+c1,c0);
        return positionblockinsector;
    }
    
    //find least used sector
    int findleastusedsector(int setindex){
        int lrusector=-1;
        int lrusectorindex=-1;
        for(int i=0;i<l2asso;i++){
            if(this->completeset[setindex].set[i].counter>lrusector){
                lrusector=this->completeset[setindex].set[i].counter;
                lrusectorindex=i;
            }
        }
        return lrusectorindex;
    }

    //Change counters
    void changecounters(int setindex,int sectorindex){
        //  cout<<"L2 update LRU\n";
        //  for(int i=0;i<l2asso;i++)
        //         cout<<this->completeset[setindex].set[i].counter<<" ";
        //     cout<<"->"<<sectorindex;
        //     cout<<endl;
        for(int i=0;i<l2asso;i++){
            if(this->completeset[setindex].set[i].counter < this->completeset[setindex].set[sectorindex].counter){
                this->completeset[setindex].set[i].counter++;
            }
        }
        this->completeset[setindex].set[sectorindex].counter=0;
        // for(int i=0;i<l2asso;i++)
        //         cout<<this->completeset[setindex].set[i].counter<<" ";
        //     cout<<endl;
    }
    //findfirstemptytagstore
    int findfirstemptytagstore(int setindex,int blockindex,int tagindex, string tag){
        for(int i=0;i<l2asso;i++){
                if((this->completeset[setindex].set[i].tagstore[tagindex]=="" 
                || this->completeset[setindex].set[i].tagstore[tagindex]==tag)
                && this->completeset[setindex].set[i].sector[blockindex].vb==0){
                    return i;
                }
        }
        return -1;
    }

    //for counting number of sector misses
    bool checkifsectormiss(int setindex,int sectorindex){
        for(int i=0;i<blocksinasector;i++){
            if(this->completeset[setindex].set[sectorindex].sector[i].vb==1){
                return false;
            }
        }
        return true;
    }

    void printl2cacheset(int i) {
        cout<<"set  "<<dec<<i<<" :"<<" ";
        sort(&(this->completeset[i].set[0]), &(this->completeset[i].set[0+l2asso]), [](cachesetl2 const & a, cachesetl2 const & b) -> bool{ return a.counter < b.counter; } );
        for(int j=0;j<l2asso;j++){
            if(this->completeset[i].set[j].tagstore[0]!=""){
                cout<<hex<<stoi(this->completeset[i].set[j].tagstore[0],0,2)<<" ";
            } else {
                cout<<"0 ";
            }
           
                
                if(this->completeset[i].set[j].sector[0].db==1){
                    cout<<"D ";
                }
                else{
                    cout<<"N ";
                }
           
            cout<<" || ";
        }
        cout<<"\n";
    }


    void readfroml2(string binadd){    
        l2reads++;
        int blockindex =0; int tagindex=0;
        int setindex=stoi(indexofset(binadd),0,2);
        if(blocksinasector!=1){
            blockindex=stoi(indexofblockwithinsector(binadd),0,2);
            }
        // printl2cacheset(setindex);
        string tag=decodetagl2(binadd);
        if(numberoftags!=1){
        tagindex=stoi(indexoftag(binadd),0,2);
        }
        int hitflag=0; int hitindexofsector;
        //hit
        //  cout<<"L2 read: "<<hex<<stringbinaryindextointeger(binadd)<<" (C0 "<<blockindex<<", C1 "<<setindex<<", C2 "<<tagindex<<", C3 "<<stringbinaryindextointeger(tag)<<")\n";
        //  cout<<dec;
        
        for(int i=0;i<l2asso;i++){
            if(this->completeset[setindex].set[i].sector[blockindex].vb!=0 &&
            this->completeset[setindex].set[i].sector[blockindex].sb==tagindex  
            && this->completeset[setindex].set[i].tagstore[tagindex]==tag){
            hitflag=1;
            //  cout<<"L2 hit\n";
            hitindexofsector=i;
            changecounters(setindex,hitindexofsector);
            break;
            }
        }
        //misses
        if(!hitflag){
            //  cout<<"L2 miss\n";
            l2readsmiss++;
            int insertatthisset=findfirstemptytagstore(setindex,blockindex,tagindex,tag);
            if(insertatthisset!=-1){                                            //no eviction required
                if(checkifsectormiss(setindex,insertatthisset)){                //sectormisses
                    l2sectormisses++;
                }
                else{
                   cacheblockmiss++;
                }
                this->completeset[setindex].set[insertatthisset].tagstore[tagindex]=tag;
                this->completeset[setindex].set[insertatthisset].sector[blockindex].vb=1;
                this->completeset[setindex].set[insertatthisset].sector[blockindex].sb=tagindex;
                changecounters(setindex, insertatthisset);
            }

            else{                                                                                              //eviction required
                cacheblockmiss++;
                int leastusedsectorindex=findleastusedsector(setindex);
                for(int i=0;i<blocksinasector && i!=blockindex;i++){                                                           //clear sector
                        if(this->completeset[setindex].set[leastusedsectorindex].sector[i].vb==1 &&           //if the block being set invalid is dirty, do a writeback
                           this->completeset[setindex].set[leastusedsectorindex].sector[i].sb==tagindex &&
                           this->completeset[setindex].set[leastusedsectorindex].tagstore[tagindex]!=tag){
                            if(this->completeset[setindex].set[leastusedsectorindex].sector[i].db==1){
                                l2writeback++;
                            }
                            this->completeset[setindex].set[leastusedsectorindex].sector[i].vb=0;
                            this->completeset[setindex].set[leastusedsectorindex].sector[i].db=0;
                            this->completeset[setindex].set[leastusedsectorindex].sector[i].sb=0;
                        }
                    }
                if(this->completeset[setindex].set[leastusedsectorindex].sector[blockindex].db==0){                // block clean
                    this->completeset[setindex].set[leastusedsectorindex].tagstore[tagindex]=tag;                 //insert tag at specified tag index
                    this->completeset[setindex].set[leastusedsectorindex].sector[blockindex].vb=1;                //set vb
                    this->completeset[setindex].set[leastusedsectorindex].sector[blockindex].sb=tagindex;         // inser tagindex in sb of specified block   

                }
                else{                                                                                          
                                l2writeback++;
                    this->completeset[setindex].set[leastusedsectorindex].sector[blockindex].vb=1;                //make referenced block valid
                    this->completeset[setindex].set[leastusedsectorindex].tagstore[tagindex]=tag;                 // insert tag in the decoded tagindex
                    this->completeset[setindex].set[leastusedsectorindex].sector[blockindex].sb=tagindex;         //insert that tagindex in the referenced block's selection bits
                    this->completeset[setindex].set[leastusedsectorindex].sector[blockindex].db=0;              //make the block clean
                }
                changecounters(setindex,leastusedsectorindex);
            }
        }
        // printl2cacheset(setindex);
    }

    void writetol2(string binadd){
        l2writes++;
        int blockindex =0; int tagindex=0;
        int setindex=stoi(indexofset(binadd),0,2);
        if(blocksinasector!=1){
            blockindex=stoi(indexofblockwithinsector(binadd),0,2);
        }
        // printl2cacheset(setindex);
        string tag=decodetagl2(binadd);
        if(numberoftags!=1){
            tagindex=stoi(indexoftag(binadd),0,2);
        }
        int hitflag=0; int hitindexofsector;
        //  cout<<"L2 write: "<<hex<<" (C0 "<<blockindex<<", C1 "<<setindex<<", C2 "<<tagindex<<", C3 "<<stringbinaryindextointeger(tag)<<")\n";
        //  cout<<dec;
        //hit
        for(int i=0;i<l2asso;i++){
            if(this->completeset[setindex].set[i].sector[blockindex].vb!=0 &&
            this->completeset[setindex].set[i].sector[blockindex].sb==tagindex  
            && this->completeset[setindex].set[i].tagstore[tagindex]==tag){
                hitflag=1;
                //  cout<<"L2 hit\n";
                hitindexofsector=i;
                changecounters(setindex,hitindexofsector);
                this->completeset[setindex].set[hitindexofsector].sector[blockindex].db=1;
                break;
            }
        }

        //misses
        if(!hitflag){
            l2writemiss++;
            //  cout<<"L2 miss\n";
            int insertatthisset=findfirstemptytagstore(setindex,blockindex,tagindex,tag);
            if(insertatthisset!=-1){                                            //eviction not required
                if(checkifsectormiss(setindex,insertatthisset)){                    //sectormiss
                    l2sectormisses++;
                }
                else{
                    cacheblockmiss++;
                }
                this->completeset[setindex].set[insertatthisset].tagstore[tagindex]=tag;
                this->completeset[setindex].set[insertatthisset].sector[blockindex].vb=1;
                this->completeset[setindex].set[insertatthisset].sector[blockindex].sb=tagindex;
                this->completeset[setindex].set[insertatthisset].sector[blockindex].db=1;
                changecounters(setindex, insertatthisset);
            }
            else{                                                                                               // eviction required
                cacheblockmiss++;
                int leastusedsectorindex=findleastusedsector(setindex);
                for(int i=0;i<blocksinasector && i!=blockindex;i++){                                                           //clear sector
                        if(this->completeset[setindex].set[leastusedsectorindex].sector[i].vb==1 &&           //if the block being set invalid is dirty, do a writeback
                           this->completeset[setindex].set[leastusedsectorindex].sector[i].sb==tagindex
                           && this->completeset[setindex].set[leastusedsectorindex].tagstore[tagindex]!=tag){
                            if(this->completeset[setindex].set[leastusedsectorindex].sector[i].db==1){
                                l2writeback++;
                            }
                            this->completeset[setindex].set[leastusedsectorindex].sector[i].vb=0;
                            this->completeset[setindex].set[leastusedsectorindex].sector[i].db=0;
                            this->completeset[setindex].set[leastusedsectorindex].sector[i].sb=0;
                        }
                    }
                if( this->completeset[setindex].set[leastusedsectorindex].sector[blockindex].db==0){
                    this->completeset[setindex].set[leastusedsectorindex].tagstore[tagindex]=tag;
                    this->completeset[setindex].set[leastusedsectorindex].sector[blockindex].vb=1;
                    this->completeset[setindex].set[leastusedsectorindex].sector[blockindex].sb=tagindex;
                    this->completeset[setindex].set[leastusedsectorindex].sector[blockindex].db=1;

                }
                else{                                                                                           
                    l2writeback++;
                    this->completeset[setindex].set[leastusedsectorindex].sector[blockindex].vb=1;                //make referenced block valid
                    this->completeset[setindex].set[leastusedsectorindex].tagstore[tagindex]=tag;                 // insert tag in the decoded tagindex
                    this->completeset[setindex].set[leastusedsectorindex].sector[blockindex].sb=tagindex;         //insert that tagindex in the referenced block's selectionbits
                    this->completeset[setindex].set[leastusedsectorindex].sector[blockindex].db=1;              //make the block clean
                }
                changecounters(setindex, leastusedsectorindex);
            }
        }
        // printl2cacheset(setindex);
    }

    void printcache(){
        cout<<"\n";
        cout<<"===== L2 Address Array contents ====="<<endl;
        for(int i=0;i<numberofcompletesets;i++){
            cout<<"set  "<<dec<<i<<" :"<<" ";
            for(int j=0;j<l2asso;j++){
                for(int k=0;k<numberoftags;k++){
                    if(this->completeset[i].set[j].tagstore[k]!=""){
                    cout<<hex<<stoi(this->completeset[i].set[j].tagstore[k],0,2)<<"    ";
                    }
                    else{
                        cout<<0<<"    ";
                    }
                }
            }
            cout<<"                  ||"<<endl;
        }
        cout<<endl<<"===== L2 Data Array contents ====="<<endl;
        for(int i=0;i<numberofcompletesets;i++){
            cout<<"set  "<<dec<<i<<" :"<<" ";
            sort(&(this->completeset[i].set[0]), &(this->completeset[i].set[0+l2asso]), [](cachesetl2 const & a, cachesetl2 const & b) -> bool{ return a.counter < b.counter; } );
            for(int j=0;j<l2asso;j++){
                for(int k=0;k<blocksinasector;k++){
                    cout<< this->completeset[i].set[j].sector[k].sb<<",";
                    if(this->completeset[i].set[j].sector[k].vb==1){
                        cout<<"V,";
                    }
                    else{
                        cout<<"I,";
                    }
                    if(this->completeset[i].set[j].sector[k].db==1){
                        cout<<"D ";
                    }
                    else{
                        cout<<"N ";
                    }
                }
            }
            cout<<"||"<<endl;
        }
    }

    void printcachestats(){
        cout<<"g. number of L2 reads:           "<<dec<<l2reads<<"\n";
        cout<<"h. number of L2 read misses:     "<<dec<<l2readsmiss<<"\n";
        cout<<"i. number of L2 writes:          "<<dec<<l2writes<<"\n";
        cout<<"j. number of L2 write misses:        "<<dec<<l2writemiss<<"\n";
        
        if(numberoftags==1 && blocksinasector==1){
            cout<<"k. L2 miss rate:          "<<fixed<<setprecision(4)<<float(float(l2readsmiss)/float(l2reads))<<"\n";   
            cout<<"l. number of writebacks from L2 memory:  "<<dec<<l2writeback<<"\n";
            cout<<"m. total memory traffic:      "<<l2readsmiss+l2writemiss+l2writeback;
        return;
        }
        cout<<"k. number of L2 sector misses:        "<<dec<<l2sectormisses<<"\n";
        cout<<"l. number of L2 cacheblock misses:        "<<dec<<cacheblockmiss<<"\n";
        cout<<"m. L2 miss rate:          "<<fixed<<setprecision(4)<<float(float(l2readsmiss)/float(l2reads))<<"\n";
        cout<<"n. number of writebacks from L2 memory:  "<<dec<<l2writeback<<"\n";
        cout<<"o. total memory traffic:      "<<l2readsmiss+l2writemiss+l2writeback<<"\n";
    }
    void printcachel1l2(){
            cout<<endl<<"===== L2 contents ====="<<endl;
            for(int i=0;i<numberofcompletesets;i++){
                cout<<"set  "<<i<<"   "<<": ";
                sort(&(this->completeset[i].set[0]), &(this->completeset[i].set[0+l2asso]), [](cachesetl2 const & a, cachesetl2 const & b) -> bool{ return a.counter < b.counter; } );
                for(int j=0;j<l2asso;j++){
                    cout<<hex<<stringbinaryindextointeger(this->completeset[i].set[j].tagstore[0]!=""?this->completeset[i].set[j].tagstore[0]:"0")<<" ";
                    if(this->completeset[i].set[j].sector[0].db==1){
                        cout<<'D'<<" || ";
                    }
                    else{
                        cout<<'N'<<" || ";
                    }
                }
                cout<<dec<<"\n";
            }
    }
    

};
// Cache 2 End
// Cache 1
    struct node{
        string tag;
        int ct;
        int db;
    };
    struct cacheset{
        node* set = NULL;
        public:
        cacheset() {
        }
        cacheset(int numberOfNodes) {
            set = new node[numberOfNodes];
            for(int j=0;j<numberOfNodes;j++){
                set[j].ct=j;
                set[j].db=0;
                set[j].tag="0";
            }

        }
    };
    class cache{
        int l1asso;
        int capacity;
        int blocksize;
        int l1reads=0;
        int l1readsmiss=0;
        int l1writes=0;
        int l1writesmiss=0;
        cacheset carray[10000];
        int numberofsets;
        cachel2 *l2 = NULL;
        public:
        cache(int blocksize,int capacity,int l1asso, int l2capacity, int p, int n, int l2asso){
            this->blocksize=blocksize;
            this->capacity=capacity;
            this->l1asso=l1asso;
            numberofsets=capacity/(blocksize*l1asso);
            for(int i=0;i<numberofsets;i++){
                carray[i] = cacheset(l1asso);
            }
            if(l2capacity !=0 && p != 0 && l2asso != 0 && n != 0 )
                l2 = new cachel2(l2capacity, p, n, blocksize, l2asso);
        }

        public:
        void printl1cacheset(int i) {
            cout<<"set  "<<dec<<i<<" :"<<" ";
                sort(&(this->carray[i].set[0]), &(this->carray[i].set[0+l1asso]), [](node const & a, node const & b) -> bool{ return a.ct < b.ct; } );
                for(int j=0;j<l1asso;j++){
                    cout<<hex<<stringbinaryindextointeger(this->carray[i].set[j].tag)<<" ";
                    if(this->carray[i].set[j].db==1){
                        cout<<'D'<<" || ";
                    }
                    else{
                    cout<<'N'<<" || ";
                    }
                }
                cout<<dec<<"\n";
        }
        void printcache(string filename){
            cout<<"===== Simulator configuration ====="<<endl;
            cout<<"BLOCKSIZE: "<<this->blocksize<<"\n";
            cout<<"L1_SIZE: "<<this->capacity<<"\n";
            cout<<"L1_ASSOC: "<<this->l1asso<<"\n";
            cout<<"L2_SIZE: "<<(l2?l2->l2capacity:0)<<"\n";
            cout<<"L2_ASSOC: "<<(l2?l2->l2asso:0)<<"\n";
            cout<<"L2_DATA_BLOCKS: "<<(l2?l2->blocksinasector:0)<<"\n";
            cout<<"L2_ADDRESS_TAGS: "<<(l2?l2->numberoftags:0)<<"\n";
            cout<<"trace_file: "<<filename<<"\n";
            cout<<endl<<"===== L1 contents ====="<<endl;
            for(int i=0;i<numberofsets;i++){
                cout<<"set  "<<i<<"   "<<": ";
                sort(&(this->carray[i].set[0]), &(this->carray[i].set[0+l1asso]), [](node const & a, node const & b) -> bool{ return a.ct < b.ct; } );
                for(int j=0;j<l1asso;j++){
                    cout<<hex<<stringbinaryindextointeger(this->carray[i].set[j].tag)<<" ";
                    if(this->carray[i].set[j].db==1){
                        cout<<'D'<<" || ";
                    }
                    else{
                    cout<<'N'<<" || ";
                    }
                }
                cout<<dec<<"\n";
            }
            if(l2) {
                if(l2->blocksinasector == 1 && l2->numberoftags == 1) {
                    l2->printcachel1l2();
                } else {
                    l2->printcache();
                }   
            }  
            cout<<endl;             
            cout<<"===== Simulation Results ====="<<"\n";
            cout<<"a. number of L1 reads:           "<<l1reads<<"\n";
            cout<<"b. number of L1 read misses:     "<<l1readsmiss<<"\n";
            cout<<"c. number of L1 writes:          "<<l1writes<<"\n";
            cout<<"d. number of L1 write misses:        "<<l1writesmiss<<"\n";
            cout<<"e. L1 miss rate:         "<<fixed<<setprecision(4)<<float(float(l1readsmiss+l1writesmiss)/float(l1reads+l1writes)) <<"\n";
            cout<<"f. number of writebacks from L1 memory:  "<<writebacks<<"\n";
            if(!l2)
            cout<<"g. total memory traffic:  "<<l1readsmiss+l1writesmiss+writebacks;
            if(l2)
                l2->printcachestats();
            cout<<endl;
        }
        int findleastused(int index){
            int leastusedindex=-1;
            int lrumin = -1;
            for(int i=0;i<l1asso;i++){
                if(this->carray[index].set[i].ct> lrumin){
                    leastusedindex=i;
                    lrumin = this->carray[index].set[i].ct;
                }
            }
            return leastusedindex;
        }
        bool checkifempty(int index){
            for(int i=0;i<l1asso;i++){
                if(this->carray[index].set[i].tag==""){
                    return true;
                }
            }
            return false;
        }

        void incrementlru(int index, int leastusedindex) {
            // cout<<"L1 update LRU\n";
            // for(int i=0;i<l1asso;i++)
            //     cout<<this->carray[index].set[i].ct<<" ";
            // cout<<endl;
            for(int i=0;i<l1asso;i++){
                if(this->carray[index].set[i].ct<(this->carray[index].set[leastusedindex].ct)){
                    this->carray[index].set[i].ct++;
                }
            }
            this->carray[index].set[leastusedindex].ct=0;
            // for(int i=0;i<l1asso;i++)
            //     cout<<this->carray[index].set[i].ct<<" ";
            // cout<<endl;
        }

        void readfromaddress(string binadd){
            l1reads+=1;
            int hitindex;
            int index;
            string indexstring;
            string tag = decodetag(blocksize, capacity, l1asso, binadd);
            if(numberofsets!=1){
            indexstring = decodeindex(blocksize, capacity, l1asso, binadd);}
            if(numberofsets!=1){
            index = stringbinaryindextointeger(indexstring);}
            else{ index=0;}
            // printl1cacheset(index);
            string blockstring = decodeblock(blocksize, capacity, l1asso, binadd);
            //  cout<<"L1 read: "<<hex<<stringbinaryindextointeger(binadd)<<" (tag "<<stringbinaryindextointeger(tag);
            //  cout<<dec<<", index "<<stringbinaryindextointeger(indexstring)<<")\n";
            
            int leastusedindex= findleastused(index);
            bool hitFlag = false;
            for(int i=0;i<l1asso;i++){
                if(this->carray[index].set[i].tag == tag){
                    hitFlag = true;
                    hitindex=i;
                }
            }
            if(!hitFlag){
                //  cout<<"L1 miss\n";
                // Miss
                l1readsmiss++;
                
                if(!checkifempty(index) && this->carray[index].set[leastusedindex].db==1) {
                    writebacks+=1;
                    string oldbinadd = this->carray[index].set[leastusedindex].tag + indexstring + blockstring;
                    if(l2)                  
                    l2->writetol2(oldbinadd);
                    this->carray[index].set[leastusedindex].db=0;
                }
                this->carray[index].set[leastusedindex].tag=tag;
                if(l2)
                l2->readfroml2(binadd);
                incrementlru(index, leastusedindex);
            }
            else{
                //  cout<<"L1 hit\n";
                incrementlru(index, hitindex);
            }
            // printl1cacheset(index);
        }

        void writetoaddress(string binadd){
            l1writes+=1;
            int hitindex;
            string indexstring;
            int index;
            string tag = decodetag(blocksize, capacity, l1asso, binadd);
            if(numberofsets!=1){
            indexstring = decodeindex(blocksize, capacity, l1asso, binadd);}
            if(numberofsets!=1){
            index = stringbinaryindextointeger(indexstring);}
            else{ index=0;}
            
            // printl1cacheset(index);
            string blockstring = decodeblock(blocksize, capacity, l1asso, binadd);
            //  cout<<"L1 write: "<<hex<<stringbinaryindextointeger(binadd)<<" (tag "<<stringbinaryindextointeger(tag);
            //  cout<<dec<<", index "<<stringbinaryindextointeger(indexstring)<<")\n";
            
            int leastusedindex=findleastused(index);
            bool hitFlag = false;
            for(int i=0;i<l1asso;i++){
                if(this->carray[index].set[i].tag == tag){
                    hitFlag = true;
                    hitindex=i;
                }
            }

            if(!hitFlag) {
                // cout<<"L1 miss\n";
                // Miss
                l1writesmiss++;
                if(!checkifempty(index)&& this->carray[index].set[leastusedindex].db==1) {
                    writebacks+=1;
                    string oldbinadd = this->carray[index].set[leastusedindex].tag + indexstring + blockstring;   
                    if(l2)               
                    l2->writetol2(oldbinadd);
                }
                this->carray[index].set[leastusedindex].tag=tag;
                this->carray[index].set[leastusedindex].db=1;
                if(l2)
                l2->readfroml2(binadd);
                incrementlru(index, leastusedindex);
            } else {
                // Hit
                //  cout<<"L1 hit\n";
                incrementlru(index, hitindex);
                this->carray[index].set[hitindex].tag=tag;
                this->carray[index].set[hitindex].db=1;
            }
            // printl1cacheset(index);

        }

        string decodetag(int blockSize,int l1s,int l1a,string binadd){
            int i=0;
            int bn=l1s/blockSize;
            int sn=bn/l1a;
            int ob=log2(blockSize);
            int ib=log2(l1s/(blockSize*l1a));
            int tagb=32-ob-ib;
            char tag[tagb];
            char index[ib];
            char offset[ob];
            for(i=0;i<tagb;i++){
                tag[i]=binadd[i];
            }
            tag[tagb]='\0';
            //cout<<tag<<endl;
            return tag;
        }
        string decodeindex(int blockSize,int l1s,int l1a,string binadd){
            int i=0;
            int bn=l1s/blockSize;
            int sn=bn/l1a;
            int ob=log2(blockSize);
            int ib=log2(l1s/(blockSize*l1a));
            int tagb=32-ob-ib;
            char tag[tagb];
            char index[ib];
            char offset[ob];
            for(i=0;i<tagb;i++){
                tag[i]=binadd[i];
            }
            tag[tagb]='\0';
            for(int j=0;j<ib;j++){
                index[j]=binadd[i];
                i++;
            }
            index[ib]='\0';
            for(int k=0;k<ob;k++){
                offset[k]=binadd[i];
                i++;
            }
            offset[ob]='\0';
            return index;
        }
        string decodeblock(int blockSize,int l1s,int l1a,string binadd){
            int i=0;
            int bn=l1s/blockSize;
            int sn=bn/l1a;
            int ob=log2(blockSize);
            int ib=log2(l1s/(blockSize*l1a));
            int tagb=32-ob-ib;
            char tag[tagb];
            char index[ib];
            char offset[ob];
            for(i=0;i<tagb;i++){
                tag[i]=binadd[i];
            }
            tag[tagb]='\0';
            for(int j=0;j<ib;j++){
                index[j]=binadd[i];
                i++;
            }
            index[ib]='\0';
            for(int k=0;k<ob;k++){
                offset[k]=binadd[i];
                i++;
            }
            offset[ob]='\0';
            return offset;
        }
    };
// Cache 1
string chartobin(char c){
    switch(c)
    {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'A': return "1010";
        case 'a': return "1010";
        case 'B': return "1011";
        case 'b': return "1011";
        case 'C': return "1100";
        case 'c': return "1100";
        case 'D': return "1101";
        case 'd': return "1101";
        case 'E': return "1110";
        case 'e': return "1110";
        case 'F': return "1111";
        case 'f': return "1111";
        default: return "2222";
    }

}
string hextobin(string s){
    //cout<<s<<"\n";
    string by="";
    int len=s.length();
    for(int i=0;i<len;i++){
        by=by+(chartobin(s[i]));
        }
        int l=by.length();
        if(l<32){
            for(int i=0;i<32-l;i++){
               by="0"+by;
               //cout<<by.length()<<endl;
            }
            //cout<<by<<endl;
        }
    return by;
}


int main(int argc,char **argv){

    int blockSize=stoi(argv[1]);
    int l1s=stoi(argv[2]);
    int l1asso=stoi(argv[3]);
    int l2capacity=stoi(argv[4]);
    int l2asso=stoi(argv[5]);
    int l2p=stoi(argv[6]);
    int l2n=stoi(argv[7]);
    ifstream cfile(argv[8]);
    // int blockSize=32;
    // int l1s=4096;
    // int l1asso=4;
    // int l2capacity=32768;
    // int l2asso=4;
    // int l2p=1;
    // int l2n=1;     
    // ifstream cfile("C:/Users/Daksh Kandpal/Desktop/cpp/perl_trace.txt");

    if(!cfile) {
         cout<<"File not found";
         return -1;
     }
     char type;
     string hadd;
    int count = 1;
     cache c = cache(blockSize, l1s, l1asso, l2capacity,l2p,l2n,l2asso);
     while(cfile>>type>>hadd){
         //cout<<hextobin(hadd)<<endl;
        //  cout<<"------------------------------------------------\n";
         if(type=='r'){
            // cout<<"# "<<count<<": read "<<hadd<<"\n";
             c.readfromaddress(hextobin(hadd));

         }
         else{
            // cout<<"# "<<count<<": write "<<hadd<<"\n";
             c.writetoaddress(hextobin(hadd));
         }
        count++;
     }

     c.printcache(argv[8]);
     return 0;
}