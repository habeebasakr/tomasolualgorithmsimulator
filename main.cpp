//
//  main.cpp
//  Project
//
//  Created by Habeeba Sakr on 12/8/20.
//  Copyright © 2020 Habeeba Sakr. All rightread reserved.
//
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <queue>
using namespace std;
int PC=0;
map <string, int>RegisterStatus;
int Cycles=0;
bool databus=false;
int result;
int branchesencountered=0;
int branchesfalse=0;
int mispredictor=0;
int written=0;
bool Stall_Iss=false;
bool stall_exec=false;


struct RS
{
    bool Busy=false;
    string Op = " ";
    string Vj=" ";
    string Vk=" ";
    string Qj= " ";
    string Qk=" ";
    int A=0;
    int execCycles = 0;

};

RS RStations[9];
map<string, string> RegFile;
int offset=200;

struct Instruction
{
    string op;
    string rd, operand1, operand2;
    int imm;
    int support1=-1;
    int support2=-1;
    int issuing=0;
    int startexec=0;
    int finishexec=0;
    int writing=0;
    int result=0;
    int PCissue=0;
    bool finished=false;
    int cyclesafterexec=0;
    int stalledcycle=0;
};

vector <Instruction> instt;
string memory[65536];
queue <Instruction> LSqueue;
queue <Instruction> instructions;
vector <Instruction> instructionsissued;

