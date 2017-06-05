#!/usr/bin/env python

from optparse import OptionParser
import os

import ROOT as r
r.PyConfig.IgnoreCommandLineOptions = True
r.gROOT.SetBatch(True)
r.gStyle.SetOptStat(False)

import sys

class OR() :
    def __init__(self, je_bjet, ej_boosted, jm_bjet, jm_ptratio, mj_boosted) :
        self.je_bjet = je_bjet
        self.ej_boosted = ej_boosted
        self.jm_bjet = jm_bjet
        self.jm_ptratio = jm_ptratio
        self.mj_boosted = mj_boosted

    def j_e_bjet(self) :
        return self.je_bjet
    def e_j_boosted(self) :
        return self.ej_boosted
    def j_m_bjet(self) :
        return self.jm_bjet
    def j_m_ptratio(self) :
        return self.jm_ptratio
    def m_j_boosted(self) :
        return self.mj_boosted

class WP() :
    def __init__(self, idx) :
        self.idx = idx

        self.ele_id = ""
        self.ele_iso = ""

        self.muo_id = ""
        self.muo_iso = ""

        self.overlap = None

        self.default = False

        self.eff = -1.0
        self.eff_ratio = -1.0

    def is_default(self) :
        return self.default

    def OR(self) :
        return self.overlap

    def setOR(self, ordef) :
        self.overlap = ordef

    def description(self) :
        x = "WP[%d] ele(%s,%s) muo(%s,%s) OR(je_bjet, ej_boosted, jm_bjet, jm_ptratio, mj_boosted)=("%(self.idx, self.ele_id, self.ele_iso, self.muo_id, self.muo_iso)
        x += "%d, %d, %d, %d, %d)"%(self.OR().j_e_bjet(), self.OR().e_j_boosted(), self.OR().j_m_bjet(), self.OR().j_m_ptratio(), self.OR().m_j_boosted())

        return x

def wp_is_default(working_point) :

    if working_point.ele_id=="medium" and \
        working_point.ele_iso=="GradientLoose" and \
        working_point.muo_id=="medium" and \
        working_point.muo_iso=="GradientLoose" and \
        working_point.OR().j_e_bjet()==1 and \
        working_point.OR().e_j_boosted()==0 and \
        working_point.OR().j_m_bjet()==1 and \
        working_point.OR().j_m_ptratio()==0 and \
        working_point.OR().m_j_boosted()==1 :
        return True
    else :
        return False

def wp_is_interesting(working_point) :

    if working_point.ele_id=="medium" and \
        working_point.ele_iso=="LooseTrackOnly" and \
        working_point.muo_id=="medium" and \
        working_point.muo_iso=="LooseTrackOnly" and \
        working_point.OR().j_e_bjet()==0 and \
        working_point.OR().e_j_boosted()==0 and \
        working_point.OR().j_m_bjet()==0 and \
        working_point.OR().j_m_ptratio()==0 and \
        working_point.OR().m_j_boosted()==0 :
        return True
    else :
        return False

def test_OR(wp) :

    #if wp.OR().j_e_bjet() == 0 and wp.OR().e_j_boosted() == 0 and wp.OR().j_m_bjet() == 1:
    if wp.OR().j_e_bjet() == 0 :
        return True
    else :
        return False

   # if wp.OR().j_e_bjet()==     0 and \
   #     wp.OR().e_j_boosted==   0 and \
   #     wp.OR().j_m_bjet()==    0 and \
   #     wp.OR().j_m_ptratio()== 0 and \
   #     wp.OR().m_j_boosted()== 0 :
   #     return True
   # else :
   #     return False

def is_good_line(line) :
    if "HOLDER DEF" in line :
        return True
    else :
        return False
    

