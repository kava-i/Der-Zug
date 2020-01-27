#ifndef CSTANDARDCONTEXT_H
#define CSTANDARDCONTEXT_H

#include "CContext.hpp"
#include "CDialogContext.hpp"


class CStandardContext : public CContext
{
public:
    CStandardContext() : CContext()
    {
        //Set permeability
        m_permeable = true;

        //Add listeners
        add_listener("show", &CContext::h_show);
        add_listener("examine", &CContext::h_examine);
        add_listener("lookIn", &CContext::h_lookIn);
        add_listener("goTo", &CContext::h_goTo);
        add_listener("talkTo", &CContext::h_startDialog);
        add_listener("take", &CContext::h_take);
        add_listener("consume", &CContext::h_consume);
        add_listener("equipe", &CContext::h_equipe);
        add_listener("dequipe", &CContext::h_dequipe);
        add_listener("help", &CContext::h_help);
        add_listener("error", &CContext::h_error);

        //Rooms
        add_listener("goTo", &CContext::h_firstZombieAttack);
        add_listener("goTo", &CContext::h_moveToHospital, 0);
        add_listener("goTo", &CContext::h_endTutorial);

        //Tutorial
        add_listener("startTutorial", &CContext::h_startTutorial);

        //Developer
        add_listener("try", &CContext::h_try, 0);
    }

    //parser
    vector<event> parser(string, CPlayer*);

    //Handler
    void h_show             (string& sIdentiier, CPlayer* p);
    void h_examine          (string& sIdentiier, CPlayer* p);
    void h_lookIn           (string& sIdentiier, CPlayer* p);
    void h_take             (string& sIdentiier, CPlayer* p);
    void h_consume          (string& sIdentiier, CPlayer* p);
    void h_equipe           (string& sIdentiier, CPlayer* p);
    void h_dequipe          (string& sIdentiier, CPlayer* p);
    void h_goTo             (string& sIdentiier, CPlayer* p);
    void h_startDialog      (string& sIdentiier, CPlayer* p);
    void h_error            (string& sIdentiier, CPlayer* p);

    //Rooms
    void h_firstZombieAttack (string& sIdentiier, CPlayer* p);
    void h_moveToHospital    (string& sIdentiier, CPlayer* p);
    void h_endTutorial       (string& sIdentiier, CPlayer* p);

    //Tuturials
    void h_startTutorial     (string&, CPlayer* p);

    //Programmer
    void h_try(string& sIdentiier, CPlayer* p);
    void h_try1(string& sIdentiier, CPlayer* p);
    void h_try2(string& sIdentiier, CPlayer* p);
    void h_try3(string& sIdentiier, CPlayer* p);
    void h_try4(string& sIdentiier, CPlayer* p);
};

#endif