void issue(int i)
{
  if(instt[i].support1!=-1)
  {  if (RStations[instt[i].support1].Busy==false)
  {     if ((instt[i].op == "ADD") || (instt[i].op == "DIV"))
            {
                if(RegisterStatus[instt[i].operand1]!=0) //reservation station number, if register is occupied
                { RStations[instt[i].support1].Qj= RegisterStatus[instt[i].operand1];}//put in qj
                else
                {
                    RStations[instt[i].support1].Vj=RegFile[instt[i].operand1];// put value in vj regfile
                    RStations[instt[i].support1].Qj="0";//set qj to zero
                }
                
                if(RegisterStatus[instt[i].operand2]!=0) //reservation station number
                { RStations[instt[i].support1].Qk= RegisterStatus[instt[i].operand2];}
                else
                {
                    RStations[instt[i].support1].Vk=RegFile[instt[i].operand2];
                    RStations[instt[i].support1].Qk="0";
                }
                RStations[instt[i].support1].Busy=true; //rs is now busy
                RegisterStatus[instt[i].rd]= instt[i].support1; //which RS we are currently in
                instt[i].issuing=Cycles; //
                PC++;
                instructionsissued.push_back(instt[i]);
            }
             else if (instt[i].op == "NEG" || instt[i].op == "ADDI" )
            {
                        if(RegisterStatus[instt[i].operand1]!=0) //reservation station number
                               { RStations[instt[i].support1].Qj= RegisterStatus[instt[i].operand1];}
                               else
                               {
                                   RStations[instt[i].support1].Vj=RegFile[instt[i].operand1];
                                   RStations[instt[i].support1].Qj="0";
                               }
                               RStations[instt[i].support1].Busy=true;
                               RegisterStatus[instt[i].rd]= instt[i].support1; //which RS
                               instt[i].issuing=Cycles;
                                PC++;
                                instructionsissued.push_back(instt[i]);


            } else  if (instt[i].op == "SW" )
            {
                            if(RegisterStatus[instt[i].operand1]!=0) //reservation station number
                               { RStations[instt[i].support1].Qj= RegisterStatus[instt[i].operand1];}
                               else
                               {
                                   RStations[instt[i].support1].Vj=RegFile[instt[i].operand1];
                                   RStations[instt[i].support1].Qj="0";
                               }
                                if(RegisterStatus[instt[i].operand2]!=0) //reservation station number
                                { RStations[instt[i].support1].Qk= RegisterStatus[instt[i].operand2];}
                                else
                                {
                                RStations[instt[i].support1].Vk=RegFile[instt[i].operand2];
                                RStations[instt[i].support1].Qk="0";
                                               }
                               RStations[instt[i].support1].Busy=true;
                               RStations[instt[i].support1].A=instt[i].imm;
                               instt[i].issuing=Cycles;
                                PC++;
                             LSqueue.push(instt[i]);
                            instructionsissued.push_back(instt[i]);

                
            }else  if (instt[i].op == "LW" )
                {
                                    if(RegisterStatus[instt[i].operand1]!=0) //reservation station number
                                   { RStations[instt[i].support1].Qj= RegisterStatus[instt[i].operand1];}
                                   else
                                   {
                                       RStations[instt[i].support1].Vj=RegFile[instt[i].operand1];
                                       RStations[instt[i].support1].Qj="0";
                                   }
                                   RStations[instt[i].support1].Busy=true;
                                   RStations[instt[i].support1].A=instt[i].imm;
                                   RegisterStatus[instt[i].rd]= instt[i].support1; //which RS
                                   instt[i].issuing=Cycles;
                                    PC++;
                                LSqueue.push(instt[i]);
                                instructionsissued.push_back(instt[i]);


                }
            else  if (instt[i].op == "BEQ" )
            {
                           
                            if(RegisterStatus[instt[i].operand1]!=0) //reservation station number
                             { RStations[instt[i].support1].Qj= RegisterStatus[instt[i].operand1];}
                             else
                             {
                                 RStations[instt[i].support1].Vj=RegFile[instt[i].operand1];
                                 RStations[instt[i].support1].Qj="0";
                             }
                             
                             if(RegisterStatus[instt[i].operand2]!=0) //reservation station number
                             { RStations[instt[i].support1].Qk= RegisterStatus[instt[i].operand2];}
                             else
                             {
                                 RStations[instt[i].support1].Vk=RegFile[instt[i].operand2];
                                 RStations[instt[i].support1].Qk="0";
                             }
                             RStations[instt[i].support1].Busy=true;
                             RStations[instt[i].support1].A=instt[i].imm+PC;
                             RegisterStatus[instt[i].rd]= instt[i].support1; //which RS
                             instt[i].issuing=Cycles;
                            instt[i].PCissue=PC;
                             PC++;
                            instructionsissued.push_back(instt[i]);
                            branchesencountered++;
                            stall_exec=true;
                            
            }else  if (instt[i].op == "JALR" )
            {

                if(RegisterStatus[instt[i].operand1]!=0) //reservation station number
                 { RStations[instt[i].support1].Qj= RegisterStatus[instt[i].operand1];}
                 else
                 {
                     RStations[instt[i].support1].Vj=RegFile[instt[i].operand1];
                     RStations[instt[i].support1].Qj="0";
                 }
                 RStations[instt[i].support1].Busy=true;
                 RegisterStatus[instt[i].rd]= instt[i].support1; //which RS
                 instt[i].issuing=Cycles;
                 instt[i].PCissue=PC;

                 PC++;
                Stall_Iss=true;
                instructionsissued.push_back(instt[i]);
                    

            }else  if (instt[i].op == "RET" )
            {
                           
                            if(RegisterStatus["R1"]!=0) //reservation station number
                             { RStations[instt[i].support1].Qj= RegisterStatus["R1"];}
                             else
                             {
                                 RStations[instt[i].support1].Vj=RegFile["R1"];
                                 RStations[instt[i].support1].Qj="0";
                             }

                            Stall_Iss=true;
                            instt[i].issuing=Cycles;
                            instructionsissued.push_back(instt[i]);
            }
        
            
  }
  
  else if (instt[i].support2!=-1)
      {  if (RStations[instt[i].support2].Busy==false)
        {     if ((instt[i].op == "ADD") || (instt[i].op == "DIV"))
                  {
                      if(RegisterStatus[instt[i].operand1]!=0) //reservation station number, if register is occupied
                      { RStations[instt[i].support2].Qj= RegisterStatus[instt[i].operand1];}//put in qj
                      else
                      {
                          RStations[instt[i].support2].Vj=RegFile[instt[i].operand1];// put value in vj regfile
                          RStations[instt[i].support2].Qj="0";//set qj to zero
                      }
                      
                      if(RegisterStatus[instt[i].operand2]!=0) //reservation station number
                      { RStations[instt[i].support2].Qk= RegisterStatus[instt[i].operand2];}
                      else
                      {
                          RStations[instt[i].support2].Vk=RegFile[instt[i].operand2];
                          RStations[instt[i].support2].Qk="0";
                      }
                      RStations[instt[i].support2].Busy=true; //rs is now busy
                      RegisterStatus[instt[i].rd]= instt[i].support2; //which RS we are currently in
                      instt[i].issuing=Cycles; //
                      PC++;
                        instructionsissued.push_back(instt[i]);

                  }
                   else if (instt[i].op == "NEG" || instt[i].op == "ADDI" )
                  {
                              if(RegisterStatus[instt[i].operand1]!=0) //reservation station number
                                     { RStations[instt[i].support2].Qj= RegisterStatus[instt[i].operand1];}
                                     else
                                     {
                                         RStations[instt[i].support2].Vj=RegFile[instt[i].operand1];
                                         RStations[instt[i].support2].Qj="0";
                                     }
                                     RStations[instt[i].support2].Busy=true;
                                     RegisterStatus[instt[i].rd]= instt[i].support2; //which RS
                                     instt[i].issuing=Cycles;
                                    PC++;
                                    instructionsissued.push_back(instt[i]);


                  } else  if (instt[i].op == "SW" )
                  {
                                  if(RegisterStatus[instt[i].operand1]!=0) //reservation station number
                                     { RStations[instt[i].support2].Qj= RegisterStatus[instt[i].operand1];}
                                     else
                                     {
                                         RStations[instt[i].support2].Vj=RegFile[instt[i].operand1];
                                         RStations[instt[i].support2].Qj="0";
                                     }
                                      if(RegisterStatus[instt[i].operand2]!=0) //reservation station number
                                      { RStations[instt[i].support1].Qk= RegisterStatus[instt[i].operand2];}
                                      else
                                      {
                                      RStations[instt[i].support2].Vk=RegFile[instt[i].operand2];
                                      RStations[instt[i].support2].Qk="0";
                                                     }
                                     RStations[instt[i].support2].Busy=true;
                                     RStations[instt[i].support2].A=instt[i].imm;
                                     instt[i].issuing=Cycles;
                                    PC++;
                      LSqueue.push(instt[i]);
                        instructionsissued.push_back(instt[i]);


                  }else  if (instt[i].op == "LW" )
                      {
                                          if(RegisterStatus[instt[i].operand1]!=0) //reservation station number
                                         { RStations[instt[i].support2].Qj= RegisterStatus[instt[i].operand1];}
                                         else
                                         {
                                             RStations[instt[i].support2].Vj=RegFile[instt[i].operand1];
                                             RStations[instt[i].support2].Qj="0";
                                         }
                                         RStations[instt[i].support2].Busy=true;
                                         RStations[instt[i].support2].A=instt[i].imm;
                                         RegisterStatus[instt[i].rd]= instt[i].support2; //which RS
                                         instt[i].issuing=Cycles;
                                         PC++;
                          LSqueue.push(instt[i]);
                          instructionsissued.push_back(instt[i]);


                      }
               
              
                  
            }
      }
    
}
}
void execute(int i)
{
 
    int vj;
    int vk;
        for (int j = 0; j < 9; j++)
            {if (RStations[j].Busy == true)
            {
                if (instt[i].startexec == 0)
                    instt[i].startexec = Cycles+1;
                               
                if (RStations[j].Vj !=" ")
                   vj= stoi(RStations[j].Vj);
                    if(RStations[j].Vk!=" ")
                        vk= stoi(RStations[j].Vk);
                
                        if (instt[i].op == "ADD")
                        { if (RStations[j].Qj == "0" && RStations[j].Qk == "0")
                        {    instt[i].result = vj+ vk ;

                            }
                            }
                        else if (instt[i].op == "NEG")
                        { if (RStations[j].Qj == "0" && RStations[j].Qk == "0")
                        {
                               instt[i].result = ~vj;
                               
                        }
                            
                        }
                        else if (instt[i].op == "DIV")
                { if (RStations[j].Qj == "0" && RStations[j].Qk == "0")
                {
                       instt[i].result = vj / vk;

                }
                   }  else if (instt[i].op =="LW")
                    {   if (RStations[j].Qj == "0")
                            {
                                RStations[j].A += vj;
                                instt[i].result = stoi(memory[RStations[j].A]);
                                
                                }
                    }
                        else if (instt[i].op =="SW")
                                {   if (RStations[j].Qj == "0")
                                    {
                                        RStations[j].A += vj;
                                        instt[i].result= vk;
                                    }
                                }else if (instt[i].op =="BEQ")
                                {   if (RStations[j].Qj == "0")
                                    {if (vj==vk)
                                    { instt[i].result=RStations[j].A;
                                        mispredictor++;
                                    }
                                    } else
                                    {   result=instt[i].PCissue;
                                            }
                                    }
                            
                                        else if (instt[i].op =="JALR")
                                            {   if (RStations[j].Qj == "0")
                                                {
                                                    instt[i].result=vj;
                        
                                                    }
                                                }
                                    else if (instt[i].op =="RET")
                        {   if (RStations[j].Qj == "0")
                                     {
                                         instt[i].result=vj;
                                        }
                                }
                    }
       
            }
  
    
    
}

    
    
    