def get_working_points(wpfile) :

    out_wps = []
    default_eff = -1

    found_default = False

    lines = open(wpfile).readlines()
    for line in lines :
        if not line : continue
        line = line.strip()
        if not is_good_line(line) : continue

        # idx
        idx = int(line.split()[0].replace("[","").replace("]",""))

        # eff
        eff = float(line.split()[1])

        # electron
        electron = line.split()[5]
        ele_id = electron.split(",")[0].replace("(","").replace("LLH","").lower()
        ele_iso = electron.split(",")[1].replace(")","")

        # muon
        muon = line.split()[6]
        muo_id = muon.split(",")[0].replace("(","").lower()
        muo_iso = muon.split(",")[1].replace(")","")

        # OR
        overlap = line.split()[7:]
        #['OR:', 'j_e_bjet', '1', 'e_j_boosted', '0', 'j_m_bjet', '1', 'j_m_ptratio', '1', 'm_j_boosted', '1'] 
        j_e_bjet = int(overlap[2])
        e_j_boosted = int(overlap[4])
        j_m_bjet = int(overlap[6])
        j_m_ptratio = int(overlap[8])
        m_j_boosted = int(overlap[10])

        ordef = OR(j_e_bjet, e_j_boosted, j_m_bjet, j_m_ptratio, m_j_boosted)

        # setup the WP
        wp = WP(idx)

        wp.ele_id = ele_id
        wp.ele_iso = ele_iso
        wp.muo_id = muo_id
        wp.muo_iso = muo_iso
        wp.setOR(ordef)
        wp.eff = eff

        if wp_is_default(wp) :
            wp.default = True
            default_eff = wp.eff
            if found_default :
                print "FOUND MORE THAN ONE DEFAULT WORKING POINT!!"
            found_default = True

        out_wps.append(wp)

    return out_wps, default_eff

