#include "Analyzer.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <thread>
#include <fstream>

// File can be big enough and can not be fit into RAM.
// We can analyze file by parts.
// It is better to consider the option where we have not enough RAM to store word dictionaty
#define BUFFER_SIZE 10000

std::string Analyzer::read_text(size_t len, std::ifstream& file)
{
    // lock file for other threads
    std::lock_guard<std::recursive_mutex> lock(file_mutex);

    if (file.eof())
    {
        return "";
    }

    // read part of file into array
    char* str = new char[len];
    file.read(str, static_cast<long long>(len));
    std::string string_part;
    bool found = false;

    // if file is over get only real part
    if (file.eof())
    {
        found = true;
        string_part = std::string(str, 0, file.gcount());
    }
    // in case of buffer size we can read only part of the word
    // so we need to take only words before the last space symbol
    // and move reading pointer back
    else for(size_t i = len; i > 0 ; i--)
    {
        if (str[i] == ' ')
        {
            string_part = std::string(str, 0, i);
            text_position += i + 1;
            file.seekg(text_position);
            found = true;
            break;
        }
    }
    // in case one singal word is bigger than our buffer (there are no space in text part)
    // we increase buffer and read again
    if (!found)
    {
        file.seekg(text_position);
        return read_text(len * 2, file);
    }
    return string_part;

}

void Analyzer::add_word_to_dict(std::string word, word_freq_dict& local_dict)
{
    // convert word to lower case
    boost::algorithm::to_lower(word);
    // check for a lot of new line symbols in a row
    if (word != "")
    {
        local_dict[word]++;
    }
}

void Analyzer::analyze_part_of_text(std::ifstream& file)
{
    // local dictionary for every thread
    word_freq_dict local_word_freq_dict;

    while(not file.eof())
    {
        std::string string_part = read_text(BUFFER_SIZE, file);
        // removing all characters except letters and spaces
        string_part.erase(remove_if(string_part.begin(), string_part.end(), [](char c)
        {
            return !isalpha(c) && !isspace(c);
        }), string_part.end());

        // removing end of line symbols
        boost::replace_all(string_part, "\n", " ");

        // looking for every word and adding it to dict
        size_t last_space_position = 0;
        for(size_t current_position = 0; current_position < string_part.size(); current_position++){
            if (string_part[current_position] == ' '){
                add_word_to_dict(string_part.substr(last_space_position, current_position - last_space_position), local_word_freq_dict);
                last_space_position = current_position + 1;
            }
        }
        // add last word
        add_word_to_dict(string_part.substr(last_space_position, string_part.size()), local_word_freq_dict);
    }
    {
        // moving frequencies from local dictionary to general one
        std::lock_guard<std::mutex> lock(dict_mutex);
        for(auto && [word, freq]: local_word_freq_dict)
        {
            words_frequency[word] += freq;
        }
    }

}

void Analyzer::analyze_text(const std::string& input_file)
{
    std::ifstream file = check_input_file(input_file);


    std::vector<std::thread> threads;
    for(ushort i = 0; i < 4; i++)
    {
        threads.push_back(std::thread(&Analyzer::analyze_part_of_text, this, std::ref(file)));
    }
    for(ushort i = 0; i < 4; i++)
    {
        threads[i].join();
    }
    // to sort words by frequency we can move words to ordered map with ordered set inside
    for(auto& [word, freq]: words_frequency)
    {
        frequency_words[freq].insert(std::move(word));
    }
    words_frequency.clear();
}

void Analyzer::write_data_to_file(const std::string& output_file) const
{
    std::ofstream file = check_output_file(output_file);
    // frequency of words are sorted ascending
    // to get descending we need to take reverse iterators
    for(auto& [freq, words]: boost::adaptors::reverse(frequency_words))
    {
        for(auto& word: words)
        {
            file << std::to_string(freq) + " " + word + "\n";
        }
    }
}

std::ifstream Analyzer::check_input_file(const std::string& input_file) const
{
    // check if file exists
    if (!boost::filesystem::exists(input_file))
    {
        throw std::runtime_error("File is not exist: " + input_file);
    }
    // check if file could not be opened
    std::ifstream file(input_file);
    if(!file)
    {
        throw std::runtime_error("File could not be opened: " + input_file);
    }
    // check if file is empty
    if (file.peek() == std::ifstream::traits_type::eof())
    {
        throw std::runtime_error("File is empty: " + input_file);
    }
    return file;

}

std::ofstream Analyzer::check_output_file(const std::string& output_file) const
{
    // check if folder exists
    boost::filesystem::path path_to_file(output_file);
    boost::filesystem::path dir = path_to_file.parent_path();
    if (!boost::filesystem::exists(dir))
    {
        throw std::runtime_error("Directory is not exist: " + dir.string());
    }
    // check if file could not be opened
    std::ofstream file(output_file, std::ofstream::out | std::ofstream::trunc);
    if (!file)
    {
        throw std::runtime_error("File could not be opened: " + output_file);
    }
    return file;
}
