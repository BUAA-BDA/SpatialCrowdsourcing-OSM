#include "ortools/linear_solver/linear_solver.h"
#include "ortools/linear_solver/linear_solver.pb.h"
#include <bits/stdc++.h>

using namespace operations_research;
using namespace std;

const int MAX_D = 8192, MAX_R = 16384;

int m, n, E;

struct Point {
    double x, y;
};

struct Driver{
    Point o;
}driver[MAX_D];

struct Request {
    int t;
    Point o, d;
    double rad, val;
}req[MAX_R];

vector <pair <int, int> > flist;

vector <pair <int, int> > uneibor[MAX_D];
vector <pair <int, int> > vneibor[MAX_R];

double sqr(double x) {
    return x * x;
}

double dist(const Point &A, const Point &B) {
    return sqrt(sqr(A.x - B.x) + sqr(A.y - B.y));
}


double grid_len;
int num_x, num_y;

Point center(int id) {
    Point ret;
    int x = id / num_y;
    int y = id % num_y;
    ret.x = (x + 0.5) * grid_len;
    ret.y = (y + 0.5) * grid_len;
    return ret;
}

void read_data(string inFile) {
    freopen(inFile.c_str(), "r", stdin);
    scanf("%d%d%d%lf%d%d", &m, &n, &E, &grid_len, &num_x, &num_y);
    for (int i = 0; i < m; ++ i) {
        scanf("%lf%lf", &driver[i].o.x, &driver[i].o.y);
    }
    for (int i = 0; i < n; ++ i) {
        scanf("%d%lf", &req[i].t, &req[i].val);
        req[i].o = center(req[i].t);
    }
    int u, v;
    for (int i = 0; i < E; ++ i) {
        scanf("%d%d", &u, &v);
        flist.push_back(make_pair(u, v));
        uneibor[u].push_back(make_pair(v, i));
        vneibor[v].push_back(make_pair(u, i));
    }
}

MPVariable* xf[1000010];
MPVariable* yu[MAX_D];
MPVariable* yv[MAX_R];

MPConstraint* cforu[MAX_D];
MPConstraint* cforv[MAX_R];
MPConstraint* cfors[1000010];

const double EXP = exp(1.0);

void RunLPModel(string outFile) {
    MPSolver solver("LinearExample", MPSolver::GLOP_LINEAR_PROGRAMMING);
    
    // variables.
    for (int i = 0; i < flist.size(); ++ i) {
        string s = "xf_";
        s += to_string(i);
        xf[i] = solver.MakeNumVar(0.0, 1.0, s.c_str());
    }
    for (int i = 0; i < m; ++ i) {
        string s = "yu_";
        s += to_string(i);
        yu[i] = solver.MakeNumVar(0.0, 1.0, s.c_str());
    }
    for (int i = 0; i < n; ++ i) {
        string s = to_string(flist[i].first);
        s += "_";
        s += to_string(flist[i].second);
        yv[i] = solver.MakeNumVar(0.0, 1.0, s.c_str());
    }
    
    // Objective function: 
    MPObjective* const objective = solver.MutableObjective();
    for (int i = 0; i < flist.size(); ++ i) {
        objective->SetCoefficient(xf[i], req[flist[i].second].val);
    }
    objective->SetMaximization();
    
    // Constraint: u
    for (int i = 0; i < m; ++ i) {
        cforu[i] = solver.MakeRowConstraint(1.0, 1.0);
        for (int j = 0; j < uneibor[i].size(); ++ j) {
            int ff = uneibor[i][j].second;
            cforu[i]->SetCoefficient(xf[ff], 1);
        }
        cforu[i]->SetCoefficient(yu[i], 1);
    }

    // Constraint: v
    for (int i = 0; i < n; ++ i) {
        cforv[i] = solver.MakeRowConstraint(1.0, 1.0);
        for (int j = 0; j < vneibor[i].size(); ++ j) {
            int ff = vneibor[i][j].second;
            cforv[i]->SetCoefficient(xf[ff], 1);
        }
        cforv[i]->SetCoefficient(yv[i], 1);
    }
    
    
    // Constraint: stable
    for (int i = 0; i < flist.size(); ++ i) {
        int u = flist[i].first, v = flist[i].second;
        cfors[i] = solver.MakeRowConstraint(0.0, 1.0 + 1.0 / EXP);
        double cmp;
        cmp = req[v].val;
        for (int j = 0; j < uneibor[u].size(); ++ j) {
            if (uneibor[u][j].first == v) continue;
            double tmp = req[uneibor[u][j].first].val;
            if (tmp < cmp) {
                cfors[i]->SetCoefficient(xf[uneibor[u][j].second], 1.0);
            }
        }
        
        cmp = dist(driver[u].o, req[v].o);
        for (int j = 0; j < vneibor[v].size(); ++ j) {
            if (vneibor[v][j].first == u) continue;
            double tmp = dist(driver[vneibor[v][j].first].o, req[v].o);
            if (tmp > cmp) {
                cfors[i]->SetCoefficient(xf[vneibor[v][j].second], 1.0);
            }
        }
        
        cfors[i]->SetCoefficient(xf[i], 1);
        cfors[i]->SetCoefficient(yu[flist[i].first], 1.0);
        cfors[i]->SetCoefficient(yv[flist[i].second], 1.0);
    }
    
    

    printf("Number of variables = %d \n", solver.NumVariables());
    printf("Number of constraints = %d \n", solver.NumConstraints());

    
    solver.Solve();

    // The objective value of the solution.
    printf("Optimal objective value = %.6f \n", objective->Value());
    
    FILE *fp = fopen(outFile.c_str(), "w");
    // The value of each variable in the solution.
    for (int i = 0; i < m; ++ i) {
        fprintf(fp, "%.6f\n", yu[i]->solution_value());
    }
    for (int i = 0; i < n; ++ i) {
        fprintf(fp, "%.6f\n", yv[i]->solution_value());
    }
    for (int i = 0; i < flist.size(); ++ i) {
        fprintf(fp, "%.6f\n", xf[i]->solution_value());
    }
    fprintf(fp, "%.6f\n", objective->Value());
    fclose(fp);
    
}

int main(int argc, char** argv) {
    string inFile, outFile;
    if (argc > 1) {
        inFile = argv[1];
    } else {
        inFile = "in.txt";
    }
    if (argc > 2) {
        outFile = argv[2];
    } else {
        outFile = "out.txt";
    }
    
    read_data(inFile);
    
    RunLPModel(outFile);
    return 0;
}
