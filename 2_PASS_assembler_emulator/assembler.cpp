#include <bits/stdc++.h>
using namespace std;

// it is for storing symbol and label after the pass1
struct i_s {
	int address ; 		
	int type ;			// when type = 1 indicates  Label name or block name
};

struct i_m {
	string opcode ;	// when type = 0 it indicates  no operand
	int type ;			//type = 1 it indicates argument is given
							//type = 2 it indicates offset to be given
};

struct func_c {
	string l_n ;
	string instruction ;
	string num_opcode ;
	func_c(string &a , string &b , string &c){
		l_n = a ;
		instruction = b ;
		num_opcode = c ;
	}
	friend ostream& operator <<(ostream& os , const func_c & H){
		os << H.l_n <<" "<<H.instruction <<" "<< H.num_opcode <<" ";
		return os ;
	}
};

vector<string> mc; 					 // machine code is stored ;
vector<string> lc ; 					 // listing is stored in it ;
vector < pair < int , func_c > > clean_pass1 ; // after the clean instruction for pass1
vector < pair < int , func_c >  > i_pass2 ;  //instruction for pass2
map < string , i_m > i_opcodetable ; 			 // it is instruction Opcode Table 
map < string , i_s > l_declare ; 			 // Declared Labels 
map < string , i_s > l_used ; 					 // Used Labels 
vector<pair < int , string > > alert ;   //storing all the warning in the file  
vector < pair < int , string >  > mistake ; // storing all the errors in the file ;
int program_count = 0 ;										 
set < string > label_datadeclare ;				    //labels dused in data
set < string > lable_setdeclare ;					 //labels in set instruction 
vector < pair < int , func_c > > data_segment ;
bool check_Halt_Present = false ;	

				
//clearing the commments in the code and also the spaces
string remove_com_sp(string str){
	string str_r ; 
	int k = 0 ;
	while(k < str.size() && str[k] == ' ')k++ ;
	for(; k < str.size() ; k++){
		char a = str[k] ;
		if(a == ';')break ;
		if(a >= 'A' && a <= 'Z')a = tolower(a) ; 
		str_r.push_back(a) ;
	}
	return str_r ;
}
bool verify_c(char z){
	return ((z >= 'A' && z <= 'Z' ) || (z >= 'a' && z <= 'z')) ;
}
bool verify_num(char z){
	return (z >= '0' && z <= '9') ;
}
bool check_octal(string str){
	if(str.size() < 2)return false ;
	bool temp = true ;
	temp &= (str[0] == '0') ;
	for(int k = 1 ; k < str.size() ; k++){
		temp &= (str[k] >= '0' && str[k] <= '7') ;
	}
	return temp ;
}
bool isHexadecimal(string str){
	bool temp = true ;
	if(str.size() < 3)return false ;
	temp &= (str[0] == '0') ;
	temp &= (str[1] == 'x') ;
	for(int k = 2 ; k < str.size() ; k++){
		bool t = (str[k] >= '0' && str[k] <= '9') ;
		t |= (str[k] >= 'a' && str[k] <= 'f') ;
		temp &= t ;
	}
	return temp ;
}
//oct to decimal conversion
int con_oct_to_dec(string str){
	int ptr = 0 , z = 1;
	for(int k = str.size() - 1 ; k > 0 ; k--){
		ptr += (str[k] - '0') * z ;
		z *= 8 ; 
	}
	return ptr ;
}
//hex to decimal conversion
int con_hex_to_dec(string str){
	int ptr = 0 , z = 1 ;
	for(int k = str.size() - 1; k > 1 ; k--){
		if(verify_num(str[k])){
			ptr += (str[k] - '0') * z ;
		}else {
			ptr += ((str[k] - 'a') + 10) * z ;
		}
		z *= 16 ;
	}
	return ptr ;
}
//checking name is valid or not 
bool check_validname(string i_name){
	if(i_name.empty())return false ;
	bool ptr = true ;
	ptr &= verify_c(i_name[0]) ;
	return ptr ;
	for(int k = 0 ; k < i_name.size() ; k++){
		ptr &= (verify_c(i_name[k]) || verify_num(i_name[k]) || i_name[k] == '_') ;
	}
	return ptr ;
}


