#include <iostream>
#include <conio.h>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <Windows.h>
#include <iomanip>

using namespace std;


int dl = 0;
char key;
int max_quntum = 0;
int curr_quntum = 1;
int proccess_counter = 0;
int burstTime_counter = 0;
int cpu_counter = 0;
int curr_time = 0;
int *temp_procces;
int temp_procces_counter = 0;
int curr_state = -1;
string data;
int front = 0;
int rear = 0;
int criterion_counter = 0;
int criterion_key = -1;
HANDLE Color = GetStdHandle(STD_OUTPUT_HANDLE);
int colors[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};



class queue
{
private:
    int *mqueue;
    int maxsize;


public:
    queue(int n)
    {
        mqueue = new int[n];
        maxsize = n;
        // front = 0;
        // rear = -1;
    }
    void add(int x);
    int del();
    bool IsEmpty();
    bool IsFull();
    void shift();
    void Display();
};

void queue::Display()
{
    for (int i = front; i < rear; i++)
    {
        cout << mqueue[i] << " ";
    }
    cout << endl;
}

bool queue::IsFull()
{
    if (rear == maxsize - 1)
        return true;
    return false;
}

bool queue::IsEmpty()
{
    if (front == rear)
        return true;
    return false;
}

void queue::add(int x)
{
    if (IsFull())
    {
        cout << "queue is full!" << endl;
        return;
    }
    mqueue[rear] = x;
    rear++;
}

int queue::del()
{
    if (IsEmpty())
    {
        cout << "queue is empty";
        return -1;
    }
    int x;
    x = mqueue[front];
    front++;
    return x;
}


class Criterion
{
public:    
    float CpuUtilization;
    float Throughput;
    float ATT;
    float ART;
    float AWT;
    int counter;

    Criterion()
    {
        CpuUtilization = 0.0;
        Throughput = 0.0;
        ATT = 0.0;
        ART = 0.0;
        AWT = 0.0;
    }
};

class Procces
{

public:
    int arrival_time;
    int burts_number;
    int *burst_time;
    int io_time;
    bool *burst_position;
    bool io_flag;
    bool busy;
    bool firstIOFlag;
    int firstIO;
    int state;              //* 1:new   2:wating    3:running   4:terminate

    Procces()
    {}
    Procces(int n)
    {
        burst_time = new int[n];
        burst_position = new bool[n];
        for (int i = 0; i < n; i++)
        {
            burst_position[i] = true;
        }
        state = 1;
        busy = false;
        io_time = 0;
        io_flag = false;
        firstIOFlag = true;
        firstIO = 0;
    }
};

class Cpu
{
public: 
    int start;
    int end;
    int procces;
    int counter;

    Cpu()
    {
        counter = 0;
    }

};

class TempCpu
{
public: 
    int start;
    int end;
    int procces;
    int counter;
    int quantim;

    TempCpu()
    {
        counter = 0;
        quantim = 0;
    }
};

void UtilizationCpu(Cpu cpu[], Criterion criterion[])
{
    float cpu_utilizatoin;
    float final_time = cpu[cpu_counter-1].end;
    int dispatcherLetency = ((cpu_counter-1)*dl);
    float idel = 0;
    for (int i = 1; i <= cpu_counter-1; i++)
    {
        if (cpu[i].start - cpu[i-1].end != 0)
        {
            idel += cpu[i].start - cpu[i-1].end;
        }
    }

    cpu_utilizatoin = (final_time - (idel + dispatcherLetency)) / final_time * 100;
    criterion[criterion_counter].CpuUtilization = cpu_utilizatoin;
    SetConsoleTextAttribute(Color , colors[1]);
    cout<<"cpu utilization : "<<criterion[criterion_counter].CpuUtilization<<endl;
    SetConsoleTextAttribute(Color , colors[6]);
}

void Throughput(Cpu cpu[], Criterion criterion[])
{
    float total_time = cpu[cpu_counter-1].end;
    float throughput = (proccess_counter / total_time);
    criterion[criterion_counter].Throughput = throughput;
    SetConsoleTextAttribute(Color , colors[1]);
    cout<<"Troughput : "<<criterion[criterion_counter].Throughput<<endl;
    SetConsoleTextAttribute(Color , colors[6]);
}

