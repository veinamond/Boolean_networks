#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <Windows.h>
using namespace std;
enum class BN_operation{Plus, Minus, Times, Divide, Degree, Log, Neg};
enum class BN_operation_arity {Single, Double};
enum class BN_variable_type {Discr_var, Constant};
enum class BN_variable_iotype {Input, Output, Intermediate};
enum class LS_operation{NOT, AND, OR, EQUIV, ATOM, UNDEF};
enum class LE_equation_type{Assign, Assert};
string inttostr(int number)
{
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}
int strtoint(string s){
	int x = atoi(s.c_str());
	return x;
}
int Arity_LS(LS_operation a){
	int result;
	switch (a){
	case LS_operation::AND:		
	case LS_operation::OR:
		result=-1;
		break;
	case LS_operation::EQUIV:
		result=2;
		break;
	case LS_operation::NOT:
	case LS_operation::ATOM:
		result=1;
		break;
	default:
		result=0;	
	}
	return result;
}
int Arity_BN(BN_operation a){
	int result;
	switch (a){
		case BN_operation::Plus:
		case BN_operation::Minus:
		case BN_operation::Times:
		case BN_operation::Divide:
		case BN_operation::Degree:
		result=2;
		break;
	
		case BN_operation::Neg:
		case BN_operation::Log:
		result=1;
		break;
	default:
		result=0;	
	}
	return result;
}
string op_txt(BN_operation a){
	string result;
	switch (a){
		case BN_operation::Plus:
			result='+';
			break;
		case BN_operation::Minus:
			result='-';
			break;
		case BN_operation::Times:
			result='*';
			break;
		case BN_operation::Divide:
			result='/';
			break;
		case BN_operation::Degree:
			result='^';
			break;
		case BN_operation::Neg:
			result='-';
			break;
		case BN_operation::Log:
			result='log';
			break;		
	default:
		result="";	
	}
	return result;

}
class Logical_statement{
private:
	LS_operation st_op;
	vector<Logical_statement> arguments;
	int encoding_var;
public:
	Logical_statement::Logical_statement();
	Logical_statement(int arg);
	bool Create_LS(LS_operation op,vector<int> args);
	bool Create_LS(LS_operation op,vector<Logical_statement> args);
	string Print();
	
};
Logical_statement::Logical_statement(){
	st_op=LS_operation::UNDEF;
	encoding_var=0;
}
Logical_statement::Logical_statement(int arg){
	st_op=LS_operation::ATOM;
	encoding_var=arg;
}
bool Logical_statement::Create_LS(LS_operation op, vector<int>args){
	bool res;
	int ar=Arity_LS(op);
	if ((ar==0)|| ((ar!=-1)&&(args.size()!=ar))) {res=false;}
	else{
		st_op=op;
		for (int i=0;i<args.size();i++){
			Logical_statement t(args[i]);
			arguments.push_back(t);
		}
		res=true;
	}
	return res;
}
bool Logical_statement::Create_LS(LS_operation op, vector<Logical_statement>args){
	bool res;
	int ar=Arity_LS(op);
	if ((ar==0)|| ((ar!=-1)&&(args.size()!=ar))) {res=false;}
	else{
		st_op=op;
		for (int i=0;i<args.size();i++){
			Logical_statement t=args[i];
			arguments.push_back(t);
		}
		res=true;
	}
	return res;
}
string Logical_statement::Print(){
	string s;
	switch(st_op){
	case LS_operation::ATOM:
		s="x_"+inttostr(encoding_var);
		break;
	case LS_operation::NOT:
		s='!'+arguments[0].Print();
		break;
	case LS_operation::EQUIV:
		s=arguments[0].Print()+"=="+arguments[1].Print();
		break;
	case LS_operation::AND:
		for (int i=0;i<arguments.size()-1;i++){
			s+=arguments[i].Print()+ " & ";
		}
		s+=arguments[arguments.size()-1].Print();
		break;
	case LS_operation::OR:
		for (int i=0;i<arguments.size()-1;i++){
			s+=arguments[i].Print()+ " | ";
		}
		s+=arguments[arguments.size()-1].Print();
		break;
	default:
		s="";		
	}
	if (arguments.size()>1) s="("+s+")";
	return s;
}
class Logical_equation{
private:
	LE_equation_type le_et;
	int leftpart;
	Logical_statement rightpart;
public:
	Logical_equation(int var_num, LE_equation_type et, Logical_statement rp);
	string Print();
};
Logical_equation::Logical_equation(int var_num, LE_equation_type et, Logical_statement rp){
	le_et=et;
	leftpart=var_num;
	rightpart=rp;
}
string Logical_equation::Print(){
	string s;
	switch (le_et){
	case LE_equation_type::Assert:
		s=rightpart.Print() + " = " + inttostr(leftpart);
		break;
	case LE_equation_type::Assign:
		s="x_"+inttostr(leftpart)+" = "+rightpart.Print();
		break;
	default:
		s="";
	}
	return s;
}
struct coords{
	int i;
	int j;
};
void quicksort(vector<double> &vals, int first, int last){
	int i=first;
	int j=last;
	int x=vals[(first+last)/2];

	do {
		while (vals[i]<x) i++;
		while (vals[j]>x) j--;

		if (i<=j){
			if (i<j) {
				double tmp=vals[j];
				vals[j]=vals[i];
				vals[i]=tmp;				
			}
			i++;
			j--;
		}
	} while (i<=j);

	if (i<last) {quicksort (vals,i,last);}
	if (first<j){quicksort (vals,first,j);}
}
class BN_range{
private:
	double leftborder;
	double rightborder;
	double mean;
	vector<double> approximated_values;
	double deviation;
public:
	double dev(){return deviation;}
	BN_range(double lb, double rb, vector<double> values);
	BN_range(double lb, double rb, double singlevalue);
	void addtorange(double val);
	void removefromrange(double val);
	double mostright(){return approximated_values[0];}
	double mostleft(){return approximated_values[approximated_values.size()-1];}
	double compute_deviation();
	double compute_deviation(bool droplb, bool droprb, bool usenewval, double newval);
};
BN_range::BN_range(double lb, double rb, double singlevalue){
	leftborder=lb;
	rightborder=rb;
	approximated_values.push_back(singlevalue);
	if ((singlevalue<lb)||(singlevalue>=rb)){
			cout<<endl<<"Incorrect range";
	}
	mean=singlevalue;
	deviation=0;
}
BN_range::BN_range(double lb, double rb, vector<double> values){
	leftborder=lb;
	rightborder=rb;
	approximated_values=values;
	quicksort(approximated_values,0,approximated_values.size()-1);
	for (int i=0;i<approximated_values.size();i++){
		if ((approximated_values[i]<lb)||(approximated_values[i]>=rb)){
			cout<<endl<<"Incorrect range";
		}
	}
	mean=(lb+rb)/2;
	compute_deviation();
}
double BN_range::compute_deviation(){
	double r=0;
	for (int i=0;i<approximated_values.size();i++){
		r+=(approximated_values[i]-mean)*(approximated_values[i]-mean);
	}
	r=r/approximated_values.size();
	deviation=sqrt(r);
	return deviation;
}
double BN_range::compute_deviation(bool droplb, bool droprb, bool usenewval, double newval){
	if ((droplb==false)&&(droprb==false)){ return deviation;}
	else{
		if ((usenewval==false)&&(approximated_values.size()==1))return deviation;
		
		double r=0;		
		int i1= droplb ? 1 : 0;
		int il= droprb ? approximated_values.size()-2 : approximated_values.size()-1;		
		int sz=approximated_values.size();
		double nlb=approximated_values[i1];
		double nrb=approximated_values[il];
		if (usenewval==true){
			nlb=(approximated_values[i1]<newval)?approximated_values[il]:newval;
			nrb=(approximated_values[il]>newval)?approximated_values[il]:newval;			
		}
		double mn=(nrb-nlb)/2;
		if (droplb==true) {sz--;}
		if (droprb==true) {sz--;}
		if (usenewval==true) {
			sz++;	
			r=(newval-mn)*(newval-mn);
		}
		for (int i=1;i<il;i++){
			r+=(approximated_values[i]-mn)*(approximated_values[i]-mn);
		}
		r=r/sz;
		r=sqrt(r);
		return r;
	}
}
void BN_range::addtorange(double val){
	approximated_values.push_back(val);
	//very uneffective but will work - todo later
	quicksort(approximated_values,0,approximated_values.size()-1);
	leftborder=approximated_values[0];
	rightborder=approximated_values[approximated_values.size()-1];
	mean=(leftborder+rightborder)/2;
	compute_deviation();
}
void BN_range::removefromrange(double val){
	if (val==leftborder){
		approximated_values.erase(approximated_values.begin());
	}
	if (val==rightborder){
		approximated_values.erase(approximated_values.end());
	}
	if ((val<leftborder)&&(val>rightborder)) cout<<endl<<"error while removing from range";
		
	//very uneffective but will work - todo later
	quicksort(approximated_values,0,approximated_values.size()-1);
	leftborder=approximated_values[0];
	rightborder=approximated_values[approximated_values.size()-1];
	mean=(leftborder+rightborder)/2;
	compute_deviation();
}
class BN_Variable{
private:
	string caption;
	vector<Logical_equation> Equations;
	vector<int> encoding_vars;	
	vector<double> vars_values;