//dividing it is for dividing the every line into the opcode,ins
void dividing(string &l_n , string &instruction , string &num_opcode , string &i_1_l , int num_l){
	int k = 0 ;
	bool l = false ;
	for(; k < i_1_l.size() ; k++){
		if(i_1_l[k] == ':'){
			l = true ;
			for(int p = 0 ; p < k ; p++){
				l_n.push_back(i_1_l[p]) ;
			}
			k++;
			break ;
		}
	}
	if(!l)k = 0 ;
	while(k < i_1_l.size() && (i_1_l[k] == ' ' || i_1_l[k] == '\t'))k++ ;
	while(k < i_1_l.size() && !(i_1_l[k] == ' ' || i_1_l[k] == '\t')){
		instruction.push_back(i_1_l[k]);
		k++;
	}
	while(k < i_1_l.size() && (i_1_l[k] == ' ' || i_1_l[k] == '\t'))k++ ;
	while(k < i_1_l.size() && !(i_1_l[k] == ' ' || i_1_l[k] == '\t')){
		num_opcode.push_back(i_1_l[k]);
		k++ ;
	}
	while(k < i_1_l.size() && (i_1_l[k] == ' ' || i_1_l[k] == '\t'))k++ ;
	if(k < i_1_l.size() && !(i_1_l[k] == ' ' || i_1_l[k] == '\t')){
		string t = "Extra operand on end of line " ;
		mistake.push_back({num_l , t}) ;
	}
}
//opcode 
string op_function(string str , int * ptr1 , int num_l){
	if(check_validname(str)){
		l_used[str] = {num_l , 5} ;
		return str ;
	}
	int num_s = 1 ;
	if(str[0] == '-')num_s = -1 ;
	if(str[0] == '+' || str[0] =='-')str = str.substr(1) ;
	if(check_octal(str))return to_string(num_s *con_oct_to_dec(str)) ;
	if(isHexadecimal(str))return to_string(num_s*con_hex_to_dec(str)) ;
	bool temp = true ;
	for(int k = 0; k < str.size() ; k++){
		temp &= verify_num(str[k]) ;
	}
	if(temp)return to_string(num_s * stoi(str)) ;
	*ptr1 = 1 ;
	return str ;
}



void data_found(){
	for(auto& i_1_l : clean_pass1){
		if(i_1_l.second.instruction == "set"){
			lable_setdeclare.insert(i_1_l.second.l_n) ;
		}
	}
	for(auto& i_1_l : data_segment){
		label_datadeclare.insert(i_1_l.second.l_n) ;
	}
	vector < pair < int , func_c > > temp5 ;
	for(auto i_1_l : clean_pass1){
		bool temp3 = false ;
		if(i_1_l.second.instruction == "set"){
			if(label_datadeclare.count(i_1_l.second.l_n) == 0){
				label_datadeclare.insert(i_1_l.second.l_n) ;
				i_1_l.second.instruction = "data" ;
				data_segment.push_back(i_1_l) ;
				temp3 = true ; 
			}
		}
		if(!temp3){
			temp5.push_back(i_1_l) ;
		}
	}
	clean_pass1.clear() ;
	for(auto i_1_l : temp5){
		clean_pass1.push_back(i_1_l) ; 
	}
	for(auto i_1_l : data_segment){
		clean_pass1.push_back(i_1_l) ;
	}
	temp5.clear() ;
	data_segment.clear() ;
	
}



