#include <iostream>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include<iterator>
#include <queue>
#include <utility>
using namespace std;

struct Passenger{
    int FIRST_ARRIVAL, flight, luggageTime, securityTime, time, stat;
    bool vip, luggage_free;
    int arrival_for_queue;
    int arrival_for_counter;
    int name;
    bool hasLuggage;
    Passenger(int _arrival, int _flight, int _luggageTime, int _securityTime,
              bool _vip, bool _has_luggage) {
        FIRST_ARRIVAL=_arrival;
        arrival_for_queue=_arrival;
        time = _arrival;
        flight=_flight;
        luggageTime=_luggageTime;
        hasLuggage=_has_luggage;
        securityTime=_securityTime;
        vip=_vip;
        luggage_free=!_has_luggage;
        stat = 0;
        arrival_for_counter=_arrival;
    }
    Passenger(int _name, int _arrival, int _flight, int _luggageTime, int _securityTime,
              bool _vip, bool _has_luggage) {
        name=_name;
        hasLuggage=_has_luggage;
        FIRST_ARRIVAL=_arrival;
        arrival_for_queue=_arrival;
        time = _arrival;
        flight=_flight;
        luggageTime=_luggageTime;
        securityTime=_securityTime;
        vip=_vip;
        luggage_free=!_has_luggage;
        stat = 0;
        arrival_for_counter=_arrival;
    }
    bool state(int _stat){
        return (stat == _stat);
    }
};


struct AirPort{

    int TIME = 0;
    int  lug_limit, sec_limit, available_luggage,passenger_num,available_security;
    int waiting_time ;
    int failed_passengers ;
    double avg_waiting;
    bool flight_priority, vip_system, online_system;

    AirPort(int _p, int _l, int _s, bool _flight_priority, bool vip, bool online) {
        // cout<<41<<endl;
        // cout<<_l<<endl;
        waiting_time=0;
        failed_passengers=0;
        lug_limit = _l;
        sec_limit = _s;
        available_luggage = _l;
        available_security = _s;
        passenger_num = _p;
        flight_priority = _flight_priority;
        vip_system=vip;
        online_system=online;

    }


    struct  compareForFlight{
        bool operator() (Passenger &i, Passenger &j){
            if(i.flight==j.flight){
                return i.FIRST_ARRIVAL>j.FIRST_ARRIVAL;
            }
            return i.flight>j.flight;
        }
    };
    struct  compareForTime{
        bool operator() (Passenger &i, Passenger &j){
            if (i.time==j.time) {
                if(i.stat==j.stat) {

                    return i.FIRST_ARRIVAL > j.FIRST_ARRIVAL;

                }
                return i.stat<j.stat;
            }
            return i.time>j.time;
        }
    };

    struct  compareForArrival{
        bool operator() (Passenger &i, Passenger &j){
            if(i.arrival_for_queue==j.arrival_for_queue) return i.FIRST_ARRIVAL>j.FIRST_ARRIVAL;
            return i.arrival_for_queue>j.arrival_for_queue;
        }
    };
    priority_queue<Passenger, vector<Passenger>, compareForTime> eventQ;
    priority_queue<Passenger, vector<Passenger>, compareForArrival> q1 ;
    priority_queue<Passenger, vector<Passenger>, compareForArrival> q2 ;
    priority_queue<Passenger, vector<Passenger>, compareForFlight> fq1 ;
    priority_queue<Passenger, vector<Passenger>, compareForFlight> fq2 ;

    void addPassgener(Passenger  p){

        Passenger pas(p.name, p.FIRST_ARRIVAL, p.flight, p.luggageTime,p.securityTime,p.vip,p.hasLuggage);
        //  cout<<p.name<<" eklendi"<<endl;
        eventQ.push(pas);
    }
    void out(Passenger& p, int& time){
        //   cout<<"out() : "<<p.name<<" time : "<<time<<endl;

        waiting_time = waiting_time+time-p.FIRST_ARRIVAL;

        if(time>p.flight) {
            failed_passengers++;

            //      cout<<"-----------------------------------------\n"<<
//                  "FAILED A GIRDI\n"<<
//                   p.name<<
//                  "time : "<<time<<
//                  "\np.flight : " << p.flight<<
//                  "\n------------------------------------------"<<endl;

        }
        if(p.vip&&vip_system){

            return;
        }
        available_security++;

        //     cout<<"waiting time = "<<waiting_time<<" "<<failed_passengers<<endl;

    }