	vector<BN_range> ranges;

	double leftborder;
	double rightborder;
	double lattice_step;
	BN_Variable * leftparent;
	BN_Variable * rightparent;
	BN_operation parent_operation;
	BN_variable_iotype BN_iotype;
	vector<vector<coords>> Remove_redundant(vector<double> & in,  vector<coords> &c);
	vector<vector<coords>> Weaken(vector<double> & in,  vector<vector<coords>> &c, int number_of_entries);
	vector<double> Compute(vector<double> in1, vector<double> in2, BN_operation op, vector<coords> &c);
	void Improve_ranges(int number_of_iterations);
	void DoubleSort(vector<coords> & inds, vector<double> &vals, int first, int last);
public:
	BN_Variable (string varname, int &freevar, double left, double right, int number_of_entries, BN_variable_iotype iotype);
	BN_Variable (string varname, int &freevar, BN_Variable &left_par, BN_Variable& right_par, BN_operation op, BN_variable_iotype iotype,  int number_of_entries);
	string Print();
};
BN_Variable::BN_Variable(string varname, int &freevar, double left, double right, int number_of_entries, BN_variable_iotype iotype){
	caption=varname;
	BN_iotype=iotype;
	leftparent=NULL;
	rightparent=NULL;
	leftborder=left;
	rightborder=right;
	lattice_step=(right-left)/number_of_entries;	
	double hstep=lattice_step/2;
	for (int i=0;i<number_of_entries+1;i++){
		BN_range a(left+lattice_step*i - hstep,left+lattice_step*i+hstep,left+lattice_step*i);
		ranges.push_back(a);

		encoding_vars.push_back(++freevar);
		vars_values.push_back(left+lattice_step*i);				
	}

}
vector<double> BN_Variable::Compute(vector<double> in1, vector<double> in2, BN_operation op, vector<coords> &c){
	vector<double> r;
	switch (op){
	case BN_operation::Degree:
		for (int i=0;i<in1.size();i++){
			for (int j=0;j<in2.size();j++){
				coords a;
				a.i=i;
				a.j=j;
				c.push_back(a);
				r.push_back(pow(in1[i],in2[j]));
			}
		}
		break;
	case BN_operation::Divide:
		for (int i=0;i<in1.size();i++){
			for (int j=0;j<in2.size();j++){
				coords a;
				a.i=i;
				a.j=j;
				c.push_back(a);
				if (in2[j]==0){r.push_back(MAXINT32);} else {r.push_back(in1[i]/in2[j]);}	
			}
		}
		break;
	case BN_operation::Log:
		for (int i=0;i<in1.size();i++){
				coords a;
				a.i=i;
				a.j=-1;
				c.push_back(a);
				r.push_back(log(in1[i]));			
		}
		break;
	case BN_operation::Minus:
		for (int i=0;i<in1.size();i++){
			for (int j=0;j<in2.size();j++){
				coords a;
				a.i=i;
				a.j=j;
				c.push_back(a);
				r.push_back(in1[i]-in2[j]);
			}
		}
		break;
	case BN_operation::Neg:
		for (int i=0;i<in1.size();i++){
			coords a;
			a.i=i;
			a.j=0;
			c.push_back(a);
			r.push_back(-in1[i]);			
		}
		break;
	case BN_operation::Plus:
		for (int i=0;i<in1.size();i++){
			for (int j=0;j<in2.size();j++){
				coords a;
				a.i=i;
				a.j=j;
				c.push_back(a);
				r.push_back(in1[i]+in2[j]);
			}
		}
		break;
	case BN_operation::Times:
		for (int i=0;i<in1.size();i++){
			for (int j=0;j<in2.size();j++){
				coords a;
				a.i=i;
				a.j=j;
				c.push_back(a);
				r.push_back(in1[i]*in2[j]);
			}
		}
		break;
	default:
		break;
	}
	return r;
}

