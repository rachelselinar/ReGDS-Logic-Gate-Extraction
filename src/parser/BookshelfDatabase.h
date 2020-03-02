//Initial version obtained from Wuxi Li (UTDA) and modified by Rachel Selina Rajarathnam (UTDA)
#ifndef __BOOKSHELFPARSER_DATABASE_H__
#define __BOOKSHELFPARSER_DATABASE_H__

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <array>

namespace BookshelfParser
{

class BookshelfDatabase
{
    public:
        /* parsing SPICE file */
        virtual void createCellCbk(const std::string &str) = 0;
        virtual void addPortCbk(const std::string &str) = 0;
        virtual void addfeatureCbk(const std::string &str) = 0;
        virtual void endofcellCbk() = 0;

        /* MOS information */
        virtual void addMOSCbk(const std::string &str0, const std::string &str1, const std::string &str2, const std::string &str3, const std::string &str4, const std::string &str5) = 0; 

        /* DIO information */
        virtual void addDioCbk(const std::string &str0, const std::string &str1, const std::string &str2, const std::string &str3) = 0; 

        /* BJT information */
        virtual void addBjtCbk(const std::string &str0, const std::string &str1, const std::string &str2, const std::string &str3, const std::string &str4) = 0; 

        /* COMP information */
        virtual void addCompCbk(const std::string &str0, const std::string &str1) = 0; 
        virtual void addCompPortCbk(const std::string &str) = 0; 

        /* RES information */
        virtual void addResCbk(const std::string &str0, const std::string &str1, const std::string &str2) = 0; 
        virtual void addResSCbk(const std::string &str0, const std::string &str1, const std::string &str2, const std::string &str3) = 0; 
        virtual void addResNCbk(const std::string &str0, const std::string &str1, const std::string &str2, const std::string &str3, const std::string &str4) = 0; 

        /* CAP information */
        virtual void addCapCbk(const std::string &str0, const std::string &str1, const std::string &str2) = 0;
        virtual void addCapSCbk(const std::string &str0, const std::string &str1, const std::string &str2, const std::string &str3) = 0;
        virtual void addCapNCbk(const std::string &str0, const std::string &str1, const std::string &str2, const std::string &str3, const std::string &str4) = 0;

};

} // namespace BookshelfParser

#endif