bool instr_set(string &l_n , string &str5 , int num_l , int toadd_Num){
	int m = 0 ;
	if(l_n.empty()){
		string str6 = "Missing Label op_name for set Instruction" ;
		mistake.push_back({num_l , str6}) ;
		return false;
	}
	if(str5.empty()){
		string str6 = "Missing operand" ;
		mistake.push_back({num_l , str6}) ;
		return false;
	}
	string str7 = op_function(str5 , &m, num_l) ;
	if(verify_c(str5[0]) || m == 1){
		string str6 = "Not a Number for SET Instruction" ;
		mistake.push_back({num_l , str6}) ;
		return false ;
	}
	string instruction ;
	if(label_datadeclare.count(l_n) == 0 && l_declare.count(l_n) == 0 ){
		return true ;

	}else {
		string num_opcode ;
		string label ;
		instruction = "adj" ; num_opcode = "2" ;										
		str7 = op_function(num_opcode , &m , num_l) ;
		i_pass2.push_back({program_count , func_c(label , instruction , str7)}) ; program_count++ ;

		instruction = "stl" ; num_opcode = "-1" ;									
		str7 = op_function(num_opcode , &m , num_l) ;
		i_pass2.push_back({program_count , func_c(label , instruction , str7)}) ; program_count++ ;

		instruction = "stl" ; num_opcode = "0" ;										
		str7 = op_function(num_opcode , &m , num_l) ;
		i_pass2.push_back({program_count , func_c(label , instruction , str7)}) ; program_count++ ;

		instruction = "ldc" ; num_opcode = str5 ;										
		str7 = op_function(num_opcode , &m , num_l) ;
		i_pass2.push_back({program_count , func_c(label , instruction , str7)}) ; program_count++ ;

		instruction = "ldc" ; num_opcode = l_n ;									
		str7 = op_function(num_opcode , &m , num_l) ;
		i_pass2.push_back({program_count , func_c(label , instruction , str7)}) ; program_count++ ;

		instruction = "adc" ; num_opcode = to_string(toadd_Num) ;
		str7 = op_function(num_opcode , &m , num_l) ;
		i_pass2.push_back({program_count , func_c(label , instruction , str7)}) ; program_count++ ; 		

		instruction = "stnl" ; num_opcode = "0" ;										
		str7 = op_function(num_opcode , &m , num_l) ;
		i_pass2.push_back({program_count , func_c(label , instruction , str7)}) ; program_count++ ;

		instruction = "ldl" ; num_opcode = "0" ;										
		str7 = op_function(num_opcode , &m , num_l) ;
		i_pass2.push_back({program_count , func_c(label , instruction , str7)}) ; program_count++ ;

		instruction = "ldl" ; num_opcode = "-1" ;										
		str7 = op_function(num_opcode , &m , num_l) ;
		i_pass2.push_back({program_count , func_c(label , instruction , str7)}) ; program_count++ ;

		instruction = "adj" ; num_opcode = "-2" ;										
		str7 = op_function(num_opcode , &m , num_l) ;
		i_pass2.push_back({program_count , func_c(label , instruction , str7)}) ; program_count++ ;
	}
	return false ;
}



