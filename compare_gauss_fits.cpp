#include <iostream>
#include <cmath>
#include <string>
#include <fstream>

#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
using namespace std;

int compare_gauss_fits()
{
    string file_name_prefix = "noField_SPD_1500e/Ar_70_co2_30_1750V_B0_1atm_4_8mm_bin2000_";
    string file_name_prefix_new = "noField_SPD_1500e/Ar_70_co2_30_1750V_B0_1atm_0_1mm_bin2000_";
    int signal_count = 4999;
    string out_prefix = "draw_on_one_canvas_";
    int hist_bin_preset = 1;
    ifstream filein;
    ifstream filein_new;
    string filename;
    string filename_new;

    double h1_l1 = 0., h1_l2 = -1500., h1_r1 = 400., h1_r2 = 1500.;
    int h1_b1 = 400, h1_b2 = 200;
    double h4_l = 0, h4_r = 250;
    int h4_b = 50;
    double h5_l = 0, h5_r = 150;
    int h5_b = 50;
    double h6_l = 0, h6_r = 5000;
    int h6_b = 100;
    double fit1_l = h4_r * 0.1, fit1_r = h4_r * 0.9;
    double fit2_l = h5_r * 0.1, fit2_r = h5_r * 0.9;

    switch(hist_bin_preset)
    {
        case 1: //1 cm tube; noAngle; noField; 1 mm dist; peaking time 25 nsec
        h4_b = 50;
        h4_l = 0;
        h4_r = 200;

        h5_b = 120;
        h5_l = 0;
        h5_r = 180;

        fit2_l = 180;
        fit2_r = 155;
        break;

        case 2: //1 cm tube; noAngle; noField; 2 mm dist; peaking time 25 nsec
        h4_b = 50;
        h4_l = 25;
        h4_r = 225;

        h5_b = 60;
        h5_l = 15;
        h5_r = 75;

        fit2_l = 5;
        fit2_r = 55;
        break;

    }

    TH2D signal_collection("SPICE_signals", "Garfield+LTSpice signals, 4_8 mm;Time [nsec];Amplitude [mV]", h1_b1, h1_l1, h1_r1, h1_b2, h1_l2, h1_r2); //h1
    TH1D time_of_signal_max("time_distr", "Time of signal extremum, 4_8 mm;Time [nsec]", h4_b, h4_l, h4_r); //h4
    TH1D amp_spectra("peak_distr", "Maximum of amplitude, 4_8 mm; Amplitude [mV]", h6_b, h6_l, h6_r); //h6

    TH1D time_of_low_threshold("time_distr2", "Signal arrival time, distance 0.1 mm and 4.8 mm;Time [nsec]", h5_b, h5_l, h5_r); //h5

    for(int sigN = 0; sigN <= signal_count; sigN++)
    {
        filename = file_name_prefix + std::to_string(sigN) + ".sig.csv";
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
        bool thr_rec = false;

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

            if(!thr_rec)
            {
                if(voltage > 0.01)
                {
                    time_of_low_threshold.Fill(time*1000000000 - 50);
                    //cout << "Signal #" << sigN << "   time for low thr crossing: " << time*1000000000 << endl;
                    thr_rec = true;
                }
            }
        }

        time_of_signal_max.Fill(signal_max_time*1000000000);
        amp_spectra.Fill(signal_max*1000);
        //cout << "Signal #" << sigN << "   time for signal extremum: " << signal_max_time*1000000000 << endl << endl;

        filein.close();
        filein.clear();
    }

    //Сигналы для 0.1 мм
    for(int sigN = 0; sigN <= signal_count; sigN++)
    {
        filename_new = file_name_prefix_new + std::to_string(sigN) + ".sig.csv";
        filein_new.open(filename_new.c_str(), ios::in);
        if(!filein_new.is_open())
        {
            cout << "File <<" << filename_new << ">> not open!" << endl;
            return 0;
        }

        string line_new;
        getline(filein_new, line_new);
        getline(filein_new, line_new);

        double time, current, voltage;
        double signal_max = -1, signal_max_time = 0;
        bool thr_rec = false;

        while(!filein_new.eof())
        {
            filein_new >> time >> current >> voltage;
            //cout << "Signal #" << sigN << "   time: " << time*1000000000 << "   voltage: " << voltage*1000 << endl;

            signal_collection.Fill(time*1000000000, voltage*1000);

            if(signal_max < voltage)
            {
                signal_max = voltage;
                signal_max_time = time;
            }

            if(!thr_rec)
            {
                if(voltage > 0.01)
                {
                    time_of_low_threshold.Fill(time*1000000000 - 50);
                    //cout << "Signal #" << sigN << "   time for low thr crossing: " << time*1000000000 << endl;
                    thr_rec = true;
                }
            }
        }

        time_of_signal_max.Fill(signal_max_time*1000000000);
        amp_spectra.Fill(signal_max*1000);
        //cout << "Signal #" << sigN << "   time for signal extremum: " << signal_max_time*1000000000 << endl << endl;

        filein_new.close();
        filein_new.clear();
    }

    auto canv = new TCanvas("canv", "Find average output", 300, 150, 1000, 600);
    gStyle->SetOptStat(0);

    signal_collection.Draw("COL");
    canv->Update();
    filename = out_prefix + "_allSig.png";
    canv->SaveAs(filename.c_str());

    gStyle->SetOptStat(0000);
    gStyle->SetOptFit(0000);
    gStyle->SetStatX(0.95);
    gStyle->SetStatY(0.92);
    canv->SetGrid(0, 0);

    time_of_signal_max.Draw();

    canv->Update();
    filename = out_prefix + "_maxOfSig.png";
    canv->SaveAs(filename.c_str());

    auto fit1 = new TF1("fit_red1","gaus", 5, 20);
    fit1->SetLineColor(6);
    fit1->SetLineWidth(3);

    auto fit2 = new TF1("fit_red2","gaus", 140, 160);
    fit2->SetLineWidth(3);
    time_of_low_threshold.SetMaximum(800);
    time_of_low_threshold.Sumw2(kFALSE);
    time_of_low_threshold.Draw("][");
    time_of_low_threshold.Fit(fit1, "WCFQ+", "same", 2, 45);
    time_of_low_threshold.Fit(fit2, "WCFQ+", "same", 140, 175);

    auto legend0 = new TLegend(0.3, 0.3, 0.6, 0.6);
    legend0 -> SetHeader("Signal arrival time", "C");
  	legend0 -> AddEntry(fit1, "0.1 mm, #sigma = 5.922", "");
    legend0 -> AddEntry(fit1, "0.1 mm, Mean = 21.24", "");

    legend0 -> AddEntry(fit2, "4.8 mm, #sigma = 3.745", "");
    legend0 -> AddEntry(fit2, "4.8 mm, Mean = 156.5", "");
    //legend -> AddEntry(six_graph,"Fit with pol6, Error 2.50486 %","l");
  	legend0 -> Draw();

    filename = out_prefix + "_thrCross.png";

    canv->Update();
    canv->SaveAs(filename.c_str());

    gStyle->SetOptStat(111111);
    amp_spectra.Draw();
    canv->Update();
    filename = out_prefix + "_ampSpectra.png";
    canv->SaveAs(filename.c_str());

    return 0;
}