void TurnaroundTime(Cpu cpu[], Criterion criterion[], Procces process[])
{
    float total_turnaroundTime = 0.0;
    float Average = 0.0;
    for (int i = 0; i < proccess_counter; i++)
    {
        int terminate_time = 0;
        for (int j = 0; j <= cpu_counter-1; j++)
        {
            if (cpu[j].procces == i)
                terminate_time = cpu[j].end;
        }
        total_turnaroundTime += (terminate_time - process[i].arrival_time);
    }
    Average = (total_turnaroundTime / proccess_counter);
    criterion[criterion_counter].ATT = Average;
    SetConsoleTextAttribute(Color , colors[1]);
    cout<<"TurnAroundTime : "<<criterion[criterion_counter].ATT<<endl;
    SetConsoleTextAttribute(Color , colors[6]);
}

void ResponseTime(Cpu cpu[], Criterion criterion[], Procces process[])
{
    float total_responseTime = 0.0;
    float Average = 0.0;
    for (int i = 0; i < proccess_counter; i++)
    {
        int response_time = 0;

        if(process[i].burts_number == 1)
        {
            for (int j = 0; j < cpu_counter; j++)
            {
                if(cpu[j].procces == i)
                    response_time = cpu[j].end;
            }
            
        }
        else
        {
            response_time = process[i].firstIO;
        }

        total_responseTime += (response_time - process[i].arrival_time);
    }
    Average = (total_responseTime / proccess_counter);
    criterion[criterion_counter].ART = Average;
    SetConsoleTextAttribute(Color , colors[1]);
    cout<<"Response : "<<criterion[criterion_counter].ART<<endl;
    SetConsoleTextAttribute(Color , colors[6]);
    
}

void WatingTime(Cpu cpu[], Criterion criterion[], Procces process[])
{
    float Average = 0.0;
    for (int i = 0; i < proccess_counter; i++)
    {
        int timeInCpu = 0;
        int lastTime = 0;
        float total_time = 0.0;
        int ioTime = 0;
        for (int j = 0; j < cpu_counter; j++)
        {
           if(cpu[j].procces == i)
           {
               timeInCpu += (cpu[j].end - cpu[j].start);
               lastTime = cpu[j].end;
           }
        }
        for (int k = 0; k < process[i].burts_number; k++)
        {
            if((k + 1) %2 == 0)
                ioTime += process[i].burst_time[k];
        }
        
        total_time =  lastTime - process[i].arrival_time;
        total_time -= timeInCpu;
        total_time -= ioTime;
        Average += total_time;
    }
    Average /= proccess_counter;
    criterion[criterion_counter].AWT = Average;
    SetConsoleTextAttribute(Color , colors[1]);
    cout<<"AWT : "<<criterion[criterion_counter].AWT<<endl;
    SetConsoleTextAttribute(Color , colors[6]);
}

void copyCpuValue(Cpu cpu[], TempCpu Tcpu[])
{
    for (int i = 0; i <= cpu_counter-1; i++)
    {
        Tcpu[i].procces = cpu[i].procces;
        Tcpu[i].start = cpu[i].start;
        Tcpu[i].end = cpu[i].end;
        Tcpu[i].counter = cpu_counter;
        Tcpu[i].quantim = curr_quntum;
    }
}

void setProcessValue(string line, int num_burst, int proc_index, Procces procces[])  
{
    string temp = "";
    bool isArrival = true;          //* Recognize arrival time
    int burstTime_index = 0;           
    int InputArrival;
    int InputBurstTime;

    procces[proc_index] = Procces(num_burst);            
    procces[proc_index].burts_number = num_burst;        

    for (int i = 0; i < line.size(); i++)
    {
        if(line[i] == ',')
        {
            if(isArrival)
            {
                stringstream ss(temp);
                ss >> InputArrival;          
                procces[proc_index].arrival_time = InputArrival;      //? SET ARRIVAL TIME
                temp="";
                isArrival = false;
            }
            else
            {
                stringstream ss(temp);
                ss >> InputBurstTime;
                procces[proc_index].burst_time[burstTime_index] = InputBurstTime;   //? SET BURST TIME (cpu or i/o)
                temp = "";
                burstTime_index++;
            }
            continue;
        }
        temp.push_back(line[i]);
    }
}