    void second_q_pop(){

        if (!flight_priority) {

            if(!q2.empty()){
                //cout<<"q2 de bekleyen biri var sira onda"<<endl;
                Passenger next = q2.top();
                q2.pop();
                //   cout<<next.name<<" ikinci siradan cikti sec countera girdi"<<endl;
                next.stat = 4;
                next.arrival_for_counter=TIME;
                next.time = TIME + next.securityTime;
                // cout<<next.time<<". dakikada eventq dan cikacak"<<endl;
                eventQ.push(next);
                available_security--;

            }// else cout<<"q2 de kimse beklemiyor"<<endl;
        }else {
            if (!fq2.empty()) {
                //cout<<"fq2 de bekleyen biri var sira onda"<<endl;
                Passenger next = fq2.top();
                fq2.pop();
                //   cout<<next.name<<" ikinci siradan cikti sec countera girdi"<<endl;
                next.stat = 4;
                next.arrival_for_counter=TIME;
                next.time = TIME + next.securityTime;
                //  cout<<next.time<<". dakikada eventq dan cikacak"<<endl;
                eventQ.push(next);
                available_security--;
            }//else cout<<"fq2 de kimse beklemiyor"<<endl;
        }
    }
    void first_counter_out(Passenger& p, int&time){
        //  cout<<"first_counter_out() : "<<p.name<<" time : "<<time<<endl;
        if (p.vip && vip_system) {
            //  cout<<p.name<<" vip oldugu icin sec e girmeyecek\nout() cagirildi"<<endl;
            out(p, TIME);

        }
        else if(available_security>0){


            p.arrival_for_counter=time;
            p.time=time+p.securityTime;
            p.stat=4;
            //    cout<<p.name<<" eventQ dan "<<p.time<<". dakikada cikacak"<<endl;
            //   cout<<"available_security : "<<available_security;
            available_security--;
            //   cout<<" idi "<< available_security<<" oldu"<<endl;
            eventQ.push(p);
        }else{
            p.arrival_for_queue=time;
            p.stat=3;
            //   cout<<p.name<<" ikinci qya girdi"<<endl;
            if(!flight_priority) q2.push(p);
            else fq2.push(p);
        }
        if(p.luggage_free&&online_system){
            return;
        }
        available_luggage++;
        // cout<<"available luggage : "<<available_luggage<<endl;
    }
    void first_q_pop(){

        if (!flight_priority) {

            if(!q1.empty()){
                //       cout<<"q1 de bekleyen biri var sira onda"<<endl;
                Passenger next = q1.top();
                q1.pop();
                //     cout<<next.name<<" birinci siradan cikti lug countera girdi"<<endl;
                next.stat = 2;
                next.time = TIME + next.luggageTime;
                //      cout<<next.time<<". dakikada eventq dan cikacak"<<endl;
                eventQ.push(next);
                available_luggage--;

            }// else cout<<"q1 de kimse beklemiyor"<<endl;
        }else {
            if (!fq1.empty()) {
                //    cout<<"fq1 de biri var sira onda"<<endl;
                Passenger next = fq1.top();
                fq1.pop();
                //     cout << next.name << " birinci siradan cikti lug countera girdi" << endl;
                next.stat = 2;
                next.time = TIME + next.luggageTime;
                //     cout << next.time << ". dakikada eventq dan cikacak" << endl;
                eventQ.push(next);
                available_luggage--;
            }//else cout<<"fq1 de kimse beklemiyor"<<endl;
        }
    }

    void start(){
        // cout<<"start()..."<<endl;

        while (!eventQ.empty()){

            Passenger cur = eventQ.top();
            eventQ.pop();
            //    cout<<"\n\n"<<cur.name<<" while dan poplandi "<<cur.stat<<". state te"<<endl;
            if(cur.time>TIME) TIME = cur.time;
            //    cout<<"TIME : "<<TIME<<endl;
            if(cur.state(4)) {

                out(cur,TIME);
                second_q_pop();

            }// luggage counter ý terk ediyor
            else if(cur.state(2)) {


                first_counter_out(cur,TIME);
                first_q_pop();

            }
            else if(cur.state(0)){

                //          cout<<"cur state 0"<<endl;
                //online bilet iŷlemi
                if(cur.luggage_free && online_system) {
                    //           cout<<"luggage free ve online system"<<endl;
                    // online sistem varsa ve vip sistem varsa yolcu bunlarý atlayabilir
                    if (cur.vip && vip_system) {
                        out(cur, TIME);

                    }
                        // online sistem var, vip yok
                    else {
                        // security counter müsaitse
                        if(available_security>0){
                            cur.stat=4;
                            cur.time=TIME+cur.securityTime;
                            available_security--;
                            eventQ.push(cur);
                        }
                            //security q ya girecek
                        else {
                            cur.stat = 3;
                            cur.arrival_for_queue = TIME;
                            if (!flight_priority) {
                                q2.push(cur);
                            } else {
                                fq2.push(cur);
                            }
                        }
                    }
                }
                    // online týcket yok
                else{
                    //          cout<<"online ticket yok"<<endl;
                    //          cout<<"available lug : "<<available_luggage<<endl;
                    // lug counter boŷsa
                    if(available_luggage){
                        //             cout<<"lug counter available"<<endl;
                        cur.stat=2;
                        available_luggage--;
                        cur.time = TIME+ cur.luggageTime;
                        //              cout<<"eventQ ya pushlandi"<<endl;
                        //             cout<<cur.time<<". dakikada cikacak"<<endl;
                        eventQ.push(cur);
                    }
                        // lug counter doluysa luggage q ya girecek
                    else{
                        //             cout<<"lug q ya giriyor"<<endl;
                        cur.stat=1;
                        if(!flight_priority) q1.push(cur);
                        else fq1.push(cur);
                    }
                }
            }
            else {
                //     cout<<"out of cases 0-4"<<endl;
                return;
            }


        }

        avg_waiting = (double)waiting_time/passenger_num;

    }



};
template <class Container>


