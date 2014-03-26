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
class BN_Variable{
private:
	vector<Logical_equation> Equations;
	vector<int> encoding_vars;
	vector<double> vars_values;
	double leftborder;
	double rightborder;
	double lattice_step;
	BN_Variable * leftparent;
	BN_Variable * rightparent;
	BN_operation parent_operation;
	BN_variable_iotype BN_iotype;
	vector<vector<coords>> Remove_redundant(vector<double> & in,  vector<coords> &c);
	vector<vector<coords>> Weaken(vector<double> & in,  vector<coords> &c, int number_of_entries);
	vector<double> Compute(vector<double> in1, vector<double> in2, BN_operation op, vector<coords> &c);
	void DoubleSort(vector<coords> & inds, vector<double> &vals, int first, int last);
public:
	BN_Variable (int &freevar, double left, double right, int number_of_entries, BN_variable_iotype iotype);
	BN_Variable (int &freevar, BN_Variable &left_par, BN_Variable& right_par, BN_operation op, BN_variable_iotype iotype,  int number_of_entries);
	string Print();
};
BN_Variable::BN_Variable(int &freevar, double left, double right, int number_of_entries, BN_variable_iotype iotype){
	BN_iotype=iotype;
	leftparent=NULL;
	rightparent=NULL;
	leftborder=left;
	rightborder=right;
	lattice_step=(right-left)/number_of_entries;	
	for (int i=0;i<number_of_entries+1;i++){
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
	return res;
}
vector<vector<coords>> BN_Variable::Weaken(vector<double> & in,  vector<coords> &c, int number_of_entries){
	double lb=in[0];
	double rb=in[in.size()-1];
	leftborder=lb;
	rightborder=rb;
	lattice_step=(rb-lb)/number_of_entries;
	vector<double> src;
	vector<vector<coords>> res;
	int j=0;
	for (int i=0;i<=number_of_entries;i++){
		src.push_back(lb+i*(rb-lb));
		vector<coords> a;
		while ((in[j]<=src[i]+(rb-lb)/2)&&(in[j]>src[i]-(rb-lb)/2)){
			a.push_back(c[j]);
			j++;
		}
		res.push_back(a);
	}	
	in=src;
	return res;
}

BN_Variable::BN_Variable(int &freevar, BN_Variable& left_par, BN_Variable& right_par, BN_operation op, BN_variable_iotype iotype, int number_of_entries){
	BN_iotype=iotype;
	leftparent=&left_par;
	rightparent=&right_par;

	vector<double> t;
	vector<coords> indexes;
	t = Compute(leftparent->vars_values, rightparent->vars_values, op, indexes);	
	DoubleSort(indexes,t,0,indexes.size()-1); //sorted this ..
	vector<vector<coords>> r=Remove_redundant(t,indexes);
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


int main (){
	int nVars;
	nVars=0;
	BN_Variable a(nVars,0,10,10,BN_variable_iotype::Input);
	BN_Variable b(nVars,40,60,20,BN_variable_iotype::Input);
	BN_Variable c(nVars, a,b, BN_operation::Plus, BN_variable_iotype::Input, 10);

	cout<<"Work in progress"<<endl;
	int typein;
	cin>>typein;
}