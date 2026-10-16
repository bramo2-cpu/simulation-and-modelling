import java.util.*;

public class BankQueue {

    static class Rng {
        private long x;
        Rng(long seed) { x = ((seed % 1000) + 1000) % 1000; }
        double next()  { x = (221 * x + 71) % 1000; return x / 1000.0; }
    }

    static class Row { int id; double iat, at, st, sbt, wq, dep, tis, idle; }

    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);

        System.out.println("=============================================================");
        System.out.println("             BANK QUEUE SIMULATION - INPUT SCREEN");
        System.out.println("=============================================================");
        System.out.print("Number of customers to simulate (e.g. 100)    : ");
        int n = sc.nextInt();
        System.out.print("Random number seed             (e.g. 123)    : ");
        long seed = sc.nextLong();
        System.out.print("Inter-arrival lower bound, minutes (e.g. 1)  : ");
        double iaLo = sc.nextDouble();
        System.out.print("Inter-arrival upper bound, minutes (e.g. 5)  : ");
        double iaHi = sc.nextDouble();
        System.out.print("Service time  lower bound, minutes (e.g. 1)  : ");
        double svLo = sc.nextDouble();
        System.out.print("Service time  upper bound, minutes (e.g. 6)  : ");
        double svHi = sc.nextDouble();

        if (n <= 0) n = 100;
        if (iaHi <= iaLo) iaHi = iaLo + 1;
        if (svHi <= svLo) svHi = svLo + 1;

        Rng rng = new Rng(seed);
        Row[] rows = new Row[n];
        ArrayList<Double> deps = new ArrayList<>();

        double prevArrival = 0, prevDeparture = 0;
        double sumIat=0, sumSt=0, sumWq=0, sumTis=0, idleTotal=0, maxWq=0, maxTis=0;
        int waited = 0, maxQ = 0;

        for (int i = 0; i < n; i++) {
            double r1 = rng.next(), r2 = rng.next();

            Row r = new Row();
            r.id   = i + 1;
            r.iat  = iaLo + (iaHi - iaLo) * r1;
            r.st   = svLo + (svHi - svLo) * r2;
            r.at   = prevArrival + r.iat;
            r.idle = Math.max(0, r.at - prevDeparture);
            r.sbt  = Math.max(r.at, prevDeparture);
            r.wq   = r.sbt - r.at;
            r.dep  = r.sbt + r.st;
            r.tis  = r.dep - r.at;

            int q = 0;
            for (double d : deps) if (d > r.at) q++;
            maxQ = Math.max(maxQ, q);
            deps.add(r.dep);

            if (r.wq > 0) waited++;
            maxWq  = Math.max(maxWq,  r.wq);
            maxTis = Math.max(maxTis, r.tis);
            sumIat += r.iat; sumSt += r.st; sumWq += r.wq;
            sumTis += r.tis; idleTotal += r.idle;

            prevArrival = r.at; prevDeparture = r.dep;
            rows[i] = r;
        }

        double totalTime = prevDeparture;
        double busyTime  = sumSt;
        double util      = busyTime / totalTime;
        double avgIat = sumIat/n, avgSt = sumSt/n;
        double avgWq  = sumWq/n,  avgTis = sumTis/n;
        double lq = sumWq/totalTime, ls = sumTis/totalTime;

        System.out.println("\n\n=============================================================");
        System.out.println("                    SIMULATION TABLE");
        System.out.println("=============================================================");
        System.out.printf(" C#      IAT      AT      ST     SBT      WQ     DEP     TIS    IDLE%n");
        System.out.println("---------------------------------------------------------------------");
        for (Row r : rows)
            System.out.printf("%4d%9.3f%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f%n",
                r.id, r.iat, r.at, r.st, r.sbt, r.wq, r.dep, r.tis, r.idle);

        System.out.println("\n\n=============================================================");
        System.out.println("                    QUEUE STATISTICS");
        System.out.println("=============================================================");
        System.out.printf("Customers simulated ................ : %d%n", n);
        System.out.printf("Total simulated time (min) ......... : %.3f%n", totalTime);
        System.out.printf("Average inter-arrival time (min) ... : %.3f%n", avgIat);
        System.out.printf("Average service time (min) ......... : %.3f%n", avgSt);
        System.out.printf("Average waiting time in queue (min)  : %.3f%n", avgWq);
        System.out.printf("Average time in system (min) ....... : %.3f%n", avgTis);
        System.out.printf("Maximum waiting time (min) ......... : %.3f%n", maxWq);
        System.out.printf("Maximum time in system (min) ....... : %.3f%n", maxTis);
        System.out.printf("Maximum queue length (customers) ... : %d%n",   maxQ);
        System.out.printf("Average queue length (customers) ... : %.3f%n", lq);
        System.out.printf("Average number in system ........... : %.3f%n", ls);
        System.out.printf("Customers who had to wait .......... : %d  (%.2f %%)%n",
                          waited, 100.0*waited/n);
        System.out.printf("Customers served without waiting ... : %d%n", n - waited);
        System.out.printf("Server busy time (min) ............. : %.3f%n", busyTime);
        System.out.printf("Server idle time (min) ............. : %.3f%n", idleTotal);
        System.out.printf("Server utilisation (%%) ............. : %.3f%n", 100.0*util);
        System.out.printf("Probability server is idle ......... : %.3f%n", 1.0 - util);
        System.out.printf("Traffic intensity rho (measured) ... : %.3f%n", avgSt/avgIat);
        System.out.printf("Theoretical E[IAT], E[ST], rho ..... : %.3f, %.3f, %.3f%n",
            (iaLo+iaHi)/2, (svLo+svHi)/2, (svLo+svHi)/(iaLo+iaHi));
        System.out.printf("%nLittle's Law check  Lq = lambda*Wq : %.3f  ~=  %.3f%n",
            lq, (double)n/totalTime * avgWq);
        if (util >= 1.0)
            System.out.println("\n*** WARNING: rho >= 1 -- queue is unstable, it grows with time.");
    }
}
