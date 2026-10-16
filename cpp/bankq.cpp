/* ==============================================================
   BANK QUEUE SIMULATION -- single server, FIFO discipline
   Inter-arrival ~ U(a,b) minutes , Service ~ U(c,d) minutes
   Compile:  g++ -O2 -std=c++17 -o bankq bankq.cpp
   ============================================================== */
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
using namespace std;

class Rng {
    long long x;
public:
    explicit Rng(long long seed = 123) { x = ((seed % 1000) + 1000) % 1000; }
    double next() { x = (221 * x + 71) % 1000; return (double)x / 1000.0; }
};

struct Row { int id; double iat, at, st, sbt, wq, dep, tis, idle; };

int main() {
    int n; long long seed;
    double iaLo, iaHi, svLo, svHi;

    cout << "=============================================================\n"
         << "             BANK QUEUE SIMULATION - INPUT SCREEN\n"
         << "=============================================================\n";
    cout << "Number of customers to simulate (e.g. 100)    : "; cin >> n;
    cout << "Random number seed             (e.g. 123)    : "; cin >> seed;
    cout << "Inter-arrival lower bound, minutes (e.g. 1)  : "; cin >> iaLo;
    cout << "Inter-arrival upper bound, minutes (e.g. 5)  : "; cin >> iaHi;
    cout << "Service time  lower bound, minutes (e.g. 1)  : "; cin >> svLo;
    cout << "Service time  upper bound, minutes (e.g. 6)  : "; cin >> svHi;

    if (n    <= 0)    n    = 100;
    if (iaHi <= iaLo) iaHi = iaLo + 1;
    if (svHi <= svLo) svHi = svLo + 1;

    Rng rng(seed);
    vector<Row> rows(n);
    vector<double> deps;

    double prevArrival = 0.0, prevDeparture = 0.0;
    double sumIat=0, sumSt=0, sumWq=0, sumTis=0, idleTotal=0;
    double maxWq=0, maxTis=0;
    int    waited=0, maxQ=0;

    for (int i = 0; i < n; ++i) {
        double r1 = rng.next();
        double r2 = rng.next();

        Row &r = rows[i];
        r.id   = i + 1;
        r.iat  = iaLo + (iaHi - iaLo) * r1;
        r.st   = svLo + (svHi - svLo) * r2;

        r.at   = prevArrival + r.iat;
        r.idle = max(0.0, r.at - prevDeparture);
        r.sbt  = max(r.at, prevDeparture);
        r.wq   = r.sbt - r.at;
        r.dep  = r.sbt + r.st;
        r.tis  = r.dep - r.at;

        int q = 0;
        for (double d : deps) if (d > r.at) ++q;
        maxQ = max(maxQ, q);
        deps.push_back(r.dep);

        if (r.wq > 0) ++waited;
        maxWq  = max(maxWq,  r.wq);
        maxTis = max(maxTis, r.tis);
        sumIat += r.iat; sumSt += r.st; sumWq += r.wq;
        sumTis += r.tis; idleTotal += r.idle;

        prevArrival = r.at; prevDeparture = r.dep;
    }

    double totalTime = prevDeparture;
    double busyTime  = sumSt;
    double util      = busyTime / totalTime;
    double avgIat = sumIat/n, avgSt = sumSt/n;
    double avgWq  = sumWq/n,  avgTis = sumTis/n;
    double lq = sumWq/totalTime;
    double ls = sumTis/totalTime;

    cout << "\n\n=============================================================\n"
         << "                    SIMULATION TABLE\n"
         << "=============================================================\n";
    cout << " C#      IAT      AT      ST     SBT      WQ     DEP     TIS    IDLE\n";
    cout << "---------------------------------------------------------------------\n";
    cout << fixed << setprecision(3);
    for (const Row &r : rows)
        cout << setw(4) << r.id << setw(9) << r.iat << setw(8) << r.at
             << setw(8) << r.st << setw(8) << r.sbt << setw(8) << r.wq
             << setw(8) << r.dep << setw(8) << r.tis << setw(8) << r.idle << "\n";

    cout << "\n\n=============================================================\n"
         << "                    QUEUE STATISTICS\n"
         << "=============================================================\n";
    cout << "Customers simulated ................ : " << n          << "\n";
    cout << "Total simulated time (min) ......... : " << totalTime  << "\n";
    cout << "Average inter-arrival time (min) ... : " << avgIat     << "\n";
    cout << "Average service time (min) ......... : " << avgSt      << "\n";
    cout << "Average waiting time in queue (min)  : " << avgWq      << "\n";
    cout << "Average time in system (min) ....... : " << avgTis     << "\n";
    cout << "Maximum waiting time (min) ......... : " << maxWq      << "\n";
    cout << "Maximum time in system (min) ....... : " << maxTis     << "\n";
    cout << "Maximum queue length (customers) ... : " << maxQ       << "\n";
    cout << "Average queue length (customers) ... : " << lq         << "\n";
    cout << "Average number in system ........... : " << ls         << "\n";
    cout << "Customers who had to wait .......... : " << waited
         << "  (" << setprecision(2) << 100.0*waited/n << " %)\n";
    cout << setprecision(3);
    cout << "Customers served without waiting ... : " << n - waited << "\n";
    cout << "Server busy time (min) ............. : " << busyTime   << "\n";
    cout << "Server idle time (min) ............. : " << idleTotal  << "\n";
    cout << "Server utilisation (%) ............. : " << 100.0*util << "\n";
    cout << "Probability server is idle ......... : " << 1.0 - util << "\n";
    cout << "Traffic intensity rho (measured) ... : " << avgSt/avgIat << "\n";
    cout << "Theoretical E[IAT], E[ST], rho ..... : "
         << (iaLo+iaHi)/2 << ", " << (svLo+svHi)/2 << ", "
         << (svLo+svHi)/(iaLo+iaHi) << "\n";
    cout << "\nLittle's Law check  Lq = lambda*Wq : "
         << lq << "  ~=  " << (double)n/totalTime * avgWq << "\n";
    if (util >= 1.0)
        cout << "\n*** WARNING: rho >= 1 -- queue is unstable, it grows with time.\n";
    return 0;
}
