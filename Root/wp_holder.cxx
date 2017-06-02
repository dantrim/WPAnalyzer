#include "WPAnalyzer/wp_holder.h"

#include <sstream>
#include <iostream>
using namespace std;

///////////////////////////////////////////////////////////////////////////////
WPHolder::WPHolder() :
    m_wp_idx(-1),
    m_baseline_electron_id(Susy::ElectronId::LooseLLH),
    m_signal_electron_id(Susy::ElectronId::MediumLLH),
    m_ele_isolation(Susy::Isolation::GradientLoose),
    m_baseline_muon_id(Susy::MuonId::Medium),
    m_signal_muon_id(Susy::MuonId::Medium),
    m_muo_isolation(Susy::Isolation::GradientLoose)
{
    clear_objects();
}
///////////////////////////////////////////////////////////////////////////////
void WPHolder::set_definitions(Susy::ElectronId eid, Susy::MuonId mid, Susy::Isolation iso, ORStruct ordef)
{
    m_signal_electron_id = eid;
    m_ele_isolation = iso;

    m_signal_muon_id = mid;
    m_muo_isolation = iso;

    m_or = ordef;
}
///////////////////////////////////////////////////////////////////////////////
void WPHolder::set_objects(ElectronVector electrons, MuonVector muons,
        JetVector jets, JetVector bjets)
{
    clear_objects();
    m_electrons = electrons;
    m_muons = muons;
    m_jets = jets;
    m_bjets = bjets;
}
///////////////////////////////////////////////////////////////////////////////
void WPHolder::clear_objects()
{
    m_electrons.clear();
    m_muons.clear();
    m_jets.clear();
    m_bjets.clear();
}
///////////////////////////////////////////////////////////////////////////////
WPHolder::~WPHolder()
{

}
///////////////////////////////////////////////////////////////////////////////
std::string WPHolder::definition()
{
    stringstream sx;
    sx << "HOLDER DEF [" << m_wp_idx << "] "
            << "(" << Susy::ElectronId2str(sig_ele_id()) << ","
            << Susy::Isolation2str(ele_iso()) << ")  ("
            << Susy::MuonId2str(sig_muo_id()) << ","
            << Susy::Isolation2str(muo_iso()) << ")   "
            << m_or.definition();
    return sx.str();

}
