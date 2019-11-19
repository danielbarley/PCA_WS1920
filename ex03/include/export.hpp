#ifndef _EXPORT_HPP_
#define _EXPORT_HPP_

#include <string>
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <array>

namespace Export{
    class PGM {
        std::vector<std::string> lines;
        std::string filepath;
        uint32_t width, height;
        uint32_t max_val;

        public:
        PGM (std::string fp, std::vector<uint32_t> pixelarray, uint32_t array_width):
            filepath(fp),
            width(array_width)
        {
            height = pixelarray.size() / array_width;   // rows
            for (size_t i = 0; i < pixelarray.size() / array_width; i++) {
                std::vector<uint32_t> line {};
                for (size_t j = 0; j < array_width; j++) {
                    max_val = std::max(max_val, pixelarray[i*array_width+j]);
                    line.push_back(pixelarray[i*array_width+j]);
                }

                new_line(line);
            }
        }
        PGM (std::string fp, std::vector<double> pixelarray, uint32_t array_width, double max_double):
            filepath(fp)
        {
            width = array_width; // cols
            height = pixelarray.size() / array_width;   // rows
            max_val = 255;
            for (size_t i = 0; i < pixelarray.size() / array_width; i++) {
                std::vector<uint32_t> line {};
                for (size_t j = 0; j < array_width; j++) {
                    max_val = std::max(max_val, (uint32_t)(pixelarray[i*array_width+j]/max_double*max_val));
                    line.push_back((uint32_t)(pixelarray[i*array_width+j]/max_double*max_val));
                }

                new_line(line);
            }
        }
        PGM (std::string fp, std::vector<std::vector<uint32_t>> pixelarray):
            filepath(fp)
        {
            width = pixelarray[0].size(); // cols
            height = pixelarray.size();   // rows
            for (auto r: pixelarray) {
                new_line(r);
                for (auto p: r)
                    max_val = std::max(p, max_val);
            }
        }

        template<typename streamable>
        void new_line(std::vector<streamable> & entries, const char*  delimiter = " "){
            std::stringstream out;
            std::copy(entries.begin(), entries.end() -1, std::ostream_iterator<streamable>(out, delimiter));
            out << *entries.end();
            this->lines.push_back(out.str());
        }

        void flush() {
            std::ofstream fs;
            fs.open(this->filepath);
            //header
            fs << "P2\n" << width << " " << height << "\n" << max_val << "\n";
            //pixels
            for (auto & line: this->lines)
                fs << line << "\n";
            fs.close();
            lines.clear();
        }
    };

    class Export {

        std::vector<std::string> lines;
        std::string filepath;

        public:
        Export (std::string fp): filepath(fp) {}

        void from_df(std::vector<std::vector<double>> df) {
            // resize all vectors in df to same size
            size_t max_height =0;
            for (auto & column: df)
                max_height = std::max(max_height, column.size());

            for (auto & column: df)
                column.resize(max_height);


            // insert rows
            for (size_t row =0; row < max_height; row++){
                std::vector<double> new_row {};
                for (auto & column: df)
                    new_row.push_back(column[row]);
                new_line(new_row);
            }
        }

        void from_df(std::vector<std::string> & names, std::vector<std::vector<double>> & df) {
            // insert names
            new_line(names);

            from_df(df);
        }

        void flush() {
            std::ofstream fs;
            fs.open(this->filepath);
            for (auto line: this->lines)
                fs << line << "\n";
            fs.close();
            lines.clear();
        }

        template<typename streamable>
        void new_line(std::vector<streamable> & entries, const char*  delimiter = " "){
            std::stringstream out;
            std::copy(entries.begin(), entries.end(), std::ostream_iterator<streamable>(out, delimiter));
            this->lines.push_back(out.str());
        }
    };

}

#endif /* ifndef _EXPORT_HPP_ */
