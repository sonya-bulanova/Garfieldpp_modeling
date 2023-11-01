#include <iostream>
#include <cmath>
#include <string>
#include <fstream>

#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
//#include "TRandom2.h"

using namespace std;

int get_shaper_signal_params_RT(string file_name_prefix="shift_wire_", int signal_count = 1, string out_prefix = "shift_wire_250mkm_", int hist_bin_preset = 1)
{
    ifstream filein;
    string filename;

    double h1_l1 = 0., h1_l2 = 5., h1_r1 = 300., h1_r2 = 1200.;
    int h1_b1 = 250, h1_b2 = 300;
    double h4_l = 0, h4_r = 250;
    int h4_b = 50;
    double h5_l = 0, h5_r = 350;
    int h5_b = 50;
    double h6_l = 0, h6_r = 1500;
    int h6_b = 100;
    double fit1_l = h4_r * 0.1, fit1_r = h4_r * 0.9;
    double fit2_l = h5_r * 0.1, fit2_r = h5_r * 0.9;

    switch(hist_bin_preset)
    {
        case 1: //1 cm tube; noAngle; noField; 1 mm dist; peaking time 25 nsec
        h4_b = 50;
        h4_l = 0;
        h4_r = 250;

        h5_b = 80;
        h5_l = 20;
        h5_r = 100;

        fit2_l = 37;
        fit2_r = 65;
        break;
    }

    //reading file with coordinates
    ifstream filein_coord;
    string filename_coord;
    filename_coord = "rTracks";
    filein_coord.open(filename_coord.c_str(), ios::in);
    if(!filein_coord.is_open())
    {
        cout << "File <<" << filename_coord << ">> not open!" << endl;
        return 0;
    }

    TH2D signal_collection("SPICE_signals", "Garfield+LTSpice signals;Time [nsec];Amplitude [mV]", h1_b1, h1_l1, h1_r1, h1_b2, h1_l2, h1_r2); //h1
    TH1D time_of_signal_max("time_distr", "Time of signal extremum;Time [nsec]", h4_b, 0, 250); //h4
    TH1D amp_spectra("peak_distr", "Maximum of amplitude;Amplitude [mV]", h6_b, h6_l, h6_r); //h6
    TH2D RT_curve("RT_curve", "RT Graph; R[mm]; t[ns]",  70, -5.0, 5.0, h4_b, 0, 250);

    const int NofLowThr = 11;
    TH1D tOfLowThr[NofLowThr];
    float valOfLowThr[NofLowThr] = {0.001, 0.002, 0.003, 0.004, 0.005, 0.006, 0.007, 0.008, 0.009, 0.01, 0.015}; // thresholds [V]
    for(int ii=0; ii<NofLowThr; ii++){
        TString str  = "Moment of "; str+=int(valOfLowThr[ii]*1000); str+= " mV crossing;Time [nsec]";
        TString nstr = "tOfLowThr["; nstr+=ii; nstr+="]";
        tOfLowThr[ii]=TH1D(nstr.Data(), str.Data(), h5_b, 0, 250); //h5
    }

    TFile outfile("04_99mm.root", "recreate");


    for(int sigN = 0; sigN <= 19999; sigN++) // <= modified for debug
    {
        filename = "AsselWireShift/processed_results/Ar_70_co2_30_10mm_30mkm_1750V_B0_1atm_T20_2mm_05geV_bin2000_250mkm_" + std::to_string(sigN) + ".sig.csv";
        filein.open(filename.c_str(), ios::in);
        if(!filein.is_open())
        {
            cout << "File <<" << filename << ">> not open!" << endl;
            return 0;
        }

        string line;
        getline(filein, line);
        getline(filein, line);

        double time, current, voltage;
        double signal_max = -1, signal_max_time = 0;
        bool thr_rec[NofLowThr]; for (int ii=0; ii<NofLowThr; ii++) thr_rec[ii] = false;
        double distance;
        filein_coord >> distance;

        while(!filein.eof())
        {
            filein >> time >> current >> voltage;
            //cout << "Signal #" << sigN << "   time: " << time*1000000000 << "   voltage: " << voltage*1000 << endl;

            signal_collection.Fill(time*1000000000, voltage*1000);

            if(signal_max < voltage)
            {
                signal_max = voltage;
                signal_max_time = time;
            }

            for(int ii=0; ii<NofLowThr; ii++){
                if(!thr_rec[ii]){
                    if(voltage > valOfLowThr[ii]){
                        tOfLowThr[ii].Fill(time*1000000000 - 50);
                        thr_rec[ii] = true;
                        RT_curve.Fill(distance * 10, time*1000000000 - 50);
                    }
                }
            }
        }
        time_of_signal_max.Fill(signal_max_time*1000000000);
        amp_spectra.Fill(signal_max*1000);
        //cout << "Signal #" << sigN << "   time for signal extremum: " << signal_max_time*1000000000 << endl << endl;

        filein.close();
        filein.clear();
    }
    filein_coord.close();

     outfile.WriteObject(tOfLowThr, "time_of_low_threshold");
     outfile.WriteObject(&signal_collection, "signal_collection" );
     outfile.WriteObject(&time_of_signal_max, "time_of_signal_max");
     outfile.WriteObject(&amp_spectra, "Maximum of amplitude");

    auto canv = new TCanvas("canv", "Find average output", 300, 150, 1000, 600);
    //canv->SetFillColor(42);
    //canv->SetGrid();
    //canv->GetFrame()->SetFillColor(21);
    //canv->GetFrame()->SetBorderSize(12);
    //canv->SetLogy();
    gStyle->SetOptStat(0);

    signal_collection.Draw("COL");
    canv->Update();
    filename = out_prefix + "_allSig.png";
    canv->SaveAs(filename.c_str());

    gStyle->SetOptStat(1110);
    gStyle->SetOptFit(1111);
    gStyle->SetStatX(0.95);
    gStyle->SetStatY(0.92);
    canv->SetGrid(0, 0);

    time_of_signal_max.Draw();
    //auto fit1 = new TF1("fit_red1","gaus", fit1_l, fit1_r);
    //fit1->SetLineColor(6);
    //fit1->SetLineWidth(3);
    //time_of_signal_max.Fit(fit1, "R");
    canv->Update();
    filename = out_prefix + "_maxOfSig.png";
    canv->SaveAs(filename.c_str());

    TF1 * fit2[NofLowThr];
    TF1 * fit3[NofLowThr];
    for(int ii=0; ii<NofLowThr; ii++){
        tOfLowThr[ii].Draw();
        fit2[ii] = new TF1("fit_red2","gaus", 25, 55);
        fit2[ii]->SetLineStyle(4);
        fit2[ii]->SetLineWidth(3);
        //tOfLowThr[ii].Fit(fit2[ii], "R");
        canv->Update();

        /// !!!TBD!!! get convergence
        double mean = fit2[ii]->GetParameter(1); double sigma = fit2[ii]->GetParameter(2);
        fit3[ii] = new TF1("fit_red3","gaus", mean-3*sigma, mean+3*sigma);
        fit3[ii]->SetLineWidth(3);
        //tOfLowThr[ii].Fit(fit3[ii], "R");
        canv->Update();

        /// the grass
        double lowEdge = fit3[ii]->GetParameter(1)-3*fit3[ii]->GetParameter(2);
        int binN = 0;
        int stat = 0;
        while((tOfLowThr[ii].GetBinCenter(binN)<lowEdge) && (binN<tOfLowThr[ii].GetNbinsX())) {
            stat+=tOfLowThr[ii].GetBinContent(binN);
            binN++;
        };
        std::cout << "threshold " << valOfLowThr[ii] << " V; left content [%] " << stat/tOfLowThr[ii].Integral()*100 << std::endl;

        filename = out_prefix +"_"+(valOfLowThr[ii])*1000+"_thrCross.png";
        canv->SaveAs(filename.c_str());

    };

    gStyle->SetOptFit(1);
    amp_spectra.Draw();
    amp_spectra.Fit("landau", "", "", 0, 1000);
    canv->Update();
    filename = out_prefix + "_ampSpectra.png";
    canv->SaveAs(filename.c_str());

    gStyle->SetOptStat(0);
    RT_curve.Draw("COLZ");
    canv->Update();
    filename = out_prefix + "RT_curve.png";
    canv->SaveAs(filename.c_str());

    return 0;
}
