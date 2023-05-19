#ifndef CGRAMMA
#define CGRAMMA

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <map>
#include <string>
#include <tuple>
#include "func.h"


class CGramma
{
private:
    std::unordered_map<std::string, std::pair<std::string, std::string>> m_all;
    std::unordered_map<std::string, std::vector<std::string>> m_links;
    std::map<int, std::string> m_numbers;

public:
    /**
    * Constructor loading dictionaries
    */
    CGramma(std::vector<std::string> dictionaries)
    {
        m_numbers = {{1, "ein"}, {2, "zwei"}, {3, "drei"}, {4, "vier"}, {5, "fünf"},
                    {6, "sechs"}, {7, "sieben"}, {8, "acht"}, {9, "neun"}, {10, "zehn"}};
        for(const auto& dict : dictionaries)
            initDictionary(dict);
    }

    /**
    * Add a dictionary. Add all words to m_all with their base from
    * as first and the gramma information as second (map<key, pair>).
    * Currently we habe the problem, that every word only exist once
    * and gramma information is overwritten by next word which is the 
    * same, so the are some gramma informations missing. This could be 
    * fixed by storing all gramma information of a word in a vector, 
    * result to something like: map<string, pair<string, vector>> which
    * seams a bit overkill as me only need a few base forms anyway. 
    * Thus some grammatical forms are erased.
    * @param[in] sPath (Path to dictionary)
    */
    void initDictionary(std::string sPath)
    {
        //Load dictionary from text file
        std::ifstream read(sPath);
    
        if(!read)
        {
            std::cout << sPath << " not found.\n";
            return;
        }

        std::string sBuffer;

        //Read line by line and add to dictionary
        while(getline(read, sBuffer))
        {
            //Entries in dictionary musst be of the form: 
            // word [tab] grundform [tab] gramma-information
            std::vector<std::string> vec = func::split(sBuffer, "\t");

            //Ignore mistakes and short words, skip unimportant forms
            if(vec[2].find(":") == std::string::npos || vec[2].find("VER:IMP:PLU")!=std::string::npos)
                continue;

            //Add word to all words and to fitting base form.
            m_all[vec[0]] = std::make_pair(vec[1], vec[2]); 
            m_links[vec[1]].push_back(vec[0]);
        }
        read.close();
        std::cout << "Size: " << m_all.size() << std::endl;
    }

    /**
    * test-function printing all entries for the entered basic form.
    * @param[in] sWord basic grammatical form.
    */
    void printFormsFrom(std::string sWord)
    {
        if(m_links.count(sWord) == 0)
            std::cout << "Word nicht gefunden.\n";
        else
        {
            std::cout << sWord << ": " << std::endl;
            for(auto it : m_links[sWord])
                std::cout << it << " -> " << m_all[it].second << std::endl;
        } 
    } 

    /**
    * Return the article of a given word. "Ein" for masculine, "Eine" for 
    * feminine and nothing if the word can't be found. This gives user the
    * possibility to enter "the man" for definite articles
    * @pram[in] sWord word to deduct the article of
    * @return return article or nothing if word couldn't be found (or is
    * a proper name ("EIG").
    */
    std::string getArticle(std::string sWord)
    {
        //Check if word exists at all.
        if(m_all.count(sWord)==0)
            return "";
        
        //Deduct article
        if(m_all[sWord].second.find("EIG") != std::string::npos)
            return "";
        else if(m_all[sWord].second.find("FEM") != std::string::npos)
            return "eine ";
        else
            return "ein ";
    }

    /**
    * Return a certain form of the given word. For might be "PLU" for
    * plural, or more specific, f.e. "3:SIN:PRÄ" (third per-
    * son, singular, present).
    * @param[in] sWord word, of which changed form is inquired.
    * @return return inquired form.
    */
    std::string getForm(std::string sWord, std::string sForm)
    {
        //If word does not exist, return word.
        if(m_all.count(sWord) == 0)
            return sWord;

        //Iterate over all derived forms (first get base form)
        for(auto vec : m_links[m_all[sWord].first])
        {
            if(m_all[vec].second.find(sForm) != std::string::npos)
                return vec;
        }

        return sWord;
    }

    /**
    * Create sentence. Convert message to singular, when only one item in the list. Add
    * Error to message, when no item is in the list, deduct forms and replace last 
    * occurrence of ", " by "und ".
    * @param[in] words vector of words to build sentence with.
    * @param[in] sMessage message to print before listing items in sentence.
    * @param[in] sError error message to print when no it is found.
    * @return return full sentence.
    */
    std::string build(std::vector<std::string> words, std::string sMessage, std::string sError)
    {
				std::cout << "BUILD: " << words.size() << std::endl;
        // Check whether list is empty.
        if(words.size() == 0)
            return convertMessage(sMessage) + " " + sError;

        //Convert words.
        std::string sOutput = convertWords(words);
                
        //Replace last comma with "und", convert message to singular otherwiese.
        if(sOutput.rfind(",") != std::string::npos)
            sOutput.replace(sOutput.rfind(","), 2, " und ");
        else
            sMessage = convertMessage(sMessage);
        
        //Return build sentance
        return sMessage + " " + sOutput;
    }

    /**
    * Convert words by adding article and making them plural, if serveraal
    * identical items occur. (number written out in words?)
    * @param[in] words vector of words/ items.
    * @return words + article and eventually added up.
    */
    std::string convertWords(std::vector<std::string> words)
    {
        std::string sOutput = "";
        std::vector<std::string> vfound;    //Store identical words.
        for(const auto& word : words)
        {
            if(func::inArray(vfound, word) == true)
                continue; 
            int num = std::count(words.begin(), words.end(), word);
            if(num > 1)
            {
                sOutput += m_numbers[num] + " <b>" + getForm(word, "PLU") + "</b>, ";
                vfound.push_back(word);
            }
            else
                sOutput += getArticle(word) + "<b>" + word + "</b>, ";
        }

        //Replace last comma with dot.
        sOutput.replace(sOutput.end()-2, sOutput.end(), ". ");
        return sOutput;
    }

    /**
    * Convert verbs in string to singular.
    * @param[in] sMessage words to convert.
    * @return converter string.
    */
    std::string convertMessage(std::string sMessage)
    {
        std::string str = sMessage;
        sMessage = "";
        
        //iterate over words and convert verbs to singular.
        for(const auto& word : func::split(str, " "))
            sMessage += getForm(word, "3:SIN:PRÄ") + " ";
        
        //Erase last space and return.
        sMessage.pop_back();
        return sMessage;
    }
};

#endif
