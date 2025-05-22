import ROOT
import numpy as np

dir_qcd="/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/QCD_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root"
dir_dy="/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/DY_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root"
dir_ttjets="/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/ttJets_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root"

branch_name="sist_mass"

h_dy = ROOT.TH1F("h_dy", "", 20, 0, 100)
h_qcd = ROOT.TH1F("h_qcd", "", 20, 0, 100)
h_signal = ROOT.TH1F("h_signal", "", 20, 0, 100)
h_data = ROOT.TH1F("h_data", "", 20, 0, 100)

for i in range(1, 21):
    h_dy.SetBinContent(i, np.random.poisson(10))
    h_qcd.SetBinContent(i, np.random.poisson(5))
    h_signal.SetBinContent(i, np.random.poisson(3))
    val = h_dy.GetBinContent(i) + h_qcd.GetBinContent(i) + np.random.poisson(2)
    h_data.SetBinContent(i, val)
    h_data.SetBinError(i, np.sqrt(val))

stack = ROOT.THStack("stack", "Stacked Plot;X axis;Events")
h_dy.SetFillColor(ROOT.kYellow)
h_qcd.SetFillColor(ROOT.kRed)
stack.Add(h_qcd)
stack.Add(h_dy)

c = ROOT.TCanvas("c", "c", 800, 600)
stack.Draw("HIST")
h_signal.SetLineColor(ROOT.kBlack)
h_signal.SetLineWidth(2)
h_signal.Draw("HIST SAME")
h_data.SetMarkerStyle(20)
h_data.Draw("E1 SAME")

leg = ROOT.TLegend(0.7, 0.7, 0.9, 0.9)
leg.AddEntry(h_dy, "DY", "f")
leg.AddEntry(h_qcd, "QCD", "f")
leg.AddEntry(h_signal, "Signal", "l")
leg.AddEntry(h_data, "Data", "lep")
leg.Draw()

latex = ROOT.TLatex()
latex.SetNDC()
latex.SetTextSize(0.04)
latex.DrawLatex(0.1, 0.92, "CMS Preliminary 54.9 fb^{-1} (13 TeV)")

c.SaveAs("stacked_plot.png")
