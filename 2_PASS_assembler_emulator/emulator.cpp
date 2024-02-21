#include<bits/stdc++.h>
#define ll long long int 
#define F first
#define S second
#define pb push_back
using namespace std;

map<int,string> mn={{0,"ldc"},{1,"adc"},{2,"ldl"},{3,"stl"},{4,"ldnl"},{5,"stnl"},
{6,"add"},{7,"sub"},{8,"shl"},{9,"shr"},{10,"adj"},{11,"a2sp"},{12,"sp2a"},{13,"call"},
{14,"return"},{15,"brz"},{16,"brlz"},{17,"br"},{18,"halt"},{1000,"data"},{100,"set"}};

set<int> t_num={6,7,8,9,11,12,14,18};
int s_p;
int A,B,SP,PC,count_progromcounter;
vector<int> instructions;
int temp=0;
int const N=1<<24;
int mem[N];

void ldc(int num){
    B=A;A=num;
}
void adc(int num){
    A=A+num;
}
void ldl(int num){
    B=A;
    if(SP+num>=N||SP+num<0){
        cout<<"Segmentation fault: we ran into out of range memory access Memory \n";
        exit(0);
    }
    
    A=mem[SP+num];
}
void stl(int num){
    if(SP+num>=N||SP+num<0){
       cout<<"Segmentation fault: we ran into Out of range memory access Memory \n";
        exit(0);
    }
    
    mem[SP+num]=A;
    A=B;
}
void ldnl(int num){
    if(A+num>=N||A+num<0){
     cout<<"Segmentation fault: we ran into Out of range memory access Memory \n";
        exit(0);
    }
    
    A=mem[A+num];
}
void stnl(int num){
    if(A+num>=N||A+num<0)
    {
       cout<<"Segmentation fault: we ran into Out of range memory access Memory \n";
         exit(0);
    }
    
    mem[A+num]=B;
}
void add(int num){
    A=B+A;
}
void sub(int num){
    A=B-A;
}
void shl(int num){
    A=B*2;
}
void shr(int num){
    A=B/2;
}
void adj(int num){
    SP=SP+num;
}
void a2sp(int num){
    SP=A;
    A=B;
}
void sp2a(int num){
    B=A;
    A=SP;
}
void call(int num){
    B=A;
    A=PC;
    PC=num;
}
void return1(int num){
    PC=A;
    A=B;
}
void brz(int num){
    if(A==0)PC=PC+num;
}
void brlz(int num){
    if(A<0)PC=PC+num;
}
void br(int num){
    PC=PC+num;
}

void ( *fun_arr[])(int)={ldc,adc,ldl,stl,ldnl,stnl,add,sub,shl,shr,adj,a2sp,sp2a,call,return1,brz,brlz,br};
//hex conversion
string hex_conversion(ll num){
   
    string ans ="00000000";
    int ind=7;
    while(num){
        int k=num%16;
        if(k<=9)ans[ind]='0'+k;
        else ans[ind]='a'+k-10;
        ind--;
        num/=16;
    }
    return ans;
}
//function of given instructions list
void function_printinst(unsigned int num){
    cout<<mn[num%256];
    unsigned int offset_value=num/256;
    if(((offset_value>>23)&1)){
        offset_value|=(255<<24);
    }
    if(t_num.find(num%256)==t_num.end()){
            printf(" %08x",offset_value);
    }
        printf("\n");

}
//printing address and the pstackponter and pc
void print_cs(){
    printf("A=%08X \t B=%08X \t SP=%08X \t PC=%08X\n",A,B,SP,PC);
}
//instruction detection and performing 
int inst_performance(){
    temp++;
    unsigned int num=instructions[PC];
    
    unsigned int opcode=num%256;
    unsigned int offset_value=num/256;
    if(mn.find(opcode)==mn.end()){
        cout<<"Invalid instruction:: it is not a vaild mnemonic found\n";
        exit(0);
    }
    if(!s_p){
        printf("PC=%08X    %08X\t",(unsigned int)PC,(unsigned int)instructions[PC]);
        function_printinst(instructions[PC]);
    }

    PC++;

    if(((offset_value>>23)&1)){
        offset_value|=(255<<24);
    }
    
    (*fun_arr[opcode])(offset_value);
    if(opcode==18)PC=count_progromcounter;
    return 0;

    
}
int main(int argc,char* argv[])
{   
    // equating all to 0
    A=B=PC=SP=0;
    int read;
    ifstream infile;
    infile.open (argv[1], ios::in | ios::binary);
    string input;
    string word;
    int num=0,flag=0;
    cout<<"Instruction given below are available in emulator:\n";
    cout<<"Trace:For trace write   -t command \n";
    cout<<"Trace number: For trace number write -t num(a number) in emulator\n";
    cout<<"Run at once:For all commands to run at once have to write -f in terminal  \n";
    cout<<"Future instructions: For all the upcoming instruction write -u \n";
    cout<<"Show memory dump before execution: For current memory have to write -bd\n";
    cout<<"Show a memory location: For looking to memory location -data  \n";
    cout<<"Show memory dump after execution: For the memory after the execution write -ad\n";
    count_progromcounter=0;
    while(!infile.eof()){
        infile.read((char*)&read,sizeof(int));
        mem[count_progromcounter]=read;
        instructions.pb(read);
        count_progromcounter++;
    }
    
    while(PC<count_progromcounter){
        getline(cin,input);
        stringstream input_stream(input);
        input_stream>>word;
        if(word=="-t"){

            if(input_stream.eof()){
                inst_performance();
                print_cs();
            }else{
                input_stream>>num;

                for(int i=0;i<num;i++)inst_performance(),print_cs();
            }
        }else if(word=="-f"){

            while(count_progromcounter!=PC)inst_performance(),print_cs();

        }else if(word=="-u"){
            int PC_copy=PC;
            for(int i=0;i<10&&PC<count_progromcounter;i++){
                printf("PC=%08X  %08X ",PC,instructions[PC]);
                function_printinst(instructions[PC]);
                PC++;
            }
            PC=PC_copy;
        }else if(word=="-bd"){
            for(int i=0;i<count_progromcounter;i++){
                printf("%08x\t",i);
                for(int j=0;j<4;j++){
                    printf("%08X ",mem[i]);
                    i++;
                    if(i>=count_progromcounter)break;
                }
                printf("\n");
                i--;
            }

        }else if(word=="-data"){
            input_stream>>hex>>num;
            if(num<0||num>=N){
                cout<<"Warning: Out of bound memory fetch\n";
            }else{
                printf("[%08X] = %08X\n",num,mem[num]);  
            }
        }else if(word=="-ad"){
            s_p=1;
            cout<<"DATA SEGMENT:\n";
            while(count_progromcounter!=PC)inst_performance();
            for(int i=0;i<count_progromcounter;i++){
                printf("%08x\t",i);
                for(int j=0;j<4;j++){
                    printf("%08X ",mem[i]);
                    i++;
                    if(i>=count_progromcounter)break;
                }
                printf("\n");
                i--;
            }

        }

    }
    cout<<"We have reached the end of file \n";
    
}