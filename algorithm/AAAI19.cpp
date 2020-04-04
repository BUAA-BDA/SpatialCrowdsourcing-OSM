#include <bits/stdc++.h>
using namespace std;

const int MAX_D = 8192, MAX_R = 16384, MAX = 1e9 + 7;

const double EPS = 1e-8;

struct Point {
    double x, y;
};

double sqr(double x) {
    return x * x;
}

double dist(const Point &A, const Point &B) {
    return sqrt(sqr(A.x - B.x) + sqr(A.y - B.y));
}

/////////////////// variables /////////////////////////

int m, n, E;

int M, N, Q;

struct Driver {
    Point o;
}driver[MAX_D];

struct Request {
    int id, val;
}req[MAX_R], typ[MAX_R];


vector <int> neibor[MAX_R];               // neibors of real requests
vector <pair <int, int> > gneibor[MAX_R]; // neibors of request types
vector <int> typeset[MAX_R * 16];         // the set of nodes of a type


/////////////////// grid ///////////////////////////////

double grid_len;
int num_x, num_y;

int make_type(const Request &r) {
    return r.id * 11 + r.val;
}

Point center(int id) {
    Point ret;
    double x = id / num_y, y = id % num_y;
    ret.x = (x + 0.5) * grid_len;
    ret.y = (y + 0.5) * grid_len;
    return ret;
}

///////////////// read data //////////////////////////

void read_sim(string simFile) {
    FILE *fp = fopen(simFile.c_str(), "r");
    fscanf(fp, "%d%d%d", &m, &n, &E);
    for (int i = 0; i < m; ++ i) {
        fscanf(fp, "%lf%lf", &driver[i].o.x, &driver[i].o.y);
    }
    for (int i = 0; i < n; ++ i) {
        fscanf(fp, "%d%d", &req[i].id, &req[i].val);
    }
    int u, v;
    for (int i = 0; i < E; ++ i) {
        fscanf(fp, "%d%d", &u, &v);
        neibor[v].push_back(u);
    }
}

void read_lp(string lpFile) {
    FILE *fp = fopen(lpFile.c_str(), "r");
    double tmp;
    fscanf(fp, "%d%d%d%lf%d%d", &m, &N, &Q, &grid_len, &num_x, &num_y);
    for (int i = 0; i < m; ++ i) {
        fscanf(fp, "%lf%lf", &tmp, &tmp);
    }
    for (int i = 0; i < N; ++ i) {
        fscanf(fp, "%d%d", &typ[i].id, &typ[i].val);
        typeset[make_type(typ[i])].push_back(i);
    }
    int u, v;
    for (int i = 0; i < Q; ++ i) {
        fscanf(fp, "%d%d", &u, &v);
        gneibor[v].push_back(make_pair(u, i));
    }
}

//////////////// LP solution //////////////////// 
double yu[MAX_D], yv[MAX_R], xf[1000010], OPT;

void read_sol(string solFile) {
    FILE *fp = fopen(solFile.c_str(), "r");
    for (int i = 0; i < m; ++ i) {
        fscanf(fp, "%lf", &yu[i]);
    }
    for (int i = 0; i < N; ++ i) {
        fscanf(fp, "%lf", &yv[i]);
    }
    for (int i = 0; i < Q; ++ i) {
        fscanf(fp, "%lf", &xf[i]);
    }
    fscanf(fp, "%lf", &OPT);
}

////////////////////// simulator /////////////////////////
struct Simulator {
    int matchU[MAX_D];
    int matchV[MAX_R];
    vector <int> V;
    double utility;
    int numBP, numMatch;
    void clear() {
        utility = numBP = numMatch = 0;
        memset(matchU, -1, sizeof matchU);
        memset(matchV, -1, sizeof matchV);
        V.clear();
    }
    void calcBP() {
        numBP = 0;
        for (int i = 0; i < V.size(); ++ i) {
            int v = V[i];
            for (int j = 0; j < neibor[v].size(); ++ j) {
                int u = neibor[v][j];
                if (matchU[u] == i) continue;
                int u1 = matchV[i], v1 = V[matchU[u]];
                if ((u1 == -1 || dist(driver[u1].o, center(req[v].id)) > dist(driver[u].o, center(req[v].id))) &&
                    (v1 == -1 || req[v1].val < req[v].val)) {
                    numBP ++;
                }
            }
        }
    }
} lpalg, greedy;


// input is the index of real request
void run_greedy(int v) {
    greedy.V.push_back(v);
    int vid = greedy.V.size() - 1;
    int uStar = -1;
    for (int j = 0; j < neibor[v].size(); ++ j) {
        int u = neibor[v][j];
        if (greedy.matchU[u] == -1 && (uStar == -1 || dist(driver[u].o, center(req[v].id)) <  dist(driver[uStar].o, center(req[v].id)))) {
            uStar = u;
        }
    }
    if (uStar != -1) {
        greedy.matchV[vid] = uStar;
        greedy.matchU[uStar] = vid;
        greedy.utility += req[v].val;
        greedy.numMatch ++;
    }
}

// input is the index of real request
void run_lpalg(int vv) {
    lpalg.V.push_back(vv);
    int vid = lpalg.V.size() - 1;
    int uStar = -1;
    int tnum = make_type(req[vv]);
    if (typeset[tnum].size() == 0) return;
    int v = typeset[tnum][rand() % typeset[tnum].size()];
    double prob = rand() % MAX * 1. / MAX;
    double sum = 0;
    for (int j = 0; j < gneibor[v].size(); ++ j) {
        sum += xf[gneibor[v][j].second];
        if (prob < sum) {
            uStar = gneibor[v][j].first;
            break;
        }
    }
    if (lpalg.matchU[uStar] == -1) {
        lpalg.matchV[vid] = uStar;
        lpalg.matchU[uStar] = vid;
        lpalg.utility += req[vv].val;
        lpalg.numMatch ++;
    }
}

int main(int argc, char ** argv) {
    string simFile = "test.sim", lpFile = "test.lp", solFile = "test.sol";
    string outFile = "test.out";
    if (argc > 1) {
        simFile = argv[1];
    }
    if (argc > 2) {
        lpFile = argv[2];
    }
    if (argc > 3) {
        solFile = argv[3];
    }
    if (argc > 4) {
        outFile = argv[4];
    }
    read_sim(simFile);
    read_lp(lpFile);
    read_sol(solFile);
    
    srand(time(NULL));

    double Gu = 0, Lu = 0, Gb = 0, Lb = 0, Gm = 0, Lm = 0;

    int Troud = 1000;             
    
    greedy.clear();
    lpalg.clear();
    for (int i = 0; i < n; ++ i) {
        run_greedy(i);
        run_lpalg(i);
    }
    
    
    lpalg.calcBP();
    Lu = lpalg.utility;
    Lb = lpalg.numBP;
    Lm = lpalg.numMatch;
    
    printf("LPALG-----CR = %f, BP = %f, match = %f\n", Lu / OPT, Lb, Lm);
    return 0;
}