void writeback(int i)
{   if (databus==true)
{
    instt[i].stalledcycle++;
}
    if (databus==false)
    {
        instt[i].writing=1+instt[i].stalledcycle+instt[i].finishexec;
       
        databus=true;
        written++;
         if ((instt[i].op == "ADD")|| (instt[i].op == "DIV"))
                    {
                        RegFile[instt[i].rd]=instt[i].result;
                        RegisterStatus[instt[i].rd]=0;
                        
                        for (int j=0; j<9; j++)
                        {
                          if( RStations[j].Qj == to_string(instt[i].support1))
                          {
                              RStations[j].Vj =instt[i].result;
                              RStations[j].Qj = to_string('0');
                          }
                              
                         if( RStations[j].Qj == to_string(instt[i].support2))
                         {
                             RStations[j].Vj =instt[i].result;
                             RStations[j].Qj = to_string('0');
                         }
                        if( RStations[j].Qk == to_string(instt[i].support1))
                            {
                                RStations[j].Vk =instt[i].result;
                                RStations[j].Qk = to_string('0');
                            }
                            
                        if( RStations[j].Qk == to_string(instt[i].support2))
                            {
                            RStations[j].Vk =instt[i].result;
                            RStations[j].Qk = to_string('0');
                            }

                        }
                        
                    }
                     else if (instt[i].op == "NEG" || instt[i].op == "ADDI" )
                    {
                               RegFile[instt[i].rd]=result;
                                RegisterStatus[instt[i].rd]=0;
                                
                                for (int j=0; j<9; j++)
                                {
                                  if( RStations[j].Qj == to_string(instt[i].support1))
                                  {
                                      RStations[j].Vj =instt[i].result;
                                      RStations[j].Qj = to_string('0');
                                  }
                                      
                                 if( RStations[j].Qj == to_string(instt[i].support2))
                                 {
                                     RStations[j].Vj =instt[i].result;
                                     RStations[j].Qj = to_string('0');
                                 }
                                }
                    } else  if (instt[i].op == "SW" )
                    {
                        Instruction compare=LSqueue.front();
                        if (compare.issuing == instt[i].issuing)
                        {
                            memory[RStations[instt[i].support1].A]=instt[i].result;
                            
                            
                        }
//                        memory[RStations[inst.support1].A]=
//                            for (int i=0; i<9; i++)
                        LSqueue.pop();
                    }else  if (instt[i].op == "LW" )
                    {   Instruction compare=LSqueue.front();
                                if (compare.issuing == instt[i].issuing)
                                {RegFile[instt[i].rd]=instt[i].result;
                                RegisterStatus[instt[i].rd]=0;
                                LSqueue.pop();
                                 for (int j=0; j<9; j++)
                                   {
                            if( RStations[j].Qj == to_string(instt[i].support1))
                            {
                                    RStations[j].Vj =instt[i].result;
                                RStations[j].Qj = to_string('0');
                                    }
                                                                            
                            if( RStations[j].Qj == to_string(instt[i].support2))
                            {
                                RStations[j].Vj =instt[i].result;
                                RStations[j].Qj = to_string('0');
                            }
                                   }
                                }
                        }
        
        
                    else  if (instt[i].op == "BEQ" )
                    {
                        PC=instt[i].result;
                        for (int i=0; i<instructionsissued.size(); i++)
                        {
                           if( instructionsissued[i].issuing> instt[i].issuing)
                           {
                               RStations[instructionsissued[i].support1].Busy=false;
                                RStations[instructionsissued[i].support1].Op = " ";
                              RStations[instructionsissued[i].support1].Vj=" ";
                                RStations[instructionsissued[i].support1].Vk=" ";
                               RStations[instructionsissued[i].support1].Qj= " ";
                               RStations[instructionsissued[i].support1].Qk=" ";
                               RStations[instructionsissued[i].support1].A= 0;
                           }
                            
                            stall_exec=false;
                        }
                    }
                    else  if (instt[i].op == "JALR" )
                    {
                        PC=instt[i].result;
                        Stall_Iss=false;
                        
                        
                    }else  if (instt[i].op == "RET" )
                    {
                        PC=instt[i].result;
                        Stall_Iss=false;

                    }
                
        
        
    }
    databus=false;

}
    

