//std/stl
#include <string>
#include <iostream>

//ROOT
#include "TChain.h"

//SusyNtuple
#include "SusyNtuple/ChainHelper.h"
#include "SusyNtuple/string_utils.h"

//WPAnalyzer
#include "WPAnalyzer/wp_analyzer.h"

using namespace std;


void help()
{
    cout << "--------------------------------------------------" << endl;
    cout << " WP Analyzer " << endl;
    cout << endl;
    cout << " Options: " << endl;
    cout << "  -i       input (file, list, or dir)" << endl;
    cout << "  -n       number of events to process (-1 for all, default: -1)" << endl;
    cout << "  -d       debut level (default: 0)" << endl;
    cout << "--------------------------------------------------" << endl;
}


int main(int argc, char** argv)
{

    int dbg = 0;
    int n_events = -1;
    string input = "";

    // read inputs
    for(int i = 1; i < argc; i++) {
        if      (strcmp(argv[i], "-i") == 0)        { input = argv[++i]; }
        else if (strcmp(argv[i], "-d") == 0)        { dbg = atoi(argv[++i]); }
        else if (strcmp(argv[i], "-n") == 0)        { n_events = atoi(argv[++i]); }
        else {
            help();
            return 1;
        }
    }

    if(input.empty()) {
        cout << "You must specify an input" << endl;
        return 1;
    }

    TChain* chain = new TChain("susyNt");
    ChainHelper::addInput(chain, input, dbg>0);
    Long64_t nEntries = chain->GetEntries();
    chain->ls();

    // build the analyzer
    WPAnalyzer* analyzer = new WPAnalyzer();
    analyzer->set_debug(dbg);
    analyzer->nttools().setAnaType(AnalysisType::Ana_Stop2L); // not needed since in the loop we build up our objects
    if(n_events < 0) n_events = nEntries;
    cout << endl;
    cout << "Total entries          : " << nEntries << endl;
    cout << "Entries to process     : " << n_events << endl;
    if(n_events > 0) chain->Process(analyzer, "", n_events);

    cout << endl;
    cout << "WPAnalyzer job done" << endl;

    delete chain;

    return 0;
}