void split(Procces process[])
{
    string temp;
    int Proc_counter = 0;

    for (int i = 0; i < data.size(); i++)       //* split data
    {
        temp = "";          //* ex: 0,3,2,2,
        if(data[i] == ':')
        {
            for (int j = i+1; j < data.size() ; j++)
            {
                if(data[j] == '|')
                {
                    setProcessValue(temp, burstTime_counter-1, Proc_counter, process);
                    Proc_counter++;
                    burstTime_counter = 0;
                    break;
                }
                if(data[j] == ',')
                    burstTime_counter++;
                temp.push_back(data[j]);
            }
            
        }
    }
}


bool checkEndProcess(Procces processs[])
{
    int c = 0;
    for (int i = 0; i < proccess_counter; i++)
    {
        if (processs[i].state == 4)
            c++;
    }

    if(c == proccess_counter)
        return true;
    return false;    
    
}

int findMaximumArrivalTime(Procces procces[])
{
    int max = procces[0].arrival_time;
    for (int i = 1; i < proccess_counter; i++)
    {
       if(procces[i].arrival_time > max)
            max = procces[i].arrival_time;
    }
    return max;
}

void findMinimumArrivalTime(Procces procces[])
{
    int min = procces[0].arrival_time;
    for (int i = 1; i < proccess_counter; i++)
    {
       if(procces[i].arrival_time < min)
            min = procces[i].arrival_time;
    }

    if(min > 0)
    {
        for (int i = 0; i < proccess_counter; i++)
        {
            procces[i].arrival_time -= min;
        }
        
    }
}

void checkBigestCpuBurst(Procces procces[])
{
    int max = 0;
    for (int i = 0; i < proccess_counter; i++)
    {
        for (int j = 0; j < procces[i].burts_number; j++)
        {
            if((j+1) % 2 == 1)            
                if(procces[i].burst_time[j] > max)
                    max = procces[i].burst_time[j];
        }
        
    }
    max_quntum = max;
    
}

int checkState(int p)
{
    if (p == 1)
        curr_state = 1;
    if (p == 2)
        curr_state = 2;
    if (p == 3)
        curr_state = 3;
    if (p == 4)
        curr_state = 4;            
}

void swap(int *xp, int *yp)  
{  
    int temp = *xp;  
    *xp = *yp;  
    *yp = temp;  
} 

void sortTempProcess(Procces process[])
{
    int i, j;  
    for (i = 0; i < temp_procces_counter-1; i++)      
    {
        for (j = 0; j < temp_procces_counter-i-1; j++)  
        {
            
            if (process[temp_procces[j]].state > process[temp_procces[j+1]].state)  
                swap(&temp_procces[j], &temp_procces[j+1]);  
        }
            
    }   
   
}