void parse()
{   Instruction inst;
    ifstream inFile;
    inFile.open("/Users/habeebasakr/Desktop/Project/Project/arithm.txt");
     string read;
    
    if (!inFile) {
        inFile.close();
        cout << "can't open file " <<endl;
        return;
      }
    int counter = 0;

    while (getline (inFile, read)){
        
        cout<< read << endl;
        memory[counter] = read;
        counter++;
        
        inst.op=read.substr(0, read.find(" "));
        string modifiedinst;
        if(inst.op != read)
            modifiedinst=read.substr(inst.op.length()+1);
        
        if (inst.op=="ADD")
             
                {
                    inst.rd = modifiedinst.substr(0, modifiedinst.find_first_of(','));
                    modifiedinst = modifiedinst.substr(inst.rd.length()+2, modifiedinst.length());
                    inst.operand1 = modifiedinst.substr(0, modifiedinst.find_first_of(','));
                    modifiedinst = modifiedinst.substr(inst.operand1.length()+2, modifiedinst.length());
                    inst.operand2 = modifiedinst;
                    inst.support1=6;
                    inst.support2=7;

                } else if (inst.op=="ADDI")
                {
                    inst.rd = modifiedinst.substr(0, modifiedinst.find_first_of(','));
                    modifiedinst = modifiedinst.substr(inst.rd.length()+2, modifiedinst.length());
                    inst.operand1 = modifiedinst.substr(0, modifiedinst.find_first_of(','));
                    modifiedinst =modifiedinst.substr(inst.operand1.length()+2,modifiedinst.length());
                    inst.imm = stoi(modifiedinst);
                    inst.support1=6;
                    inst.support2=7;

                } else if (inst.op=="DIV")
                  {
                      inst.rd = modifiedinst.substr(0, modifiedinst.find_first_of(','));
                      modifiedinst = modifiedinst.substr(inst.rd.length()+2, modifiedinst.length());
                      inst.operand1 = modifiedinst.substr(0, modifiedinst.find_first_of(','));
                      modifiedinst = modifiedinst.substr(inst.operand1.length()+2, modifiedinst.length());
                      inst.operand2 = modifiedinst;
                      inst.support1=8;

                  }
        else if (inst.op=="NEG")
        {
            inst.rd = modifiedinst.substr(0, modifiedinst.find_first_of(','));
            modifiedinst = modifiedinst.substr(inst.rd.length()+2, modifiedinst.length());
            inst.operand1 = modifiedinst;
         
             inst.support1=6;
             inst.support2=7;

        }
        else if (inst.op=="BEQ")
        {
            inst.operand1 = modifiedinst.substr(0, modifiedinst.find_first_of(','));
             modifiedinst = modifiedinst.substr(inst.operand1.length()+2, modifiedinst.length());
            inst.operand2 = modifiedinst.substr(0, modifiedinst.find_first_of(','));
              modifiedinst =modifiedinst.substr(inst.operand2.length()+2,modifiedinst.length());
            inst.imm = stoi(modifiedinst);
            inst.support1=4;

        } else if (inst.op=="LW")
            {
                        inst.rd = modifiedinst.substr(0, modifiedinst.find_first_of(','));
                        modifiedinst = modifiedinst.substr(inst.rd.length()+2, modifiedinst.length());
                         inst.imm = stoi(modifiedinst.substr(0, modifiedinst.find_first_of('(')));
                        modifiedinst = modifiedinst.substr(3, modifiedinst.length());
                        inst.operand1 = modifiedinst.substr(0, modifiedinst.find(')'));
                inst.support1=0;
                inst.support2=1;
            }else if (inst.op=="SW")
                {
                            inst.operand2 = modifiedinst.substr(0, modifiedinst.find_first_of(','));
                            modifiedinst = modifiedinst.substr(inst.rd.length()+2, modifiedinst.length());
                            inst.imm = stoi(modifiedinst.substr(0, modifiedinst.find_first_of('(')));
                            modifiedinst = modifiedinst.substr(3, modifiedinst.length());
                            inst.operand1 = modifiedinst.substr(0, modifiedinst.find(')'));
                        inst.support1=2;
                        inst.support2=3;
                        
                } else if (inst.op == "JALR")
                {
                    inst.operand1=modifiedinst;
                    inst.support1=5;

                }else if (inst.op == "RET")
                {
                    inst.support1=5;

                }
    
        instt.push_back(inst);
        instructions.push(inst);
    }
    inFile.close();
    
}

