#ifndef __IN_SITU_CHARES___
#define __IN_SITU_CHARES___

#include "charm++.h"

#include "conduit.hpp"
#include "ascent.hpp"


struct CharmAscentData {
    CharmAscentData() {}
    CharmAscentData(conduit::Node& node) : m_condNode(node) {}

    void pup(PUP::er& p);

    conduit::Node m_condNode;
};


#include "insitu.decl.h"


class InSitu : public CBase_InSitu {
public:
  // Constructors & Destructors
  InSitu();
  InSitu(CkMigrateMessage *msg);

  void pup(PUP::er &p);

  // Entry methods
  void CreateInSituChares(const CharmAscentData& data, int status_ep, int status_id);
  void InSituCreated(CkArrayCreatedMsg *m);

  void ExecuteAscent(bool init_phase, int inner_chare_aid = 0);

private:
  CharmAscentData m_ascentData;
  int m_statusEp;
  int m_statusId;
  // ascent::Ascent m_ascent;
  // Domain m_Dom;
};

#endif      // #ifndef __IN_SITU_CHARES___
