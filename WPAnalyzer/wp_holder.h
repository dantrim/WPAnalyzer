#ifndef WP_HOLDER_H
#define WP_HOLDER_H

///////////////////////////////////////////////////////////////////////////////
//
// wp_holder
//
// holds the holders
//
// daniel.joseph.antrim@cern.ch
// June 2017
//
///////////////////////////////////////////////////////////////////////////////

//SusyNtuple
#include "SusyNtuple/SusyNtAna.h"
#include "SusyNtuple/SusyNtTools.h"
#include "SusyNtuple/ElectronId.h"
#include "SusyNtuple/MuonId.h"
#include "SusyNtuple/Isolation.h"

//WPAnalyzer
#include "WPAnalyzer/or_struct.h"


class WPHolder
{
    public :
        WPHolder();
        virtual ~WPHolder();

        void set_index(int idx) { m_wp_idx = idx; }
        int idx() { return m_wp_idx; }

        void set_definitions(Susy::ElectronId eid, Susy::MuonId mid, Susy::Isolation iso, ORStruct ordef);

        void set_objects(ElectronVector electrons, MuonVector muons,
                    JetVector jets, JetVector bjets);
        void clear_objects();

        ElectronVector electrons() { return m_electrons; }
        MuonVector muons() { return m_muons; }
        JetVector jets() { return m_jets; }
        JetVector bjets() { return m_bjets; }

        // electron properties
        Susy::ElectronId base_ele_id() { return m_baseline_electron_id; }
        Susy::ElectronId sig_ele_id() { return m_signal_electron_id; }
        Susy::Isolation ele_iso() { return m_ele_isolation; }

        // muon properties
        Susy::MuonId base_muo_id() { return m_baseline_muon_id; }
        Susy::MuonId sig_muo_id() { return m_signal_muon_id; }
        Susy::Isolation muo_iso() { return m_muo_isolation; }

        // OR definition
        ORStruct overlap() { return m_or; }
        

        std::string definition();

    private :

        int m_wp_idx;

        ElectronVector  m_electrons;
        MuonVector      m_muons;
        JetVector       m_jets;
        JetVector       m_bjets;

        // electron properties
        Susy::ElectronId m_baseline_electron_id;
        Susy::ElectronId m_signal_electron_id;
        Susy::Isolation m_ele_isolation;

        // muon properties
        Susy::MuonId m_baseline_muon_id;
        Susy::MuonId m_signal_muon_id;
        Susy::Isolation m_muo_isolation;

        // OR definition
        ORStruct m_or;




}; // class

#endif