void split1(const string& str, Container& cont){
    istringstream iss(str);
    copy(istream_iterator<string>(iss),istream_iterator<string>(),back_inserter(cont));
}

int main(int argc, char* argv[]) {
    // below reads the input file
    // in your next projects, you will implement that part as well
    if (argc != 3) {
        cout << "Run the code with the following command: ./project1 [input_file] [output_file]" << endl;
        return 1;
    }
    int p;
    int l;
    int s;

    // cout << "input file: " << argv[1] << endl;
    //  cout << "output file: " << argv[2] << endl;


    // here, perform the input operation. in other words,
    // read the file named <argv[1]>
    ifstream infile(argv[1]);
    string line;

    // process first line
    getline(infile, line);

    vector<string> words;
    split1(line, words);
    p = stoi(words[0]);
    l = stoi(words[1]);
    s = stoi(words[2]);

    AirPort sim1 = AirPort(p,l,s,  false,  false,  false); // 000
    AirPort sim2 = AirPort(p,l,s,  true,   false,  false); // 100
    AirPort sim3 = AirPort(p,l,s,  false,  true,   false); // 010
    AirPort sim4 = AirPort(p,l,s,  true,   true,   false); // 110
    AirPort sim5 = AirPort(p,l,s,  false,  false,  true ); // 001
    AirPort sim6 = AirPort(p,l,s,  true,   false,  true ); // 101
    AirPort sim7 = AirPort(p,l,s,  false,  true,   true ); // 011
    AirPort sim8 = AirPort(p,l,s,  true,   true,   true ); // 111

    for (int i = 0; i < p; i++) {
        getline(infile, line);
        //cout << "line: " << line ;
        vector<string> words;
        split1(line, words);

        int arrivalTime = stoi(words[0]);
        int departureTime = stoi(words[1]);
        int luggageTime = stoi(words[2]);
        int securityTime = stoi(words[3]);
        bool isVip = (words[4] == "V");
        bool hasLuggage = (words[5] == "L");


        Passenger p1(i, arrivalTime, departureTime, luggageTime, securityTime, isVip, hasLuggage);
        /*   Passenger p2(i, arrivalTime, departureTime, luggageTime, securityTime, isVip, hasLuggage);
           Passenger p3(i, arrivalTime, departureTime, luggageTime, securityTime, isVip, hasLuggage);
           Passenger p4(i, arrivalTime, departureTime, luggageTime, securityTime, isVip, hasLuggage);
           Passenger p5(i, arrivalTime, departureTime, luggageTime, securityTime, isVip, hasLuggage);
           Passenger p6(i, arrivalTime, departureTime, luggageTime, securityTime, isVip, hasLuggage);
           Passenger p7(i, arrivalTime, departureTime, luggageTime, securityTime, isVip, hasLuggage);
           Passenger p8(i, arrivalTime, departureTime, luggageTime, securityTime, isVip, hasLuggage);
   */
        //cout << pass.vip << pass.luggagePass << endl;
        sim1.addPassgener(p1);
        sim2.addPassgener(p1);
        sim3.addPassgener(p1);
        sim4.addPassgener(p1);
        sim5.addPassgener(p1);
        sim6.addPassgener(p1);
        sim7.addPassgener(p1);
        sim8.addPassgener(p1);



    }

    sim1.start();
    sim2.start();
    sim3.start();
    sim4.start();
    sim5.start();
    sim6.start();
    sim7.start();
    sim8.start();

    ofstream myfile;
    myfile.open(argv[2]);
    myfile<< sim1.avg_waiting <<" " << sim1.failed_passengers<<endl;
    myfile<< sim2.avg_waiting <<" " << sim2.failed_passengers<<endl;
    myfile<< sim3.avg_waiting <<" " << sim3.failed_passengers<<endl;
    myfile<< sim4.avg_waiting <<" " << sim4.failed_passengers<<endl;
    myfile<< sim5.avg_waiting <<" " << sim5.failed_passengers<<endl;
    myfile<< sim6.avg_waiting <<" " << sim6.failed_passengers<<endl;
    myfile<< sim7.avg_waiting <<" " << sim7.failed_passengers<<endl;
    myfile<< sim8.avg_waiting <<" " << sim8.failed_passengers<<endl;
    myfile.close();




    return 0;
}