string input_file = "input.asm";
void clearing_unwanted(){
	ifstream inputed_file ;
	inputed_file.open(input_file) ;
	if(inputed_file.fail()){
		cout << "Error! finding problem  to Open the  Input file" ;
		exit(0);
	}
	string str ;
	int num_l = 0;
	while(getline(inputed_file , str)){
		string i_1_l = remove_com_sp(str) ;
		num_l++ ;
		if(i_1_l.empty())continue ;
		string instruction , num_opcode , l_n ;
		dividing(l_n , instruction , num_opcode , i_1_l , num_l) ;
		if(!instruction.empty() && instruction == "data"){
			if(clean_pass1.size() > 0){
				auto temp = clean_pass1.back() ;
				if(l_n.empty() && temp.second.instruction.empty()){
					data_segment.push_back(temp) ;
					clean_pass1.pop_back() ;
				}
			}
			data_segment.push_back({num_l , func_c(l_n , instruction , num_opcode)}) ;
		}else {
			clean_pass1.push_back({num_l , func_c(l_n , instruction , num_opcode)}) ;
		}
	}
}
//PASS 1 
void pass1(){
	clearing_unwanted() ;
	data_found() ;
	int count = 0 ; 
	string before_str ; 
	for(auto& i_1_l : clean_pass1){
		int num_l ;
		string instruction , num_opcode , l_n ;
		num_l = i_1_l.first ;
		l_n = i_1_l.second.l_n ;
		instruction = i_1_l.second.instruction ;
		num_opcode = i_1_l.second.num_opcode ;

		bool declarevalid_label = false ;
		if(!l_n.empty()){
			bool checker = check_validname(l_n) ;
			if(!checker){
				string t = "Bogus Label Name " ;
				mistake.push_back({num_l , t}) ;
			}else {
				//declare the valid Label .
				declarevalid_label = true ;
				before_str = l_n ;
				count = 0 ; 
			}
		}else count++ ;
		int n = 0 ;
		int minus = 1 , toincrease = 0  ;
		bool require_operand = false ;
		bool temp3 = false ;
		if(!instruction.empty()){
			bool checker = (i_opcodetable.count(instruction) > 0 ) ;
			if(!checker){
				string t = "Bogus Mnemonic" ;
				mistake.push_back({num_l , t}) ;
			}else {
				require_operand |= (i_opcodetable[instruction].type > 0) ;
				if(instruction == "set"){
					temp3 = instr_set(before_str , num_opcode , num_l , count) ;
				}
				if(temp3)instruction = "data" ;
				toincrease++ ;
				if(declarevalid_label && instruction == "data")n = 1 ;
				/*special care for data  and set instructions */
			}
		}
		if(!temp3 && (instruction == "set")){
			continue ;
		}
		if(declarevalid_label){
			if(l_declare.count(l_n) > 0){
				string t = "Duplicate label definition of \""+l_n+"\"";
				mistake.push_back({num_l , t}) ;
			}else {
				l_declare[l_n] = {program_count , n} ;
			}
		}
		string str7 ;
		if(!num_opcode.empty()){
			if(!require_operand){
				string t = "Unexpected Operand" ;
				mistake.push_back({num_l , t}) ;
			}else {
				//check if it is valid label or valid number 
				int m = 0 ;
				str7 = op_function(num_opcode , &m , num_l) ;
				if(m == 1){
					string t = "Not a Valid label or number" ;
					mistake.push_back({num_l , t}) ;
				}
			}
		}else {
			if(require_operand){
				string t = "Missing Operand" ;
				mistake.push_back({num_l , t}) ;
			}
		}
		i_pass2.push_back({program_count , func_c(l_n , instruction , str7)}) ;
		program_count += toincrease ;
	}
	map < string , i_s >::iterator Ptr ;
	for(Ptr = l_used.begin() ; Ptr != l_used.end() ; Ptr++){
		string op_name = Ptr->first ;
		i_s temp = Ptr->second ;
		int add = temp.address ;
		if(l_declare.count(op_name) == 0){
			string t = "this \"" +op_name+ "\"label is not been declared" ;
			mistake.push_back({add , t}) ;
		}
	}
	for(Ptr = l_declare.begin() ; Ptr != l_declare.end() ; Ptr++){
		string op_name = Ptr->first ;
		if(l_used.count(op_name) == 0){
			string t = "Label having this  \""+ op_name + "\" is not used but declared" ;
			alert.push_back({Ptr->second.address , t}) ;
		}
	}
}
string hexa_string(int num , bool check_24){
	string result = "00000000" ;
	int index = 7 ;
	if(num < 0){
		unsigned int z = num ;
		result = "" ;
		while( z != 0){
			int temp = z % 16 ;
			if(temp < 10){
				result = (char)(temp + 48) + result ;
			}else {
				result = (char)(temp + 55) + result ;
			}
			z /= 16 ;
			index-- ;
		}
		string res = "00000000" ;
		int p = 7 ;
		for(int k = result.size() - 1 ; k >= 0 && p >= 0 ; k-- , p--){
			res[p] = result[k] ;
		}
		result = res ;
	}else {
		while( num != 0){
			int temp = num % 16 ;
			if(temp < 10){
				result[index] = (char)(temp + 48) ;
			}else {
				result[index] = (char)(temp + 55) ;
			}
			num /= 16 ;
			index-- ;
		}
	}
	if(check_24)return result.substr(2) ;
	return result ;

}
//PASS 2 
void pass2() {
	for(auto &i_1_l :  i_pass2){
		
		int loc = i_1_l.first ;
		string l_n = i_1_l.second.l_n ;
		string instruction = i_1_l.second.instruction ;
		string num_opcode = i_1_l.second.num_opcode ;
		int operand_val ;
		string ins_val ;
		bool needsoffset = false ;
		bool isthisdataval = false ;
		if(!instruction.empty()){
			ins_val = i_opcodetable[instruction].opcode ;
			if(i_opcodetable[instruction].type == 2)needsoffset = true ;
		
			if(instruction == "data")isthisdataval = true ;
			if(instruction == "halt")check_Halt_Present = true ;
		}
		string final ;
		if(!num_opcode.empty()){
			if(l_declare.count(num_opcode) == 0)operand_val = stoi(num_opcode) ;
			else operand_val = l_declare[num_opcode].address ;
			
			if(needsoffset && l_declare.count(num_opcode) != 0){
				operand_val -= loc + 1 ;
				
			}
			
			if(isthisdataval){
				
				final = hexa_string(operand_val , false) ;
			}else {
				
				final = hexa_string(operand_val , true) + ins_val ;
			}
		}else final = "000000" + ins_val ;
		string program_counter = hexa_string(loc , false) ;
		string mcode , lcode;
		lcode = program_counter ;
		if(!instruction.empty()){
			mcode = final ;
			lcode += " " + final ;
		}else {
			lcode += "         ";
		}
		if(!l_n.empty()){
			lcode += " " + l_n + ":" ;
 		}
 		if(!instruction.empty()){
 			lcode += " " + instruction ;
 		}
 		if(!num_opcode.empty()){
 			lcode += " " + num_opcode ;
 		}
 		if(!mcode.empty())mc.push_back(mcode) ;
		if(!lcode.empty())lc.push_back(lcode) ;
	}
	if(!check_Halt_Present){
		string t = "HALT instruction is not found ." ;
		alert.push_back({12 , t}) ;
	}


}