void print_table(){
   
    cout<<"Inst    "<<"cycle issued    "<<"cycle start exec   "<<"cycle finish exec   "<<"cycle wb"<<endl;

    for (int i=0; i<instt.size(); i++)
    {
        cout<<instt[i].op <<"    "<<instt[i].issuing<<"    "<<instt[i].startexec<<"   "<<instt[i].finishexec<<"   "<<instt[i].writing<< endl;
        
    }
    
   }

int main(int argc, const char * argv[]) {
    
    RStations[0].Op="Load1";
    RStations[1].Op="Load2";
    RStations[2].Op="Store1";
    RStations[3].Op="Store2";
    RStations[4].Op="Branch";
    RStations[5].Op="Jalr";
    RStations[6].Op="Arith";
    RStations[7].Op="Arith";
    RStations[8].Op="DIV";
    
    RStations[0].execCycles = 2;
    RStations[1].execCycles = 2;
    RStations[2].execCycles = 2;
    RStations[3].execCycles = 2;
    RStations[4].execCycles = 1;
    RStations[5].execCycles = 1;
    RStations[6].execCycles = 2;
    RStations[7].execCycles = 2;
    RStations[8].execCycles = 8;
    
    RegisterStatus["R0"]=0;
    RegisterStatus["R1"]=0;
    RegisterStatus["R2"]=0;
    RegisterStatus["R3"]=0;
    RegisterStatus["R4"]=0;
    RegisterStatus["R5"]=0;
    RegisterStatus["R6"]=0;
    RegisterStatus["R7"]=0;

    RegFile["R0"]="0";
    RegFile["R1"]="1";
    RegFile["R2"]="3";
    RegFile["R3"]="3";
    RegFile["R4"]="2";
    RegFile["R5"]="0";
    RegFile["R6"]="0";
    RegFile["R7"]="0";
    
    int PCMax;
    cout<<"Input number of instructions: "<<endl;
    cin>>PCMax;
    
   
    
    parse();
    while (PC<=PCMax-1)
    {  cout<<"PC"<<PC<<endl;
        Cycles++;
        
        if (Stall_Iss==false)
        {
            issue(PC);
        }
        
        for (int i=0; i<instt.size(); i++)
        {
            int execloop=RStations[instt[i].support1].execCycles;
            if (instt[i].issuing!=0 && (stall_exec==false) &&(instt[i].finished==false))
            {  while(execloop>0)
            { execute(i);
                execloop--;
                
            }
                instt[i].finishexec=Cycles+RStations[instt[i].support1].execCycles;
                instt[i].finished=true;
            }
        }
        
      
           
        writeback(PC-1);
        

    }
      print_table();
    float branchmisprediction=float (mispredictor)/float(branchesencountered);
    
    float IPC = float (written) / float(Cycles);
    
    cout<<"Branch Misprediction: "<< branchmisprediction<<endl;
    cout<<"IPC: "<<IPC<<endl;
    
    return 0;
}


