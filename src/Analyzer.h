#pragma once
#include <unordered_map>
#include <map>
#include <set>
#include <string>
#include <mutex>

// In order to have fast writing of words we can use unordered map with O(1) to write
typedef std::unordered_map<std::string, long long> word_freq_dict;
// To sort result we can put information to ordered map woth O(logN) to write
typedef std::map<long long, std::set<std::string> > freq_word_dict;

class Analyzer{
public:
    Analyzer()
       : text_position(0)
    {

    }
    void analyze_text(const std::string& input_file);
    void write_data_to_file(const std::string& output_file) const;
    void print_data() const;

private:
    // methods
    void analyze_part_of_text(std::ifstream& file);
    void add_word_to_dict(std::string word, word_freq_dict& local_dict);
    std::string read_text(size_t len, std::ifstream& file);
    std::ifstream check_input_file(const std::string& input_file) const;
    std::ofstream check_output_file(const std::string& output_file) const;

    // variables
    word_freq_dict words_frequency;
    freq_word_dict frequency_words;
    long long text_position;

    // mutex
    std::recursive_mutex file_mutex;
    std::mutex dict_mutex;
};