void opcodetable_initial_instr(){

					
	i_opcodetable["add"] = {"06" , 0} ;	i_opcodetable["sub"] = {"07" , 0} ;	i_opcodetable["shl"] = {"08" , 0} ;
	i_opcodetable["shr"] = {"09" , 0} ;	i_opcodetable["a2sp"] = {"0B" , 0} ;	i_opcodetable["sp2a"] = {"0C" , 0} ;
	i_opcodetable["return"] = {"0E" , 0} ; i_opcodetable["halt"] = {"12" , 0} ;


	i_opcodetable["data"] = {"" , 1} ;	i_opcodetable["ldc"] = {"00" , 1} ;	i_opcodetable["adc"] = {"01" , 1} ;
	i_opcodetable["set"] = {"" , 1} ; i_opcodetable["adj"] = {"0A" , 1} ;


	i_opcodetable["ldl"] = {"02" , 2} ;	i_opcodetable["stl"] = {"03" , 2} ;	i_opcodetable["ldnl"] = {"04" , 2} ;
	i_opcodetable["stnl"] = {"05" , 2} ; i_opcodetable["call"] = {"0D" , 2} ;	i_opcodetable["brz"] = {"0F" , 2} ;
	i_opcodetable["brlz"] = {"10" , 2} ;	i_opcodetable["br"] = {"11" , 2} ;
	
}

void Errors_in_file(const string &logfile)
{	
	sort(mistake.begin() , mistake.end()) ;
	 cerr << "Failed to Assemble!!" << endl;
	  ofstream coutLog(logfile);
        coutLog <<"Failed to assemble!" << endl;
        coutLog <<"Errors :-" << endl;
         for(auto z : mistake){
            cerr <<"Found Error at Line " <<  z.first << " : " << z.second << endl ;
            coutLog <<"Found Error at Line " <<  z.first << " : " << z.second << endl ;
         }

     coutLog.close();        
     exit(0);      
}
void warnings_in_file(const string &logfile)
{	
    sort(alert.begin() , alert.end()) ;
     ofstream coutLog(logfile);
      coutLog << "Warning :-" << endl;
	    for(auto z : alert){
	         cerr << "Has Warning at Line " << z.first << " : " <<z.second << endl ;
	         coutLog << "Has Warning at Line " << z.first << " : " <<z.second << endl ;
	    }
     coutLog.close();
}

void listing_file(const string &listingfile){
	ofstream outfile(listingfile) ;
	for(auto & z : lc){
		outfile << z << endl ;
	}
	outfile.close() ;
}

void write_object_code_file(const string &machinecodefile){
	ofstream noutfile ;
	noutfile.open(machinecodefile , ios::binary | ios::out);
	for(auto & z : mc){
		unsigned int y ;
		std::stringstream ss ;
		ss << std::hex << z ;
		ss >> y ;
		static_cast<int>(y);
		noutfile.write((const char*)&y , sizeof(unsigned )) ;
	}
	noutfile.close() ;
}

int main(int argc , char* argv[])
{	
	

	if(argc != 2){
		cerr<<"Error! have to passs only .asm file";
		return 0;
    }
    int fname_len = strlen(argv[1]);
    if(fname_len<=3 or strcmp("asm",argv[1]+fname_len-3)){
		cerr<<"Error! Input file is not a  .asm file " << endl;
		cerr<<"Enter a .asm file" << endl;
		return 0;
    }

    string filename(argv[1]);
    string namewithoutasm = "";
    int k = 0;
    while(argv[1][k] != '.'){ namewithoutasm += argv[1][k];k++;}

	input_file = namewithoutasm + ".asm" ;
	string logfile , listingfile , machinecodefile ;
	logfile = namewithoutasm + "log.txt" ;
	listingfile = namewithoutasm + ".l" ;
	machinecodefile = namewithoutasm + ".o" ;

	opcodetable_initial_instr() ;

	
	pass1() ;

	if(mistake.size() > 0) 
		Errors_in_file(logfile);
	
	pass2() ;

	warnings_in_file(logfile);

	

	listing_file(listingfile);
	write_object_code_file(machinecodefile);
	cerr <<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
    cerr << "Code compiled with no errors!!!" << endl;
    cerr << "Logs of the code is generated in: " + namewithoutasm + "log.txt" << endl;
    cerr << "Machine code of the code  generated in: " + namewithoutasm + ".o" << endl;
    cerr << "Listing of the code  generated in: " + namewithoutasm + ".l" << endl;
	cerr <<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
    

   

	return 0;
}