def make_wp_plot(background_wps, signal_wps, bounds) :

    c = r.TCanvas("c", "", 800, 800)
    c.cd()
    c.SetGrid(1,1)
    c.Update()

    axis = r.TH2F("axis", "", 100, 0.97*bounds[2], 1.03*bounds[3], 100, 0.97*bounds[0], 1.03*bounds[1])
    xax = axis.GetXaxis()
    xax.SetTitle("(Relative) Signal Efficiency")
    xax.SetTitleFont(42)
    xax.SetLabelFont(42)
    yax = axis.GetYaxis()
    yax.SetTitle("(Relative) Background Efficiency")
    yax.SetTitleFont(42)
    yax.SetLabelFont(42)

    g = r.TGraph(len(background_wps))
    g.SetMarkerStyle(20)
    g.SetMarkerSize(0.5*g.GetMarkerSize())

    gdef = r.TGraph(1)
    gdef.SetMarkerStyle(29)
    gdef.SetMarkerColor(r.kRed)
    gdef.SetMarkerSize(2*gdef.GetMarkerSize())

    gint = r.TGraph(1)
    gint.SetMarkerStyle(24)
    gint.SetMarkerColor(r.kBlack)
    gint.SetMarkerSize(1.5*gint.GetMarkerSize())

    gtest = r.TGraph(0)
    gtest.SetMarkerStyle(20)
    gtest.SetMarkerColor(r.kMagenta)
    gtest.SetMarkerSize(0.5*gtest.GetMarkerSize())

    g_map = {}
    ids = ["medium_medium", "medium_tight", "tight_medium", "tight_tight"]
    isos = ["GradientLoose", "Gradient", "Loose", "LooseTrackOnly", "FixedCutTightTrackOnly"]

    markers = {}
    for lep_id in ids :
        for lep_iso in isos :
            key = "%s_%s"%(lep_id, lep_iso)

            color = None
            marker = None

            if lep_id == "medium_medium" :
                color = 4
            elif lep_id == "medium_tight" :
                color = 3
            elif lep_id == "tight_medium" :
                color = 6
            elif lep_id == "tight_tight" :
                color = 7

            if lep_iso == "GradientLoose" :
                marker = 20
            elif lep_iso == "Gradient" :
                marker = 22
            elif lep_iso == "Loose" :
                marker = 23
            elif lep_iso == "LooseTrackOnly" :
                marker = 21
            elif lep_iso == "FixedCutTightTrackOnly" :
                marker = 34 

            markers[key] = [color, marker]

    for lep_id in ids :
        for lep_iso in isos :
            key = "%s_%s"%(lep_id, lep_iso)
            g = r.TGraph(0)
            g.SetMarkerStyle(markers[key][1])
            g.SetMarkerColor(markers[key][0])
            g.SetMarkerSize(0.75*g.GetMarkerSize())
            g_map[key] = g
            

    wp_map_sig = {}
    wp_map_bkg = {}

    for wp in background_wps :
        wp_map_bkg[wp.idx] = wp
    for wp in signal_wps :
        wp_map_sig[wp.idx] = wp

    n_over = 0

    idx_over_map = {}
    

    for iwp in xrange(len(background_wps)) :

        sig_wp = wp_map_sig[iwp]
        bkg_wp = wp_map_bkg[iwp]

        x = sig_wp.eff_ratio
        y = bkg_wp.eff_ratio

        if sig_wp.is_default() and bkg_wp.is_default() :
            gdef.SetPoint(0, x, y)

        if wp_is_interesting(sig_wp) :
            gint.SetPoint(0, x, y)

        key = "%s_%s_%s"%(sig_wp.ele_id, sig_wp.muo_id, sig_wp.ele_iso)
        g_map[key].SetPoint(g_map[key].GetN(), x, y)

        #if sig_wp.ele_id=="medium" and sig_wp.muo_id=="medium" and sig_wp.ele_iso=="LooseTrackOnly" and test_OR(sig_wp) :
        ##if sig_wp.ele_id=="medium" and sig_wp.muo_id=="medium" and test_OR(sig_wp) :
        #    gtest.SetPoint(gtest.GetN(), x, y)

        #g.SetPoint(iwp, x, y)

        if x > 1.0 :
            n_over += 1
            if float(x/y) > 1.00 :
                print "[%d] relative eff (sig, bkg) = (%.3f, %.3f) (%.3f): %s"%(n_over, x, y, float(x/y), sig_wp.description())
                idx_over_map[sig_wp.idx] = float(x/y)
            
            #print "IDX: %d = (%.2f, %.2f)"%(sig_wp.idx, x, y)

    ############### make legend

    # entries for color (lepton id)

    gl0 = r.TGraph(0)
    gl0.SetLineColor(4)
    gl0.SetLineWidth(5*gl0.GetLineWidth())
    gl0.SetMarkerStyle(21)

    gl1 = r.TGraph(0)
    gl1.SetLineColor(3)
    gl1.SetLineWidth(5*gl1.GetLineWidth())
    gl1.SetMarkerStyle(21)

    gl2 = r.TGraph(0)
    gl2.SetLineColor(6)
    gl2.SetLineWidth(5*gl2.GetLineWidth())
    gl2.SetMarkerStyle(21)

    gl3 = r.TGraph(0)
    gl3.SetLineColor(7)
    gl3.SetLineWidth(5*gl3.GetLineWidth())
    gl3.SetMarkerStyle(21)

    leg = r.TLegend(0.34, 0.73, 0.54, 0.88)
    leg.AddEntry(gl0, "(med, med)", "l")
    leg.AddEntry(gl1, "(med, tight)", "l")
    leg.AddEntry(gl2, "(tight, med)", "l")
    leg.AddEntry(gl3, "(tight, tight)", "l")

    # entries for isolation (shape)

    # GradientLoose

    gs0 = r.TGraph(0)
    gs0.SetMarkerSize(2*gs0.GetMarkerSize())
    gs0.SetMarkerStyle(20)

    # Gradient
    gs1 = r.TGraph(0)
    gs1.SetMarkerSize(2*gs1.GetMarkerSize())
    gs1.SetMarkerStyle(22)

    # Loose
    gs2 = r.TGraph(0)
    gs2.SetMarkerSize(2*gs2.GetMarkerSize())
    gs2.SetMarkerStyle(23)

    # LooseTrackOnly
    gs3 = r.TGraph(0)
    gs3.SetMarkerSize(2*gs3.GetMarkerSize())
    gs3.SetMarkerStyle(21)

    # FixedCutTightTrackOnly
    gs4 = r.TGraph(0)
    gs4.SetMarkerSize(2*gs4.GetMarkerSize())
    gs4.SetMarkerStyle(34)

    leg2 = r.TLegend(0.12, 0.73, 0.32, 0.88)
    leg2.AddEntry(gs0, "GradientLoose", "p")
    leg2.AddEntry(gs1, "Gradient", "p")
    leg2.AddEntry(gs2, "Loose", "p")
    leg2.AddEntry(gs3, "LooseTrackOnly", "p")
    leg2.AddEntry(gs4, "FixedCutTightTrackOnly", "p")

    

    c.cd()
    axis.Draw("axis")
    c.Update()
    for key in g_map.keys() :
        g_map[key].Draw("P same")
    
    #g.Draw("P same")
    c.Update()
    gdef.Draw("P same")
    c.Update()
    gint.Draw("P same")
    c.Update()
    #gtest.Draw("P same")
    #c.Update()

    leg.Draw()
    c.Update()
    leg2.Draw()
    c.Update()

    c.SaveAs("wp_ana_curve.eps")

    ## S/B lot

    c = r.TCanvas("c_sob", "", 800, 400)
    c.cd()

    n_bins = len(idx_over_map)
    if len(idx_over_map.keys())==0 :
        return
    maxx = max(idx_over_map.keys())
    minx = min(idx_over_map.keys())
    n_bins = maxx - minx
    hsb = r.TH1F("hsb", "", n_bins, minx, maxx)
    hsb.SetLineColor(r.kBlack)
    hsb.GetXaxis().SetTitle("WP index")
    hsb.GetXaxis().SetTitleFont(42)
    hsb.GetXaxis().SetLabelFont(42)
    hsb.GetYaxis().SetTitle("(Relative) S/B")
    hsb.GetYaxis().SetTitleFont(42)
    hsb.GetYaxis().SetLabelFont(42)

    miny = 4
    for iidx, sob in idx_over_map.iteritems() :
        hsb.SetBinContent(iidx+1, sob)
        if sob < miny : miny = sob
    miny = 0.95 * miny
    hsb.SetMinimum(miny)

    c.cd()
    hsb.Draw("hist")

    c.SaveAs("wp_ana_sob.eps")