void cpuScheduling(Procces procces[],Cpu cpu[],queue ready)
{
    int p = ready.del();
    procces[p].busy = true;  
    int position;

    for (int i = 0; i < procces[p].burts_number; i++)       //* check this burst time Already finished or not
    {
        if (procces[p].burst_position[i])
        {
            position = i;
            break;
        }
    }

    if (position < 0)
        return; 
    int nextCpuBurst = procces[p].burst_time[position];
   


    if(cpu_counter == 0)
    {
        cpu[cpu_counter].start = 0;
        if(nextCpuBurst <= curr_quntum)     //* set end time 
            cpu[cpu_counter].end = cpu[cpu_counter].start + nextCpuBurst;
        else
            cpu[cpu_counter].end = cpu[cpu_counter].start + curr_quntum ; 
    }
    else
    {
        cpu[cpu_counter].start = curr_time;
        if(nextCpuBurst <= curr_quntum)     //* set end time
            cpu[cpu_counter].end = cpu[cpu_counter].start + nextCpuBurst +dl ;
        else
            cpu[cpu_counter].end = cpu[cpu_counter].start + curr_quntum +dl; 
    }

   

    cpu[cpu_counter].procces = p;       //* set proccess

    if(nextCpuBurst - curr_quntum > 0) //* still have cpu burst on this possition
    {
        procces[p].burst_time[position] = nextCpuBurst - curr_quntum;
        procces[p].state = 3; //? running
        procces[p].burst_position[position] = true;
    }
    else if(nextCpuBurst - curr_quntum <= 0) //* if cpu burst done  We are faced with two options ==>(1.go to i/o burst    2.go to terminate)
        {
            procces[p].burst_time[position] = 0;
            procces[p].burst_position[position] = false;
            if(procces[p].burts_number == 1)                   
                procces[p].state = 4; //? terminate
            else if(procces[p].burts_number - (position + 1) > 0)
            {
                procces[p].state = 2; //? I/O
                procces[p].io_flag = true;
                procces[p].burst_position[position+1] = false;
            }
            else
            {
                procces[p].state = 4; //? terminate
                procces[p].burst_position[position] = false;
            }
        }
       
     //* set new time
     if(cpu_counter == 0)
     {
         int temp_time = cpu[cpu_counter].end;
         for (int i = 1; i <= temp_time; i++)
         {
             curr_time+=1;
             if (curr_time == cpu[cpu_counter].end)
             {
                  procces[p].busy = false;
                 if(procces[p].io_flag)
                {
                    procces[p].io_time = curr_time + procces[p].burst_time[position+1];
                    procces[p].io_flag = false;
                    if(procces[p].firstIOFlag)
                    {
                        procces[p].firstIOFlag = false;
                        procces[p].firstIO = curr_time;
                    }
                }
             }
               

            for (int j = 0; j < proccess_counter; j++)
            {
               checkState(procces[j].state);
            
                if((curr_state == 1 || curr_state == 2 || curr_state == 3)  && procces[j].busy == false )      
                {
                        
                    if(procces[j].arrival_time <= curr_time && procces[j].io_time <= curr_time)
                    {
                            temp_procces[temp_procces_counter] = j;
                            temp_procces_counter++;
                            procces[j].busy = true;
                    }
                } 
            }
         }
     }    

     else if(cpu_counter > 0)
     {
         int temp_time = cpu[cpu_counter].end - cpu[cpu_counter].start ;        
         for (int i = 1; i <= temp_time; i++)
         {
             curr_time+=1;
             if (curr_time == cpu[cpu_counter].end)
             {
                procces[p].busy = false;
                if(procces[p].io_flag)
                {
                    procces[p].io_time = curr_time + procces[p].burst_time[position+1];
                    procces[p].io_flag = false;
                    if(procces[p].firstIOFlag)
                    {
                        procces[p].firstIOFlag = false;
                        procces[p].firstIO = curr_time;
                    }
                }
             }
            for (int j = 0; j < proccess_counter; j++)
            {
                checkState(procces[j].state);
                if((curr_state == 1 || curr_state == 2 || curr_state == 3) 
                         && procces[j].busy == false )        //* running or new
                    {
                        if(procces[j].arrival_time <= curr_time && procces[j].io_time <= curr_time)
                        {
                            temp_procces[temp_procces_counter] = j;
                            temp_procces_counter++;
                            procces[j].busy = true;
                        }
                    }
            }
         }
     }

     cpu_counter++;
}

void PriorityReadyQueue(queue ready,Procces process[])
{
    if (temp_procces_counter == 1)          //* for ex: in time 0 just one Process comming to readyQueue
        ready.add(temp_procces[temp_procces_counter-1]);

    if (temp_procces_counter > 1)          //* for ex: in time 0 two or more Process comming to readyQueue
    {
        sortTempProcess(process);
       
        for (int i = 0; i < temp_procces_counter; i++)
        {
            ready.add(temp_procces[i]);
            process[temp_procces[i]].busy = true;
        }  
    }
}

