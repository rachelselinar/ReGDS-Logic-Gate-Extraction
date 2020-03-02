//Initial version obtained from Wuxi Li (UTDA) and modified by Rachel Selina Rajarathnam (UTDA)
// $Id: driver.cc 39 2008-08-03 10:07:15Z tb $
/** \file driver.cc Implementation of the example::Driver class. */

#include "BookshelfDriver.h"
#include "BookshelfScanner.h"

namespace BookshelfParser {

Driver::Driver(BookshelfDatabase &db)
	: trace_scanning(false),
      trace_parsing(false),
      parser(NULL), scanner(NULL), _db(db)
{
}

Driver::~Driver()
{
   delete scanner;
   scanner = NULL;
   delete parser;
   parser = NULL;
}

bool Driver::parse_stream(std::istream& in, const std::string& sname)
{
    streamname = sname;

    delete scanner;
    scanner = new Scanner(&in);
    scanner->set_debug(trace_scanning);

    delete parser;
    parser = new Parser(*this);
    parser->set_debug_level(trace_parsing);
    return (parser->parse() == 0);
}

bool Driver::parse_file(const std::string &filename)
{
    std::ifstream in(filename.c_str());
    if (!in.good())
        return false;
    return parse_stream(in, filename);
}

bool Driver::parse_string(const std::string &input, const std::string& sname)
{
    std::istringstream iss(input);
    return parse_stream(iss, sname);
}

void Driver::error(const class location& l, const std::string& m)
{
    std::cerr << l << ": " << m << std::endl;
}

void Driver::error(const std::string& m)
{
    std::cerr << m << std::endl;
}

//////////////////////////////////
//                              //
//  Parser Call Back Functions  //
//                              //
//////////////////////////////////

/* parsing SPICE file */
void Driver::createCellCbk(const std::string &str)                                          { _db.createCellCbk(str); }
void Driver::addPortCbk(const std::string &str)                                             { _db.addPortCbk(str); }
void Driver::addfeatureCbk(const std::string &str)                                          { _db.addfeatureCbk(str); }
void Driver::endofcellCbk()                                                                 { _db.endofcellCbk(); }

/* MOS information */
void Driver::addMOSCbk(const std::string &str0, const std::string &str1, const std::string &str2, const std::string &str3, const std::string &str4, const std::string &str5) { _db.addMOSCbk(str0, str1, str2, str3, str4, str5); }

/* DIO information */
void Driver::addDioCbk(const std::string &str0, const std::string &str1, const std::string &str2, const std::string &str3) { _db.addDioCbk(str0, str1, str2, str3); }

/* BJT information */
void Driver::addBjtCbk(const std::string &str0, const std::string &str1, const std::string &str2, const std::string &str3, const std::string &str4) { _db.addBjtCbk(str0, str1, str2, str3, str4); }

/* COMP information */
void Driver::addCompCbk(const std::string &str0, const std::string &str1) { _db.addCompCbk(str0, str1); }
void Driver::addCompPortCbk(const std::string &str)                                          { _db.addCompPortCbk(str); }

/* RES information */
void Driver::addResCbk(const std::string &str0, const std::string &str1, const std::string &str2) { _db.addResCbk(str0, str1, str2); }
void Driver::addResSCbk(const std::string &str0, const std::string &str1, const std::string &str2, const std::string &str3) { _db.addResSCbk(str0, str1, str2, str3); }
void Driver::addResNCbk(const std::string &str0, const std::string &str1, const std::string &str2, const std::string &str3, const std::string &str4) { _db.addResNCbk(str0, str1, str2, str3, str4); }

/* CAP information */
void Driver::addCapCbk(const std::string &str0, const std::string &str1, const std::string &str2) { _db.addCapCbk(str0, str1, str2); }
void Driver::addCapSCbk(const std::string &str0, const std::string &str1, const std::string &str2, const std::string &str3) { _db.addCapSCbk(str0, str1, str2, str3); }
void Driver::addCapNCbk(const std::string &str0, const std::string &str1, const std::string &str2, const std::string &str3, const std::string &str4) { _db.addCapNCbk(str0, str1, str2, str3, str4); }

} /* end of BookshelfParser namespace */