def main() :

    parser = OptionParser()
    parser.add_option("--background-file", dest="background_file", default="./run_ttbar.txt")
    parser.add_option("--signal-file", dest="signal_file", default="./run.txt")
    parser.add_option("-s", "--signal-name", dest="signal_name", default="hh")
    parser.add_option("-b", "--background-name", dest="background_name", default="ttbar")
    (options, args) = parser.parse_args()
    background_file = options.background_file
    signal_file = options.signal_file
    background_name = options.background_name
    signal_name = options.signal_name

    test_wp = WP(4)
    test_wp.ele_id = "medium"
    test_wp.ele_iso = "GradientLoose"
    test_wp.muo_id = "medium"
    test_wp.muo_iso = "GradientLoose"
    test_or = OR(1,0,1,0,1)
    test_wp.setOR(test_or)

    background_wps, default_bkg_eff = get_working_points(background_file)
    signal_wps, default_sig_eff = get_working_points(signal_file)


    bkg_ratios = []
    for wp in background_wps :
        x = wp.eff * 1.0 / default_bkg_eff
        wp.eff_ratio = x
        bkg_ratios.append(x)

    sig_ratios = []
    for wp in signal_wps :
        x = wp.eff * 1.0 / default_sig_eff
        wp.eff_ratio = x
        sig_ratios.append(x)

    #for wp in background_wps :
    #    if wp.is_default() :
    #        print "DEFAULT SIG EFF: %.2f and RATIO %.2f"%(wp.eff, wp.eff_ratio)
    #sys.exit()

    bounds = [min(bkg_ratios), max(bkg_ratios), min(sig_ratios), max(sig_ratios)]

    #print "max %.2f, min %.2f :: max %.2f, min %.2f"%(max(bkg_ratios), min(bkg_ratios), max(sig_ratios), min(sig_ratios))

    make_wp_plot(background_wps, signal_wps, bounds)
        


#___________________________________________________________________________
if __name__ == "__main__" :
    main()