void clearTempProcces()
{
    for (int i = 0; i < temp_procces_counter; i++)
    {
        temp_procces[i] = -1;
    }
    temp_procces_counter = 0;
}


void manageReadyQueue(Procces procces[], Cpu cpu[], TempCpu Tcpu[], Criterion criterion[])
{
    findMinimumArrivalTime(procces);        //* ex: if min Arrival Time = 3 -----> all ArrivalTime - 3 
    checkBigestCpuBurst(procces);       
    int max = findMaximumArrivalTime(procces);  
   
   
    
    queue ready(INT_MAX);
   q:for (curr_quntum; curr_quntum <= max_quntum;)
    {
        while (true)  
        {
            for (int p = 0; p < proccess_counter; p++)
            {
                checkState(procces[p].state);
                if((curr_state == 1 || curr_state == 2 || curr_state == 3) 
                    && procces[p].busy == false)      
                {
                    if(procces[p].arrival_time <= curr_time && procces[p].io_time <= curr_time)
                    {
                        temp_procces[temp_procces_counter] = p;
                        temp_procces_counter++;
                    }
                }
            }
            if (checkEndProcess(procces))
            {
                SetConsoleTextAttribute(Color , colors[5]);
                cout<<"\nQUANTUM : "<<curr_quntum<<endl;
                SetConsoleTextAttribute(Color , colors[6]);
                UtilizationCpu(cpu,criterion);
                Throughput(cpu,criterion);
                TurnaroundTime(cpu,criterion,procces);
                ResponseTime(cpu,criterion,procces);
                WatingTime(cpu,criterion,procces);

                if (criterion_key == -1)
                {
                    criterion_key = 0;
                    copyCpuValue(cpu,Tcpu);
                }
                else
                {
                    if (key == 'W')
                    {
                        if (criterion[criterion_key].AWT > criterion[criterion_counter].AWT)
                        {
                            
                            criterion_key = criterion_counter;
                            copyCpuValue(cpu,Tcpu);
                        }
                    }
                    else if(key == 'R')
                    {
                        if (criterion[criterion_key].ART > criterion[criterion_counter].ART)
                        {
                            criterion_key = criterion_counter;
                            copyCpuValue(cpu,Tcpu);
                        }
                    }
                    else if(key == 'T')
                    {
                        if (criterion[criterion_key].ATT > criterion[criterion_counter].ATT)
                        {
                            criterion_key = criterion_counter;
                            copyCpuValue(cpu,Tcpu);
                        }
                    }
                }
                
                clearTempProcces();
                curr_time = 0;
                cpu_counter = 0;
                curr_quntum++;
                front = rear;
                criterion_counter++;
                split(procces);
                goto q;
                // return;
            }    
            if (temp_procces_counter == 0 && ready.IsEmpty())
                curr_time++;
            if (temp_procces_counter != 0 || !ready.IsEmpty())   
            {
                PriorityReadyQueue(ready,procces);
                clearTempProcces();
                cpuScheduling(procces,cpu,ready);
            } 
        }
    }
    
}


