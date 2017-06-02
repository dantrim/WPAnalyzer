#ifndef OR_STRUCT_H
#define OR_STRUCT_H

///////////////////////////////////////////////////////////////////////////////
//
// or_struct
//
// holds definitions of OR procedure
//
// daniel.joseph.antrim@cern.ch
// June 2017
//
///////////////////////////////////////////////////////////////////////////////


//std/stl
#include <string>

class ORStruct
{
    public :
        ORStruct();
        virtual ~ORStruct(){};

        bool j_e_bjet() { return m_j_e_bjet; }
        bool e_j_boosted() { return m_e_j_boosted; }
        bool j_m_bjet() { return m_j_m_bjet; }
        bool j_m_ptratio() { return m_j_m_ptratio; }
        bool m_j_boosted() { return m_m_j_boosted; }

        bool m_j_e_bjet;
        bool m_e_j_boosted;
        bool m_j_m_bjet;
        bool m_j_m_ptratio;
        bool m_m_j_boosted;

        void set(bool j_e_bjet, bool e_j_boosted, bool j_m_bjet, bool j_m_ptratio, bool m_j_boosted);

        std::string definition(); //print();

}; // class

#endif