void BN_Variable::Improve_ranges(int number_of_iterations){
	for (int i=0;i<number_of_iterations;i++){
		for (int j=0;j<ranges.size()-1;j++){	
			//try 1, move mostright value to neighbour
			double d1=ranges[j].compute_deviation(false,true, false,0);
			double d2=ranges[j+1].compute_deviation(false,false,true,ranges[j].mostright());
			
			while ((d1<ranges[j].dev())&&(d2<ranges[j+1].dev())){
					double tmp=ranges[j].mostright();
					ranges[j].removefromrange(tmp);
					ranges[j+1].addtorange(tmp);		
					d1=ranges[j].compute_deviation(false,true, false,0);
					d2=ranges[j+1].compute_deviation(false,false,true,ranges[j].mostright());				
			}

			//try 2, take mostleft value from neighbour
			double d3=ranges[j].compute_deviation(false,false,true,ranges[j+1].mostleft());
			double d4=ranges[j+1].compute_deviation(true,false,false,0);

			while ((d3<ranges[j].dev())&&(d4<ranges[j+1].dev())){
					double tmp=ranges[j+1].mostleft();
					ranges[j].addtorange(tmp);		
					ranges[j+1].removefromrange(tmp);
					d3=ranges[j].compute_deviation(false,false,true,ranges[j+1].mostleft());
					d4=ranges[j+1].compute_deviation(true,false,false,0);
			}		
		}
	}
}

