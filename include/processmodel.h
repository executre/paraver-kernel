#ifndef PROCESSMODEL_H_INCLUDED
#define PROCESSMODEL_H_INCLUDED

#include <vector>
#include <string>
#include "processmodelappl.h"

using namespace std;

class ProcessModel
{

  public:
    ProcessModel() { ready = false; }

    ProcessModel( string headerInfo );
    ~ProcessModel() {};

    bool isReady() { return ready; }

    TApplOrder totalApplications() { return applications.size(); }

  protected:
    vector<ProcessModelAppl> applications;
    bool ready;

  private:

};




#endif // PROCESSMODEL_H_INCLUDED
