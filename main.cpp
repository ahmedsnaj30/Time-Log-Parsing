// Copyright 2022 Ahmed Shaheen
#include <iostream>
#include <string>
#include <fstream>
#include <boost/regex.hpp>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

// Should output: line number, timestamp, success->elapsedtime/failure

using boost::posix_time::ptime;
using boost::posix_time::time_duration;
using boost::gregorian::date;
using boost::gregorian::from_simple_string;

time_duration time_difference(ptime time1, ptime time2,
std::function<time_duration(ptime, ptime)> t_diff) {
    return t_diff(time1, time2);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Invalid number of args: ./ps7 <device name>.log\n";
    }

    // File stuff
    std::string log_name(argv[1]);
    std::ifstream file;
    file.open(log_name);
    std::string output_name(log_name + ".rpt");
    std::ofstream output;
    output.open(output_name.c_str());

    // input string
    std::string str;

    // Date/Time Strings
    std::string start_date = "";
    std::string end_date = "";

    // Report/Boots
    std::string report = "Device Boot Report\n\n";
    std::string boots = "";
    int boot_count = 0;
    int comp_count = 0;

    int line_num = 1;

    // startup message: (log.c.166) server started
    // timestamp of startup
    // Regexes for Date, Time, and Startup msg
    std::string start_str = "[0-9]{4}\\-[0-9]{2}\\-[0-9]{2} ";
    start_str += "[0-9]{2}:[0-9]{2}:[0-9]{2}: ";
    start_str += "\\(log\\.c\\.166\\) server started";

    std::string end_str = "[0-9]{4}\\-[0-9]{2}\\-[0-9]{2} ";
    end_str += "[0-9]{2}:[0-9]{2}:[0-9]{2}\\.[0-9]{3}:INFO:";
    end_str += "oejs\\.AbstractConnector:Started SelectChannelConnector@0\\.0\\.0\\.0:9080"; //NOLINT

    boost::regex start_up(start_str, boost::regex::perl);
    boost::regex completion(end_str);

    boost::regex re_date("[0-9]{4}\\-[0-9]{2}\\-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}"); //NOLINT

    // Match strings
    boost::smatch match;    // initial full string match
    boost::smatch match2;   // date/time match

    // Date/Time duration variables
    int hours = 0;
    int minutes = 0;
    int seconds = 0;

    date d1;
    date d2;

    ptime t1;
    ptime t2;
    time_duration td;

    bool start_state = false;

    while (std::getline(file, str)) {
        start_date.clear();
        end_date.clear();
        if (boost::regex_search(str, match, start_up)) {
            if (start_state == true) {
                boots += "**** Incomplete boot ****\n";
            }

            for (size_t i = 0; i < match.size(); i++) {
                boots += "\n\n=== Device boot ===\n";
                if (boost::regex_search(str, match2, re_date)) {
                    start_date += match2.str(i) + " Boot Start\n";
                }
                boots += std::to_string(line_num) +
                "(" + log_name + "): " + start_date;
            }

            // 0000-00-00 00:00:00:
            d1 = date(from_simple_string(match.str().substr(0, 10)));
            hours = stoi(match.str().substr(11, 2));
            minutes = stoi(match.str().substr(14, 2));
            seconds = stoi(match.str().substr(17, 2));

            t1 = ptime(d1, time_duration(hours, minutes, seconds, 0));

            start_state = true;

            boot_count++;
        }
        if (boost::regex_search(str, match, completion)) {
            start_state = false;

            for (size_t i = 0; i < match.size(); i++) {
                if (boost::regex_search(str, match2, re_date)) {
                    end_date += match2.str(i) + " Boot Completed\n";
                }
                boots += std::to_string(line_num) +
                "(" + log_name + "): " + end_date;
            }

            d2 = date(from_simple_string(match.str().substr(0, 10)));
            hours = stoi(match.str().substr(11, 2));
            minutes = stoi(match.str().substr(14, 2));
            seconds = stoi(match.str().substr(17, 2));

            t2 = ptime(d2, time_duration(hours, minutes, seconds, 0));

            // td = t2 - t1 Lambda
            td = time_difference(t1, t2, [](ptime x, ptime y){ return y - x;});

            boots += "      Boot Time: " +
            std::to_string(td.total_milliseconds()) + "ms\n";

            comp_count++;
        }
        line_num++;
    }

    line_num -= 1;

    report += "InTouch log file: " + log_name + "\n";
    report += "Lines Scanned: " + std::to_string(line_num) + "\n\n";
    report += "Device boot count: initiated = " + std::to_string(boot_count);
    report += ", completed: " + std::to_string(comp_count) + " \n";
    report += boots;

    output << report;

    file.close();

    return 0;
}