void BN_Variable::DoubleSort(vector<coords> & inds, vector<double> &vals, int first, int last){
	int i=first;
	int j=last;
	int x=vals[(first+last)/2];

	do {
		while (vals[i]<x) i++;
		while (vals[j]>x) j--;

		if (i<=j){
			if (i<j) {
				double tmp=vals[j];
				vals[j]=vals[i];
				vals[i]=tmp;
				coords tmpi=inds[j];
				inds[j]=inds[i];
				inds[i]=tmpi;				
			}
			i++;
			j--;
		}
	} while (i<=j);

	if (i<last) {DoubleSort (inds,vals,i,last);}
	if (first<j){DoubleSort (inds,vals,first,j);}
}
vector<vector<coords>> BN_Variable::Remove_redundant(vector<double> & in,  vector<coords> &c){
	vector<double> src;
	vector<vector<coords>> res;
	int j=0;
	for (int i=0;i<in.size();i++){
		src.push_back(in[i]);
		vector<coords> a;
		a.push_back(c[i]);
		int j=i+1;
		if (j<in.size()){
			while (in[i]==in[j]){
				a.push_back(c[j]);
				if (j<in.size()-1)j++;
			}				
		}
		res.push_back(a);
		i=j-1;
	}	
	in=src;

	for (int i=0;i<in.size();i++){
		double lhs=(i==0)?in[i]-(in[i+1]-in[i])/2:(in[i]-in[i-1])/2;
		double rhs=(i==in.size()-1)?in[i]-(in[i]-in[i-1])/2:(in[i+1]-in[i])/2;
		BN_range b(in[i]-lhs, in[i]+rhs, in[i]);
		ranges.push_back(b);
	}
	return res;
}

vector<vector<coords>> BN_Variable::Weaken(vector<double> & in,  vector<vector<coords>> &c, int number_of_entries){
	double lb=in[0];
	double rb=in[in.size()-1];
	leftborder=lb;
	rightborder=rb;
	lattice_step=(rb-lb)/number_of_entries;
	vector<double> src;
	vector<vector<coords>> res;
	ranges.clear();
	int j=0;
	for (int i=0;i<=number_of_entries;i++){
		src.push_back(lb+i*lattice_step);
		vector<coords> a;
		vector<coords> a_t;
		vector<double> b;
		while ((in[j]<src[i]+lattice_step/2)&&(in[j]>=src[i]-lattice_step/2)){
			b.push_back(in[j]);
			
			vector<coords> a_t=c[j];
			for (int g=0;g<a_t.size();g++){
				a.push_back(a_t[g]);
			}
			j++;
			if (j==in.size()) break;
		}
		BN_range tmp(src[i]-lattice_step/2,src[i]+lattice_step/2,b);
		ranges.push_back(tmp);
		res.push_back(a);
	}	
	in=src;
	return res;
}

