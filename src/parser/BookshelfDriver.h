//Initial version obtained from Wuxi Li (UTDA) and modified by Rachel Selina Rajarathnam (UTDA)
#ifndef __BOOKSHELFPARSER_DRIVER_H__
#define __BOOKSHELFPARSER_DRIVER_H__

#include <string>
#include "BookshelfDatabase.h"


namespace BookshelfParser
{

class Driver
{
    public:
        Driver(BookshelfDatabase &db);

        /// enable debug output in the flex scanner
        bool trace_scanning;

        /// enable debug output in the bison parser
        bool trace_parsing;

        /// stream name (file or input stream) used for error messages.
        std::string streamname;

        /** Invoke the scanner and parser for a stream.
         * @param in	input stream
         * @param sname	stream name for error messages
         * @return		true if successfully parsed
         */
        bool parse_stream(std::istream& in,
                const std::string& sname = "stream input");

        /** Invoke the scanner and parser on an input string.
         * @param input	input string
         * @param sname	stream name for error messages
         * @return		true if successfully parsed
         */
        bool parse_string(const std::string& input,
                const std::string& sname = "string stream");

        /** Invoke the scanner and parser on a file. Use parse_stream with a
         * std::ifstream if detection of file reading errors is required.
         * @param filename	input file name
         * @return		true if successfully parsed
         */
        bool parse_file(const std::string& filename);

        /** Error handling with associated line number. This can be modified to
         * output the error e.g. to a dialog box. */
        void error(const class location& l, const std::string& m);

        /** General error handling. This can be modified to output the error
         * e.g. to a dialog box. */
        void error(const std::string& m);

        virtual ~Driver();

        /* parsing SPICE file */
        void createCellCbk(const std::string &str);
        void addPortCbk(const std::string &str);
        void addfeatureCbk(const std::string &str);
        void endofcellCbk();
        void EoICbk();

        /* MOS information */
        void addMOSCbk(const std::string &str0, const std::string &str1, const std::string &str2, const std::string &str3, const std::string &str4, const std::string &str5); 

        /* DIO information */
        void addDioCbk(const std::string &str0, const std::string &str1, const std::string &str2, const std::string &str3); 

        /* BJT information */
        void addBjtCbk(const std::string &str0, const std::string &str1, const std::string &str2, const std::string &str3, const std::string &str4); 

        /* COMP information */
        void addCompCbk(const std::string &str0, const std::string &str1); 
        void addCompPortCbk(const std::string &str); 

        /* RES information */
        void addResCbk(const std::string &str0, const std::string &str1, const std::string &str2); 
        void addResSCbk(const std::string &str0, const std::string &str1, const std::string &str2, const std::string &str3); 
        void addResNCbk(const std::string &str0, const std::string &str1, const std::string &str2, const std::string &str3, const std::string &str4); 

        /* CAP information */
        void addCapCbk(const std::string &str0, const std::string &str1, const std::string &str2);
        void addCapSCbk(const std::string &str0, const std::string &str1, const std::string &str2, const std::string &str3);
        void addCapNCbk(const std::string &str0, const std::string &str1, const std::string &str2, const std::string &str3, const std::string &str4);

        class Parser*  parser;
        class Scanner* scanner;
    private:
        BookshelfDatabase &_db;
};

} // End of namespace BookshelfParser

#endif // __BOOKSHELFPARSER_DRIVER_H__
