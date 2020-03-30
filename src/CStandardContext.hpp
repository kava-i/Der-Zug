#ifndef CSTANDARDCONTEXT_H
#define CSTANDARDCONTEXT_H

#include "CContext.hpp"
#include "CDialogContext.hpp"


class CStandardContext : public CContext
{
public:
    CStandardContext();

    void h_find(string&, CPlayer*);

    //Handler
    void h_show             (string& sIdentiier, CPlayer* p);
    void h_look             (string& sIdentiier, CPlayer* p);
    void h_take             (string& sIdentiier, CPlayer* p);
    void h_consume          (string& sIdentiier, CPlayer* p);
    void h_equipe           (string& sIdentiier, CPlayer* p);
    void h_dequipe          (string& sIdentiier, CPlayer* p);
    void h_examine          (string& sIdentiier, CPlayer* p);
    void h_goTo             (string& sIdentiier, CPlayer* p);
    void h_startDialog      (string& sIdentiier, CPlayer* p);
    void h_changeMode       (string& sIdentiier, CPlayer* p);

    //Rooms
    void h_firstZombieAttack (string& sIdentiier, CPlayer* p);
    void h_moveToHospital    (string& sIdentiier, CPlayer* p);
    void h_exitTrainstation  (string& sIdentiier, CPlayer* p);

    //Tuturials
    void h_startTutorial     (string&, CPlayer* p);

    //Programmer
    void h_try(string& sIdentiier, CPlayer* p);
    void h_try1(string& sIdentiier, CPlayer* p);
    void h_try2(string& sIdentiier, CPlayer* p);
    void h_try3(string& sIdentiier, CPlayer* p);
    void h_try4(string& sIdentiier, CPlayer* p);

    void error(CPlayer* p);
};

#endif
