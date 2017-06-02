#ifndef WP_ANALYZER_H
#define WP_ANALYZER_H

///////////////////////////////////////////////////////////////////////////////
//
// wp_analyzer
//
// daniel.joseph.antrim@cern.ch
// June 2017
//
///////////////////////////////////////////////////////////////////////////////


//WPAnalyzer
#include "WPAnalyzer/wp_holder.h"

//SusyNtuple
#include "SusyNtuple/SusyNtAna.h"
#include "SusyNtuple/SusyNtTools.h"

//std/stl
#include <map>
#include <vector>

class WPAnalyzer : public SusyNtAna
{

    public :
        WPAnalyzer();
        virtual ~WPAnalyzer();

        void set_debug(int dbg) { m_dbg = dbg; }
        int dbg() { return m_dbg; }

        // sorting method
        static bool comparePt(const TLorentzVector* p1, const TLorentzVector* p2);

        // get object levets
        void getBaselineObjects(const ElectronVector& pre_ele, const MuonVector& pre_muo,
                            const JetVector& pre_jets,
            ElectronVector& electrons, MuonVector& muons, JetVector& jets);
        ElectronVector get_baseline_electrons(const ElectronVector& pre_ele);
        MuonVector get_baseline_muons(const MuonVector& pre_muo);
        JetVector get_baseline_jets(const JetVector& pre_jets);

        std::vector<ORStruct> define_overlaps();

        // set the WP for the given WP holder
        void perform_test(WPHolder holder);
        ElectronVector get_signal_electrons(ElectronVector& base_ele, Susy::ElectronId id,
                        Susy::Isolation isolation);
        MuonVector get_signal_muons(MuonVector& base_muo, Susy::MuonId id, Susy::Isolation isolation);


        // TSelector methods
        virtual void Begin(TTree *tree);
        virtual void Init(TTree *tree);
        virtual Bool_t Process(Long64_t entry);
        virtual void Terminate();

    private :
        int m_dbg;

        int m_initial_counter;
        std::map<int, int> m_test_counters;
        int m_total_tests;
        std::map<int, WPHolder> m_holder_map;
        //std::map<int

        std::vector<ORStruct> m_or_definitions;
        std::vector<Susy::ElectronId> m_ele_ids;
        std::vector<Susy::MuonId> m_muo_ids;
        std::vector<Susy::Isolation> m_lep_isos;

        void print_setup();
        void initialize_holders();
    


}; // class

#endif
