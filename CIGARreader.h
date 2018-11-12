//
// Created by mjopp on 11/11/2018.
//

#ifndef GFOLD_LR_PROJECT_CIGARREADER_H
#define GFOLD_LR_PROJECT_CIGARREADER_H

#include <vector>
#include <string>
#include <inttypes.h>

class CIGAR {
public:
    typedef enum {
        alignment_match =   'M',
        insertion =         'I',
        deletion =          'D',
        skipped_region =    'N',
        soft_clipping =     'S',
        hard_clipping =     'H',
        padding =           'P', // NOT YET SUPPORTED
        sequence_match =    '=', // NOT YET SUPPORTED
        sequence_mismatch = 'X'  // NOT YET SUPPORTED
    } CIGAR_TYPE;

    CIGAR_TYPE type;
    uint32_t length;
    uint64_t qstart;
    uint64_t rstart;

    static std::vector <CIGAR> read_cigars(std::string cigarSring, uint64_t lpos) {
        std::vector <CIGAR> retCIGARS;
        std::string cur_len = "";

        uint64_t curQStart = lpos;
        uint64_t curRStart = lpos;

        for(int i = 0; i < cigarSring.size(); i++) {
            char &c = cigarSring[i];

            // while we have numbers => add to cur_len
            if (std::isdigit(static_cast<unsigned char>(c))) {
                cur_len += c;
            } else {
                // NOT A NUMBER -> MUST BE CIGAR IDENTIFIER
                CIGAR oCIGAR;
                oCIGAR.length = atoi(cur_len.c_str());


                switch (c) {
                    case 'M':
                        oCIGAR.type = CIGAR::alignment_match;
                        oCIGAR.qstart = curQStart;
                        oCIGAR.rstart = curRStart;

                        curQStart += oCIGAR.length;
                        curRStart += oCIGAR.length;

                        break;
                    case 'I':
                        oCIGAR.type = CIGAR::insertion;
                        oCIGAR.qstart = curQStart;
                        oCIGAR.rstart = curRStart;

                        curQStart += oCIGAR.length;
                        //curRStart += oCIGAR.length;
                        break;
                    case 'D':
                        oCIGAR.type = CIGAR::deletion;
                        oCIGAR.qstart = curQStart;
                        oCIGAR.rstart = curRStart;

                        //curQStart += oCIGAR.length;
                        curRStart += oCIGAR.length;
                        break;
                    case 'S':
                        oCIGAR.type = CIGAR::soft_clipping;

                        oCIGAR.qstart = curQStart;
                        oCIGAR.rstart = curRStart;

                        curQStart += oCIGAR.length;
                        curRStart += oCIGAR.length;

                        break;
                    case 'H':
                        oCIGAR.type = CIGAR::hard_clipping;

                        oCIGAR.qstart = curQStart;
                        oCIGAR.rstart = curRStart;

                        //curQStart += oCIGAR.length;
                        curRStart += oCIGAR.length;

                        break;
                    case 'N':
                        oCIGAR.type = CIGAR::skipped_region;

                        oCIGAR.qstart = curQStart;
                        oCIGAR.rstart = curRStart;

                        //curQStart += oCIGAR.length;
                        curRStart += oCIGAR.length;

                        break;
                    case 'P':
                        throw std::string("P_in_cigar");
                        break;
                    default:
                        throw std::string("unsupported_cigar");
                }
                retCIGARS.push_back(oCIGAR);
                cur_len = "";
            }
        }
        return retCIGARS;
    }

    friend std::ostream& operator<<(std::ostream &os, const CIGAR &oCIGAR) {

        os << (char) static_cast<std::underlying_type<CIGAR::CIGAR_TYPE>::type>(oCIGAR.type) << " L: " << oCIGAR.length << " Q: " << oCIGAR.qstart << " R: " << oCIGAR.rstart;
        return os;
    }

};

#endif //GFOLD_LR_PROJECT_CIGARREADER_H
