#ifndef CTEXT_H
#define CTEXT_H

#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>

#include "objects/helper/update.h"
#include "tools/func.h"
#include "tools/webcmd.h"
#include "objects/mind.h"

class CPlayer;
class COutput;

class CText {
	private:
		CPlayer* player_;
		std::vector<COutput*> texts_;

	public:
		CText(nlohmann::json attributes, CPlayer*);

		std::string print(bool events=true);
		std::string reducedPrint(bool events=true);
		std::string pagePrint(size_t page);
		size_t getNumPages();
		bool underline(size_t page, std::string str1, std::string str2);
};

class COutput {
	private:
    std::string speaker_;
    std::string text_;
    std::string music_;
    std::string image_;

    std::string logic_;
    Updates updates_;
    std::vector<std::string> pre_permanent_events_;
    std::vector<std::string> pre_one_time_events_;
    std::vector<std::string> post_permanent_events_;
    std::vector<std::string> post_one_time_events_;

    //In case of book, or read-item
    size_t page_;

	public:
    COutput(nlohmann::json jAttributes);

    // *** getter *** //
    std::string getText();
    std::string getSpeaker();
    size_t getPage();

    std::string print(CPlayer* p, bool events=true);
    std::string reducedPrint(CPlayer* p, bool events=false);

    bool checkDependencies(std::string& sSuccess, CPlayer* p);
    void addEvents(CPlayer* p);

    bool underline(std::string str1, std::string str2);
};

#endif
