#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
#include "TwoPunctures.h"

#include <mpi.h>
#include <omp.h>

using namespace std;

struct twoPunctureInput{
    int nA, nB, nphi, Newtonmaxit;

    double mp, mm, b, Mp, Mm, admtol, Newtontol;
    double P_plusx, P_plusy, P_plusz, P_minusx, P_minusy, P_minusz;
    double S_plusx, S_plusy, S_plusz, S_minusx, S_minusy, S_minusz;
};

int main(int argc, char** argv){
    // สำหรับการใช้แค่ MPI paralle;
    // MPI_Init(&argc, &argv);

    // สำหรับ Hybrid parallelism
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    int node, node_size;
    MPI_Comm_size(MPI_COMM_WORLD, &node_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &node);

    double start_time;
    if (node == node_root){
        start_time = MPI_Wtime();
    }
    twoPunctureInput data;

    // read parameter from file
    if (node == node_root){
        string file_name = "TwoPunctureinput.par";
        ifstream file(file_name, ios::in);

        if (!file.is_open()){
           cout << "Can not open parameter file " << file_name << endl;

           MPI_Abort(MPI_COMM_WORLD, 1);
        }

        string line;

        while (getline(file, line)){
            if (line.find("ABE") == 0){
                stringstream ss(line);
                string temp, name, sval;

                ss >> name >> temp >> sval;

                int pos = name.find("::") + 2;
                name = name.substr(pos);

                if (name == "mp")                   data.mp = stod(sval);
                else if (name == "mm")              data.mm = stod(sval);
                else if (name == "b")               data.b = stod(sval);

                else if (name == "P_plusx")         data.P_plusy = -stod(sval);
                else if (name == "P_plusy")         data.P_plusx = stod(sval);
                else if (name == "P_plusz")         data.P_plusz = stod(sval);

                else if (name == "P_minusx")        data.P_minusy = -stod(sval);
                else if (name == "P_minusy")        data.P_minusx = stod(sval);
                else if (name == "P_minusz")        data.P_minusz = stod(sval);

                else if (name == "S_plusx")         data.S_plusy = -stod(sval);
                else if (name == "S_plusy")         data.S_plusx = stod(sval);
                else if (name == "S_plusz")         data.S_plusz = stod(sval);

                else if (name == "S_minusx")        data.S_minusy = -stod(sval);
                else if (name == "S_minusy")        data.S_minusx = stod(sval);
                else if (name == "S_minusz")        data.S_minusz = stod(sval);

                else if (name == "Mp")              data.Mp = stod(sval);
                else if (name == "Mm")              data.Mm = stod(sval);
                else if (name == "admtol")          data.admtol = stod(sval);
                else if (name == "Newtontol")       data.Newtontol = stod(sval);

                else if (name == "nA")              data.nA = stoi(sval);
                else if (name == "nB")              data.nB = stoi(sval);
                else if (name == "nphi")            data.nphi = stoi(sval);
                else if (name == "Newtonmaxit")     data.Newtonmaxit = stoi(sval);
            }
        }
        file.close();
    }

    MPI_Bcast(&data, sizeof(twoPunctureInput), MPI_BYTE, node_root, MPI_COMM_WORLD);

    // echo parameters
    if (node == node_root){
        cout << "///////////////////////////////////////////////////////////////"   << endl;
        cout << "     mp     = " << data.mp                                         << endl;
        cout << "     mm     = " << data.mm                                         << endl;
        cout << "     b      = " << data.b                                          << endl;
        cout << "  P_plusx   = " << data.P_plusx                                    << endl;
        cout << "  P_plusy   = " << data.P_plusy                                    << endl;
        cout << "  P_plusz   = " << data.P_plusz                                    << endl;
        cout << "  P_minusx  = " << data.P_minusx                                   << endl;
        cout << "  P_minusy  = " << data.P_minusy                                   << endl;
        cout << "  P_minusz  = " << data.P_minusz                                   << endl;
        cout << "  S_plusx   = " << data.S_plusx                                    << endl;
        cout << "  S_plusy   = " << data.S_plusy                                    << endl;
        cout << "  S_plusz   = " << data.S_plusz                                    << endl;
        cout << "  S_minusx  = " << data.S_minusx                                   << endl;
        cout << "  S_minusy  = " << data.S_minusy                                   << endl;
        cout << "  S_minusz  = " << data.S_minusz                                   << endl;
        cout << "     Mp     = " << data.Mp                                         << endl;
        cout << "     Mm     = " << data.Mm                                         << endl;
        cout << "   admtol   = " << data.admtol                                     << endl;
        cout << " Newtontol  = " << data.Newtontol                                  << endl;
        cout << "     nA     = " << data.nA                                         << endl;
        cout << "     nB     = " << data.nB                                         << endl;
        cout << "    nphi    = " << data.nphi                                       << endl;
        cout << "Newtonmaxit = " << data.Newtonmaxit                                << endl;
        cout << "///////////////////////////////////////////////////////////////"   << endl;
    }

    // Computation
    {
        TwoPunctures *ADM;

        ADM = new TwoPunctures(data.mp, data.mm, data.b,
                            data.P_plusx, data.P_plusy, data.P_plusz,
                            data.S_plusx, data.S_plusy, data.S_plusz,
                            data.P_minusx, data.P_minusy, data.P_minusz,
                            data.S_minusx, data.S_minusy, data.S_minusz,
                            data.nA, data.nB, data.nphi, data.Mp, data.Mm,
                            data.admtol, data.Newtontol, data.Newtonmaxit);

        ADM->Solve();

        if (node == node_root){
            char filename[] = "Ansorg.psid";
            ADM->Save(filename);
        }
    }

    if (node == node_root){
        cout << "===============================================================" << endl;
        cout << "Initial data is successfully producede!!" << endl;
    }

    MPI_Finalize();

    double end_time;
    if (node == node_root){
        end_time = MPI_Wtime();
        double total_time = end_time - start_time;
        cout << "total time: " << total_time << " second" << endl;
    }
    return  0;
}