BN_Variable::BN_Variable(string varname, int &freevar, BN_Variable& left_par, BN_Variable& right_par, BN_operation op, BN_variable_iotype iotype, int number_of_entries){
	caption=varname;
	BN_iotype=iotype;
	leftparent=&left_par;
	rightparent=&right_par;

	vector<double> t;
	vector<coords> indexes;
	t = Compute(leftparent->vars_values, rightparent->vars_values, op, indexes);
	DoubleSort(indexes,t,0,indexes.size()-1); //sorted this ..

	vector<vector<coords>> r=Remove_redundant(t,indexes);
	if (t.size()>number_of_entries){
		r=Weaken(t,r,number_of_entries);
		Improve_ranges(3);
	}

	for (int i=0;i<r.size();i++){
		vector<coords> a=r[i];
		vector<Logical_statement> rp;
		for (int j=0;j<a.size();j++){
			vector<Logical_statement> tempv;
			Logical_statement temp1(leftparent->encoding_vars[a[j].i]);
			Logical_statement temp2(rightparent->encoding_vars[a[j].j]);
			tempv.push_back(temp1);
			tempv.push_back(temp2);
			Logical_statement temp3;
			temp3.Create_LS(LS_operation::AND,tempv);
			rp.push_back(temp3);
		}
		Logical_statement rple;
		encoding_vars.push_back(++freevar);
		rple.Create_LS(LS_operation::OR,rp);
		Logical_equation nle(encoding_vars[i],LE_equation_type::Assign,rple);
		cout<<endl<<nle.Print();
		Equations.push_back(nle);
	}
}
class Boolean_network{
private:
	vector<BN_Variable> BN_variable_list;
	vector<Logical_equation> Equations_list;
};

class BN_token{
private:
	string token;	// token itself
	int istart;		// position of token first symbol in string
	int length;		// length of the token
	int depth;		// depth in terms of braces, depth == 3 if there were 3 '(' braces before and 0 ')'
public:
	int start_ind (){return istart;}
	int len_ind (){return length;}
	BN_token(){
		token="";
		istart=0;
		length=0;
		depth=0;
	}
	BN_token(char t, int i_s, int d){
		token=t;
		istart=i_s;
		length=1;
		depth=d;
	}
	BN_token(string t, int i_s, int l, int d){
		token=t;
		istart=i_s;
		length=l;
		depth=d;
	}
};

enum class BN_parsing_error{need_opening_brace, need_closing_brace, unknown_token_name};

class op_node{
	BN_operation op;
	op_node * left;
	op_node * right;
	op_node *parent;
	int arity;
public:
	op_node(BN_operation operation, op_node &par, op_node &l_child, op_node &r_child){
		op=operation;
		parent=&par;
		left=&l_child;
		right=&r_child;
		arity=Arity_BN(op);
	}	
	string print (){
		string res;
		string tmp=op_txt(op);
		bool isbetween=false;
		if (tmp.length()==1) isbetween=true;

		if (isbetween==false) res=tmp+"(";

		if (left!=NULL){
			res+=left->print();
		}
		else cout<<endl<<"Operation tree error, null left pointer";

		if (arity==2){			
			if (isbetween) res +=" "+ tmp+" ";
			if (right!=NULL){
				res+=right->print();
			}
			else cout<<endl<<"Operation tree error, null right pointer";
		}	
		if (isbetween==false) res+= ")";		
		if (parent!=NULL) {res ="("+res+")";}
		return res;
	}
	
};

class op_tree{
private:
	op_node root;
	string text_ver;
public:
	

};
class BN_parser{	
public:
	void parse (string in);

};
void BN_parser::parse (string in){
	//€вно выдел€ем односимвольные операции +-*/
	//обрабатываем многосимвольные log(), sin() , pow() вы€вл€€ скобки, которые после них сто€т
	
	vector<BN_token> tokens;
	int d=0;
	for (int i=0;i<in.size();i++){
		switch (in[i]){
		case '+':
		case '-':
		case '*':
		case '/':
		case ',':
		case '=':
				{
					BN_token a(in[i],i,d);
					tokens.push_back(a);
				 }
			break;
		case '(':{
					string tmp;
					int h=0;
					if (tokens.size()>0){
						h=tokens[tokens.size()-1].start_ind();
					}
					tmp=in.substr(h+1,i-h-1);
					BN_token a_t(tmp,i,tmp.length(),d);
					tokens.push_back(a_t);

					d++;
					BN_token a(in[i],i,d);
					tokens.push_back(a);
				 }
			break;
		case ')':{
					d--;
					BN_token a(in[i],i,d);
					tokens.push_back(a);
				 }
			break;			
		default:
			break;
		}
	}


}

int main (){
	int nVars;
	nVars=0;
	string test="a+b+c+log(d)+pos(x,y)";
	BN_parser p;
	p.parse(test);


	BN_Variable a("a", nVars,0,10,10,BN_variable_iotype::Input);
	BN_Variable b("b", nVars,40,60,20,BN_variable_iotype::Input);
	BN_Variable c("c", nVars, a,b, BN_operation::Plus, BN_variable_iotype::Input, 10);

	cout<<"Work in progress"<<endl;
	int typein;
	cin>>typein;
}