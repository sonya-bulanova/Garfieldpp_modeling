#include <iostream>
#include <cmath>

void draw_distr(){
  Double_t distance[] = { 0.1, 0.5, 1.0, 1.5, 2.0, 3.0, 4.0, 4.5, 4.8 };
  Double_t mpv[] = { 21.42 + 50, 24.28 + 50, 29.16 + 50, 35.04 + 50, 42.68 + 50, 68.22 + 50, 110.7 + 50, 138.2 + 50, 156.2 + 50 };
  Double_t sigma[] = { 5.453, 4.681, 3.955, 3.703, 3.374, 3.411, 3.552, 3.518, 3.575 };
  Double_t er_x[9] = {0};
  Double_t er_mpv[] = { 0.09, 0.08, 0.06, 0.06, 0.06, 0.06, 0.1, 0.1, 0.1 };
  Double_t er_sigma[] = { 0.051, 0.060, 0.045, 0.046, 0.040, 0.040, 0.040, 0.043, 0.042, 0.046 };

  Double_t mpv_field[] = { 71.14, 74.15, 79.06, 84.97, 92.61, 118.2, 160.6, 188.1, 206.5 }; //actually, noise
  Double_t sigma_field[] = { 5.653, 4.885, 4.13, 3.79, 3.533, 3.581, 3.735, 3.558, 3.732 }; //actually, noise
  Double_t er_mpv_field[] = { 0.09, 0.07, 0.06, 0.06, 0.05, 0.06, 0.1, 0.1, 0.1 };
  Double_t er_sigma_field[] = { 0.062, 0.051, 0.047, 0.039, 0.037, 0.036, 0.041, 0.042, 0.043 };

  Double_t mpv_field_noise[] = { 71.01, 74.15, 79.06, 84.97, 92.61, 118.2, 160.6, 188.1, 206.5 }; //actually, noise 1500 e
  Double_t sigma_field_noise[] = { 5.922, 4.925, 4.185, 3.941, 3.614, 3.6, 3.748, 3.619, 3.745 }; //actually, noise 1500 e
  Double_t er_mpv_field_noise[] = { 0.09, 0.07, 0.06, 0.06, 0.05, 0.06, 0.1, 0.1, 0.1 };
  Double_t er_sigma_field_noise[] = { 0.062, 0.051, 0.047, 0.039, 0.037, 0.036, 0.041, 0.042, 0.043 };

  //Double_t mpv_noise[] = {};
  //Double_t sigma_noise[] = {};

  auto graph_mpv = new TGraphErrors(9, distance, mpv, er_x, er_mpv);
  auto graph_sigma = new TGraphErrors(9, distance, sigma, er_x, er_sigma);
  auto graph_mpv_field = new TGraphErrors(9, distance, mpv_field, er_x, er_mpv_field);
  auto graph_sigma_field = new TGraphErrors(9, distance, sigma_field, er_x, er_sigma_field);

  auto graph_sigma_field_noise = new TGraphErrors(9, distance, sigma_field_noise, er_x, er_sigma_field_noise);

  auto c1 = new TCanvas("c1", "c1", 1200, 1200);
  c1 -> cd();
  c1 -> SetGrid();
  graph_mpv -> SetMaximum(180);
  graph_mpv -> SetMinimum(18);
  //gPad -> SetLogy();
  graph_mpv_field -> SetTitle("MPV from distance to wire; distance, mm; MPV");
  graph_mpv -> SetLineColor(kBlue);
  graph_mpv -> SetLineWidth(2);
  graph_mpv -> SetMarkerStyle(8);
  graph_mpv_field -> SetLineColor(kGreen + 2);
  graph_mpv_field -> SetLineWidth(2);
  graph_mpv_field -> SetMarkerStyle(8);
  graph_mpv_field -> Draw("APL");
  graph_mpv -> Draw("PLsame");

  auto legend = new TLegend(0.6, 0.9, 0.9, 0.8);
  legend -> SetHeader("MPV results compare","C");
	legend -> AddEntry(graph_mpv,"No magnetic field, no angle, no noise","pl");
	legend -> AddEntry(graph_mpv_field,"No magnetic field, no angle, noise = 1000 e","pl");
  //legend -> AddEntry(six_graph,"Fit with pol6, Error 2.50486 %","l");
	legend -> Draw();

  //graph_mpv_field -> Fit("pol6", "", "", 0.0, 5.0);
  TF1 *f1 = (TF1 *)gROOT->GetFunction("pol6");
  std::cout << "In point x = " << 0. << " function: " << f1 -> Eval(0.) << std::endl;
  for (int i = 0; i < 9; i++){
    std::cout << "In point x = " << distance[i] << " function: " << f1 -> Eval(distance[i]) << std::endl;
  }
  std::cout << "In point x = " << 5.0 << " function: " << f1 -> Eval(5.0) << std::endl;

  //TSpline3 *grs = new TSpline3("grs", graph_mpv);

  c1 -> Update();
  c1 -> Print("mpv_field.png");

  auto c2 = new TCanvas("c2", "c2", 1200, 1200);
  c2 -> cd();
  c2 -> SetGrid();

  graph_sigma_field_noise -> SetMaximum(6);
  graph_sigma_field_noise -> SetMinimum(3);

  graph_sigma_field_noise -> SetTitle("Sigma from distance to wire; distance, mm; sigma");
  graph_sigma -> SetLineColor(kBlue);
  graph_sigma -> SetLineWidth(2);
  graph_sigma -> SetMarkerStyle(8);
  graph_sigma_field -> SetLineColor(kGreen + 2);
  graph_sigma_field -> SetLineWidth(2);
  graph_sigma_field -> SetMarkerStyle(8);
  graph_sigma_field_noise -> SetLineColor(kRed);
  graph_sigma_field_noise -> SetLineWidth(2);
  graph_sigma_field_noise -> SetMarkerStyle(8);

  graph_sigma_field_noise -> Draw("APL");
  graph_sigma -> Draw("PLsame");
  graph_sigma_field -> Draw("PLsame");

  auto legend1 = new TLegend(0.6, 0.9, 0.9, 0.8);
  legend1 -> SetHeader("Sigma results compare","C");
	legend1 -> AddEntry(graph_sigma,"No magnetic field","pl");
	legend1 -> AddEntry(graph_sigma_field,"No magnetic field, noise = 1000 e","pl");
  legend1 -> AddEntry(graph_sigma_field_noise,"No magnetic field, noise = 1500 e","pl");
  //legend -> AddEntry(six_graph,"Fit with pol6, Error 2.50486 %","l");
	legend1 -> Draw();

  //TF1 *f1 = new TF1("f1","[0]*exp([1] + [2]*x)", 0., 5.0);
  //graph_sigma -> Fit("f1");

  c2 -> Update();
  c2 -> Print("sigma_field.png");
}
