#include "WPAnalyzer/or_struct.h"

//std/stl
#include <iostream>
#include <sstream>
using namespace std;

///////////////////////////////////////////////////////////////////////////////
ORStruct::ORStruct() :
    m_j_e_bjet(false),
    m_e_j_boosted(false),
    m_j_m_bjet(false),
    m_j_m_ptratio(false),
    m_m_j_boosted(false)
{
}
///////////////////////////////////////////////////////////////////////////////
void ORStruct::set(bool j_e_bjet, bool e_j_boosted, bool j_m_bjet, bool j_m_ptratio, bool m_j_boosted)
{
    m_j_e_bjet = j_e_bjet;
    m_e_j_boosted = e_j_boosted;
    m_j_m_bjet = j_m_bjet;
    m_j_m_ptratio = j_m_ptratio;
    m_m_j_boosted = m_j_boosted;
}
///////////////////////////////////////////////////////////////////////////////
std::string ORStruct::definition()
{

    stringstream sx;

    //sx << "OR: " << j_e_bjet() << "  " << e_j_boosted()
    //    << "  " << j_m_bjet() << "  " << j_m_ptratio() << "  " << m_j_boosted();

    sx << "OR: j_e_bjet " << j_e_bjet()
        << "  e_j_boosted " << e_j_boosted()
        << "  j_m_bjet " << j_m_bjet()
        << "  j_m_ptratio " << j_m_ptratio()
        << "  m_j_boosted " << m_j_boosted();

    return sx.str();
}
