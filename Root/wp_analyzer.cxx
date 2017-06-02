#include "WPAnalyzer/wp_analyzer.h"

//std/stl
#include <iostream>
using namespace std;

//SusyNtuple
#include "SusyNtuple/OverlapTools.h"

///////////////////////////////////////////////////////////////////////////////
WPAnalyzer::WPAnalyzer() :
    m_dbg(0),
    m_initial_counter(0),
    m_total_tests(0)
{
    cout << "WPAnalyzer" << endl;
    m_holder_map.clear();

    m_or_definitions.clear();
    m_or_definitions = define_overlaps();

    m_ele_ids.clear();
    m_ele_ids.push_back(Susy::ElectronId::MediumLLH);
    m_ele_ids.push_back(Susy::ElectronId::TightLLH);

    m_muo_ids.clear();
    m_muo_ids.push_back(Susy::MuonId::Medium);
    m_muo_ids.push_back(Susy::MuonId::Tight);

    m_lep_isos.clear();
    m_lep_isos.push_back(Susy::Isolation::FixedCutTightTrackOnly);
    m_lep_isos.push_back(Susy::Isolation::Loose);
    m_lep_isos.push_back(Susy::Isolation::LooseTrackOnly);
    m_lep_isos.push_back(Susy::Isolation::Gradient);
    m_lep_isos.push_back(Susy::Isolation::GradientLoose);

    print_setup();

    initialize_holders();
}
///////////////////////////////////////////////////////////////////////////////
void WPAnalyzer::initialize_holders()
{
    int holder_idx = 0;
    for(int iele = 0; iele < (int)m_ele_ids.size(); iele++) {
        for(int imuo = 0; imuo < (int)m_muo_ids.size(); imuo++) {
            for(int iiso = 0; iiso < (int)m_lep_isos.size(); iiso++) {
                for(int ior = 0; ior < (int)m_or_definitions.size(); ior++) {
                    WPHolder holder;
                    holder.set_definitions(m_ele_ids.at(iele), m_muo_ids.at(imuo),
                        m_lep_isos.at(iiso), m_or_definitions.at(ior)); 
                    holder.set_index(holder_idx);
                    m_holder_map[holder_idx] = holder;
                    m_test_counters[holder_idx] = 0;
                    holder_idx++;
                } // overlap definitions
            } // lepton isolation
        } // muon id
    } // electron id

    m_total_tests = holder_idx;

    cout << "------------------------------------------------------------------" << endl;
    cout << "WPAnalyzer::initialize_holders    " << holder_idx << " total WP holders" << endl;
    cout << "------------------------------------------------------------------" << endl;
}
///////////////////////////////////////////////////////////////////////////////
void WPAnalyzer::print_setup()
{
    int total = 0;
    cout <<"----------------------------------------------------------------" << endl;
    cout << " WPAnalyzer configuration " << endl;
    cout << " - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - " << endl;
    cout << " Lepton Isolation " << endl;
    for(int i = 0; i < (int)m_lep_isos.size(); i++) {
    cout << "  " << i << "  " << Susy::Isolation2str(m_lep_isos.at(i)) << endl;
    total++;
    } 
    cout << " - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - " << endl;
    cout << " Electron ID " << endl;
    for(int i = 0; i < (int)m_ele_ids.size(); i++) {
    cout << "  " << i << "  " << Susy::ElectronId2str(m_ele_ids.at(i)) << endl;
    total++;
    }
    cout << " - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - " << endl;
    cout << " Muon ID " << endl;
    for(int i = 0; i < (int)m_muo_ids.size(); i++) {
    cout << "  " << i << "  " << Susy::MuonId2str(m_muo_ids.at(i)) << endl;
    total++;
    }
    cout << " - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - " << endl;
    cout << " Overlap Removal " << endl;
    for(int i = 0; i < (int)m_or_definitions.size(); i++) {
    cout << "  " << i << "  " << m_or_definitions.at(i).definition() << endl;
    total++;
    }
    cout << " - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - " << endl;
    cout << " - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - " << endl;
    cout << "  Total WP combinations: " << total << endl;
    cout <<"----------------------------------------------------------------" << endl;
}
///////////////////////////////////////////////////////////////////////////////
WPAnalyzer::~WPAnalyzer()
{
    cout << "~WPAnalyzer" << endl;
}
///////////////////////////////////////////////////////////////////////////////
void WPAnalyzer::Begin(TTree* /*tree*/)
{
    SusyNtAna::Begin(0);
    if(dbg()) cout << "WPAnalyzer::Begin" << endl;
}
///////////////////////////////////////////////////////////////////////////////
void WPAnalyzer::Init(TTree* tree)
{
    SusyNtAna::Init(tree);
    if(dbg()) cout << "WPAnalyzer::Init" << endl;


}
///////////////////////////////////////////////////////////////////////////////
Bool_t WPAnalyzer::Process(Long64_t entry)
{
    GetEntry(entry);
    SusyNtAna::clearObjects();

    m_chainEntry++;
    if(dbg() || m_chainEntry%100==0) {
        cout << "**** Process entry " << setw(6) << m_chainEntry
                << " run " << setw(6) << nt.evt()->run
                << " event " << setw(7) << nt.evt()->eventNumber << " ****"
                << endl;
    }

    // pre-objects common to all
    SusyNtAna::clearObjects();
    nttools().getPreObjects(&nt, Susy::NtSys::NOM, m_preElectrons, m_preMuons,
                m_preJets, m_preTaus, m_prePhotons);

    ElectronVector base_electrons;
    MuonVector base_muons;
    JetVector base_jets;

    // assume same baseline definition for all tests
    WPAnalyzer::getBaselineObjects(m_preElectrons, m_preMuons, m_preJets,
                    base_electrons, base_muons, base_jets);

    int n_base_electrons = base_electrons.size();
    int n_base_muons = base_muons.size();
    int n_base_leptons = (n_base_electrons + n_base_muons);

    // only process events with >=2 baseline leptons in the "denominator"
    if( !(n_base_leptons >= 2) ) { return kTRUE; }
    m_initial_counter++;


    // now get the #'s for each WP test
    for(int itest = 0; itest < m_total_tests; itest++) {
        WPHolder holder = m_holder_map[itest];

        ElectronVector test_ele = base_electrons;
        MuonVector test_muo = base_muons;
        JetVector test_jets = base_jets;

        holder.set_objects(test_ele, test_muo, test_jets, test_jets);

        perform_test(holder);
    }


    return kTRUE;
}
///////////////////////////////////////////////////////////////////////////////
vector<ORStruct> WPAnalyzer::define_overlaps()
{
    vector<ORStruct> or_vector;
    for(int a = 0; a < (int)2; a++) {
        for(int b = 0; b < (int)2; b++) {
            for(int c = 0; c < (int)2; c++) {
                for(int d = 0; d < (int)2; d++) {
                    for(int e = 0; e < (int)2; e++) {
                        ORStruct ors;
                        ors.set(a, b, c, d, e);
                        or_vector.push_back(ors);
                    } // e
                } // d
            } // c
        } // b
    } // a

    //for(int i = 0; i < (int)or_vector.size(); i++) {
    //    cout << "[" << i << "] " << or_vector.at(i).definition() << endl;
    //}

    return or_vector;
}
///////////////////////////////////////////////////////////////////////////////
bool WPAnalyzer::comparePt(const TLorentzVector* p1, const TLorentzVector* p2)
{
    return p1->Pt() > p2->Pt();
}
///////////////////////////////////////////////////////////////////////////////
void WPAnalyzer::getBaselineObjects(const ElectronVector& pre_ele, const MuonVector& pre_muo,
    const JetVector& pre_jets, ElectronVector& base_ele, MuonVector& base_muo,
    JetVector& base_jets)
{
    base_ele = get_baseline_electrons(pre_ele);
    base_muo = get_baseline_muons(pre_muo);
    base_jets = get_baseline_jets(pre_jets);
}
///////////////////////////////////////////////////////////////////////////////
ElectronVector WPAnalyzer::get_baseline_electrons(const ElectronVector& pre_ele)
{
    ElectronVector elecs;
    for(uint i = 0; i < pre_ele.size(); ++i) {
        Electron* e = pre_ele.at(i);

        bool pass_baseline_id = (e->looseLLHBLayer==1);
        bool passOQ = (e->passOQBadClusElectron==1);
        bool pass_pt = (e->Pt() > 10.0);
        bool pass_eta = (std::abs(e->clusEta) < 2.47);

        if(pass_baseline_id && passOQ && pass_pt && pass_eta)
            elecs.push_back(e);
    } // i

    std::sort(elecs.begin(), elecs.end(), comparePt); 

    return elecs;
}
///////////////////////////////////////////////////////////////////////////////
MuonVector WPAnalyzer::get_baseline_muons(const MuonVector& pre_muo)
{
    MuonVector muons;
    for(uint i = 0; i < pre_muo.size(); ++i) {
        Muon* m = pre_muo.at(i);

        bool pass_id = (m->medium==1);
        bool pass_pt = (m->Pt() > 10.0);
        bool pass_eta = (fabs(m->Eta()) < 2.4);

        if(pass_id && pass_pt && pass_eta)
            muons.push_back(m);
    } // i

    std::sort(muons.begin(), muons.end(), comparePt);

    return muons;
}
///////////////////////////////////////////////////////////////////////////////
JetVector WPAnalyzer::get_baseline_jets(const JetVector& pre_jets)
{
    JetVector jets;
    for(uint i = 0; i < pre_jets.size(); ++i) {
        Jet* j = pre_jets.at(i);

        bool pass_pt = (j->Pt() > 20.0);

        if(pass_pt)
            jets.push_back(j);
    } // i

    std::sort(jets.begin(), jets.end(), comparePt);

    return jets;
}
///////////////////////////////////////////////////////////////////////////////
void WPAnalyzer::Terminate()
{
    SusyNtAna::Terminate();
    if(dbg()) cout << "WPAnalyzer::Terminate" << endl;

    for(int i = 0; i < m_total_tests; i++) {
        cout << "[" << i << "]  " << ((float)(m_test_counters[i])* 1.0 / m_initial_counter)
                << "   " << m_holder_map[i].definition() << endl;
    }
}
///////////////////////////////////////////////////////////////////////////////
void WPAnalyzer::perform_test(WPHolder holder)
{
    ElectronVector electrons = holder.electrons();
    MuonVector muons = holder.muons();
    JetVector jets = holder.jets();  

    //////////////////////////////////////////////////////////////////////
    // OVERLAP REMOVAL
    //////////////////////////////////////////////////////////////////////

    // electron-muon overlaps have no configurables
    nttools().overlapTool().m_e_overlap(muons, electrons);
    nttools().overlapTool().e_m_overlap(electrons, muons);

    // electron-jet overlaps
    bool do_bjet = holder.overlap().j_e_bjet();
    nttools().overlapTool().j_e_overlap(electrons, jets, 0.2, do_bjet);
    bool do_boosted = holder.overlap().e_j_boosted();
    bool do_jvt = true;
    nttools().overlapTool().e_j_overlap(electrons, jets, 0.4, do_boosted, do_jvt);

    // muon-jet overlaps
    do_bjet = holder.overlap().j_m_bjet();
    do_boosted = holder.overlap().m_j_boosted();
    bool do_ptratio = holder.overlap().j_m_ptratio();
    nttools().overlapTool().j_m_overlap(jets, muons, 0.2, do_bjet, true, do_ptratio);
    nttools().overlapTool().m_j_overlap(muons, jets, 0.4, do_boosted, do_jvt);

    //////////////////////////////////////////////////////////////////////
    // SIGNAL CRITERIA
    //////////////////////////////////////////////////////////////////////
    MuonVector signal_muons;
    ElectronVector signal_electrons;

    signal_muons = get_signal_muons(muons, holder.sig_muo_id(), holder.muo_iso());
    signal_electrons = get_signal_electrons(electrons, holder.sig_ele_id(), holder.ele_iso());
    

    int n_leptons = (signal_muons.size() + signal_electrons.size());
    if(n_leptons == 2) {
        m_test_counters[holder.idx()] += 1;
    }
    

}
///////////////////////////////////////////////////////////////////////////////
ElectronVector WPAnalyzer::get_signal_electrons(ElectronVector& base_electrons,
        Susy::ElectronId id, Susy::Isolation isolation)
{
    ElectronVector electrons;
    for(int i = 0; i < (int)base_electrons.size(); i++) {
        Electron* e = base_electrons.at(i);

        // ID WP
        bool pass_id = false;
        if(id==Susy::ElectronId::TightLLH) {
            pass_id = (e->tightLLH);
        }
        else if(id==Susy::ElectronId::MediumLLH) {
            pass_id = (e->mediumLLH);
        }
        else {
            cout << "WPAnalyzer::get_signal_electrons    Requested non-implemented signal ID!" << endl;
            exit(1);
        }

        // ISOLATION WP
        bool pass_iso = false;
        if(isolation==Susy::Isolation::GradientLoose) {
            pass_iso = (e->isoGradientLoose);
        }
        else if(isolation==Susy::Isolation::Gradient) {
            pass_iso = (e->isoGradient);
        }
        else if(isolation==Susy::Isolation::LooseTrackOnly) {
            pass_iso = (e->isoLooseTrackOnly);
        }
        else if(isolation==Susy::Isolation::Loose) {
            pass_iso = (e->isoLoose);
        }
        else if(isolation==Susy::Isolation::FixedCutTightTrackOnly) {
            pass_iso = (e->isoFixedCutTightTrackOnly);
        }
        else {
            cout << "WPAnalyzer::get_signal_electrons    Requested non-implemented isolation WP!" << endl;
            exit(1);
        }

        // IMPACT PARAMETER
        bool pass_d0 = ( std::abs(e->d0sigBSCorr) < 5.0 );
        bool pass_z0 = ( std::abs(e->z0SinTheta()) < 0.5 );
        bool pass_ip = (pass_d0 && pass_z0);


        if(pass_ip && pass_iso && pass_id)
            electrons.push_back(e);

    } // i
    std::sort(electrons.begin(), electrons.end(), comparePt); 

    return electrons;
}
///////////////////////////////////////////////////////////////////////////////
MuonVector WPAnalyzer::get_signal_muons(MuonVector& base_muons, Susy::MuonId id, Susy::Isolation isolation)
{

    MuonVector muons;
    for(int i = 0; i < (int)base_muons.size(); i++) {
        Muon* e = base_muons.at(i);

        // ID WP
        bool pass_id = false;
        if(id==Susy::MuonId::Tight) {
            pass_id = (e->tight);
        }
        else if(id==Susy::MuonId::Medium) {
            pass_id = (e->medium);
        }
        else {
            cout << "WPAnalyzer::get_signal_muons    Requested non-implemented signal ID!" << endl;
            exit(1);
        }

        // ISOLATION WP
        bool pass_iso = false;
        if(isolation==Susy::Isolation::GradientLoose) {
            pass_iso = (e->isoGradientLoose);
        }
        else if(isolation==Susy::Isolation::Gradient) {
            pass_iso = (e->isoGradient);
        }
        else if(isolation==Susy::Isolation::LooseTrackOnly) {
            pass_iso = (e->isoLooseTrackOnly);
        }
        else if(isolation==Susy::Isolation::Loose) {
            pass_iso = (e->isoLoose);
        }
        else if(isolation==Susy::Isolation::FixedCutTightTrackOnly) {
            pass_iso = (e->isoFixedCutTightTrackOnly);
        }
        else {
            cout << "WPAnalyzer::get_signal_muons    Requested non-implemented isolation WP!" << endl;
            exit(1);
        }

        // IMPACT PARAMETER
        bool pass_d0 = ( std::abs(e->d0sigBSCorr) < 3.0 );
        bool pass_z0 = ( std::abs(e->z0SinTheta()) < 0.5 );
        bool pass_ip = (pass_d0 && pass_z0);


        if(pass_ip && pass_iso && pass_id)
            muons.push_back(e);

    } // i
    std::sort(muons.begin(), muons.end(), comparePt); 

    return muons;

}
///////////////////////////////////////////////////////////////////////////////