void GunChart(Procces process[], TempCpu Tcpu[])
{
    ofstream save_info;
    save_info.open("output.txt",ios::app);

    int x = Tcpu[0].counter;
    x*=8;

    
    int last_position;
    int pos[proccess_counter-1];
    for (int i = 0; i < proccess_counter; i++)
    {
        for (int j = 0; j <= Tcpu[0].counter-1; j++)
        {
            if(Tcpu[j].procces == i)
                last_position = j;
        }
        pos[i] = last_position;
    }
    
    
    cout<<endl;
    save_info<<"\t"<<setfill('-')<<setw(x)<<"-"<<endl;
    cout<<"\t"<<setfill('-')<<setw(x)<<"-"<<endl;

    cout<<"\t|";
    save_info<<"\t|";
    int pos_counter = 0;
    bool flag = false;
   
    for (int i = 0; i <=Tcpu[0].counter-1; i++)
    {
        Sleep(50);
       for (int j = 0; j < proccess_counter; j++)
       {
           if (i == pos[j])
           {
                cout<<"  P"<<Tcpu[i].procces<<"(T)|";
                save_info<<"  P"<<Tcpu[i].procces<<"(T)|";
                flag = true;
           }
       }
         if (!flag)
         {
            cout<<"  P"<<Tcpu[i].procces<<"   |";
            save_info<<"  P"<<Tcpu[i].procces<<"   |";
         }
         flag = false;

    }
    cout<<endl;
    save_info<<endl;

    cout<<"\t"<<setfill('-')<<setw(x)<<"-"<<endl;
    save_info<<"\t"<<setfill('-')<<setw(x)<<"-"<<endl;

    cout<<"\t";
    save_info<<"\t";

    int tmp;
    int c=0;
    int number;
    for (int i = 0; i <=Tcpu[0].counter-1; i++)
    {
        Sleep(50);
        int tmp;
        number = Tcpu[i].start;
        tmp = number;
        while(tmp!=0)
        {
            tmp/=10;
            c++;
        }
        if(c > 2)
        {
            cout<<number<<"      ";
            save_info<<number<<"      ";
        }
           
        else
        {
            cout<<number<<"       ";    
            save_info<<number<<"       ";   
        }
    }
    cout<<Tcpu[Tcpu[0].counter-1].end;
    save_info<<Tcpu[Tcpu[0].counter-1].end;

    save_info.close();
    
}


int main()
{
    getch();

    ifstream input("input.txt");
    string line;
   
    int line_count = 0;            //* line control

    while (getline(input,line))
    {
        if(line_count == 0)
        {
            stringstream n(line);
            n >> dl;                //? SET DISPATCHER
            line_count++;
        }
        else if(line_count == 1)
        {
            key = line[0];            //? SET Criterion (W/T/R...)
            line_count++;
        }
        else if(line_count > 1)
        {
            data.append(line);
            data.append(",|");              //* split data with <|> ex: data = P1:x,y,z,|P2:x,y,z,|
            proccess_counter++;
        }
    }
    input.close();

    Procces procces[proccess_counter];
    Cpu cpu[8192];
    TempCpu Tcpu[8192];
    Criterion criterion[8192];

    string temp;
    int Proc_counter = 0;

    for (int i = 0; i < data.size(); i++)       //* split یشفش
    {
        temp = "";          //* ex: 0,3,2,2,
        if(data[i] == ':')
        {
            for (int j = i+1; j < data.size() ; j++)
            {
                if(data[j] == '|')
                {
                    if(burstTime_counter % 2 == 1)
                        return 0;
                    setProcessValue(temp, burstTime_counter-1, Proc_counter, procces);
                    Proc_counter++;
                    burstTime_counter = 0;
                    break;
                }
                if(data[j] == ',')
                    burstTime_counter++;
                temp.push_back(data[j]);
            
            }
            
        }
    }


    temp_procces = new int[proccess_counter];
    manageReadyQueue(procces, cpu, Tcpu, criterion);

    

    ofstream save_info("output.txt");

    SetConsoleTextAttribute(Color , colors[2]);
    cout<<"\nQUANTUM : "<<Tcpu[0].quantim<<endl;
    save_info<<"\nOPTIMAL QUANTUM : "<<Tcpu[0].quantim<<"\n"<<endl;

    for (int i = 0; i < Tcpu[0].counter; i++)
    {
        cout<<"cpu["<<i<<"] = P"<<Tcpu[i].procces<<" , start : "<<Tcpu[i].start<<" , end : "<<Tcpu[i].end<<endl;
    }
    SetConsoleTextAttribute(Color , colors[6]);
    cout<<"\n\n";
    
    save_info<<"Cpu Utilization : "<<criterion[criterion_key].CpuUtilization<<endl;
    save_info<<"Throughput      : "<<criterion[criterion_key].Throughput<<endl;
    save_info<<"TurnAroundTime  : "<<criterion[criterion_key].ATT<<endl;
    save_info<<"Response Time   : "<<criterion[criterion_key].ART<<endl;
    save_info<<"Wating Time     : "<<criterion[criterion_key].AWT<<endl;
    save_info<<"\n\n";
    save_info.close();

    
    GunChart(procces,Tcpu);

    getch();
}
