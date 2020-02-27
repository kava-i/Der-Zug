#ifndef CPERSON_H 
#define CPERSON_H

#include <iostream>
#include <queue> 
#include <map>
#include <tuple>
#include "CItem.hpp"
#include "CDialog.hpp"
#include "CAttack.hpp"
#include "CItem.hpp"
#include "fuzzy.hpp"
#include "Webcmd.hpp"

using std::string;

class CPerson
{
protected:
    string m_sName;
    string m_sID;

    //Stats
    typedef map<string, int> stats;
    stats m_stats;

    typedef map<std::string, map<string, vector<CItem*>> > inventory;
    inventory m_inventory;

    typedef std::map<string, CAttack*> attacks;
    attacks m_attacks;

    //Dialog
    SDialog* m_dialog;

public:
    
    virtual ~CPerson() {}

    // *** GETTER *** // 
    string getName();
    string getID();
    int getStat(std::string id);
    SDialog* getDialog();
    attacks& getAttacks();

    // *** SETTER *** //
    void setDialog(SDialog* newDialog);
    void setStat(std::string, int stat);

    // *** Item and Inventory *** //
    std::string printInventory(std::string color="", int highlight=-1);
    virtual void addItem(CItem* item);
    void removeItem(string sItemName);
    CItem* getItem(string sName); 
    CItem* getItem_byID(string sID);

    // *** Attacks *** // 
    string printAttacks();
    string getAttack(string sPlayerChoice);

    template <typename T1, typename T2> std::string table(std::map<std::string, T1> t, T2 &lamda, std::string style="width:10%")
    {
        std::string str ="<table style='"+style+"'>";
        for(auto it : t) {
            str += "<tr><td>" + it.first + ":</td><td>";
            str += lamda(it.second);
            str += "</td></tr>";
        }
        str+="</table>";
        return str; 
    }

    template <typename T1, typename T2, typename T3> std::string table(std::map<std::string, std::map<std::string, T1>> t, T2 &lamda1, T3 &lamda2, std::string style="width:10%", int highlight=-1)
    {
        std::string str = "<table style='"+style+"'<tr>", color = "", white = Webcmd::set_color(Webcmd::color::WHITE);

        int counter = 0; size_t max = 0;
        for(auto& it : t){
            color = ""; 
            if(it.second.size() > max)
                max = it.second.size();
            if(counter == highlight)
                color = Webcmd::set_color(Webcmd::color::GREEN);
            str += "<th>" + color + it.first + "</th><th></th>";
            str += Webcmd::set_color(Webcmd::color::WHITE);
            counter++;
        }
        str += "</tr>";

        for(size_t i=0; i<max; i++)
        {
            counter = 0; str += "<tr>";
            for(auto it : t) {
                if(counter == highlight) color = Webcmd::set_color(Webcmd::color::GREEN);
                else color = "";

                if(it.second.size() <= i)
                    str +="<td></td><td></td>";
                else {
                    auto jt = it.second.begin(); std::advance(jt, i);
                    str += "<td>" + color + lamda1(jt->first, jt->second) +  ":</td>"
                         + "<td>" + color + lamda2(jt->first, jt->second) + "</td>";
                }
                str += Webcmd::set_color(Webcmd::color::WHITE);
                counter++;
            }
            str += "</tr>";
        }
        return str + "</table>"; 
    }

    // *** Functions needed in CPlayer *** //
    virtual void throw_event(std::string) {}
    virtual void setStatus(string)   { std::cout << "FATAL!!!\n"; }
    virtual void appendPrint(string) { std::cout << "FATAL!!!\n"; }

};

#endif

        
