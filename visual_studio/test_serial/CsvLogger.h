#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

class CsvLogger 
{
public:
    explicit CsvLogger(const std::string& filename, std::string row_key = {})
        : filename_(filename), row_key_(std::move(row_key))
    {
        file_.open(filename_, std::ios::out | std::ios::trunc);
        if (!file_) throw std::runtime_error("csv open fail");
    }

    ~CsvLogger() 
    {
        if (has_any_) 
            commit();       
        if (file_.is_open()) 
            file_.close();
    }


    template <typename T>
    void set(const std::string& type, const T& v) 
    {
        if (!row_key_.empty() && type == row_key_ && has_any_) 
        {
            commit();
        }
        const size_t idx = ensure_column_(type); 
        if (cur_.size() < cols_.size()) cur_.resize(cols_.size());
        cur_[idx] = to_string_(v);
        has_any_ = true;
    }


    void commit() 
    {
        if (!has_any_) return;

        if (cur_.size() < cols_.size())
        {
            cur_.resize(cols_.size());
        }
        rows_.push_back(cur_);

        if (schema_changed_) 
        {
            rewrite_all_();           
            schema_changed_ = false;
        }
        else 
        {
            write_row_(rows_.back());  
            file_.flush();
        }

        cur_.assign(cols_.size(), "");
        has_any_ = false;
    }

private:
    // --- ป๓ลย ---
    std::string filename_;
    std::vector<std::string> cols_;                   
    std::unordered_map<std::string, size_t> cidx_;    
    std::vector<std::vector<std::string>> rows_;      
    std::vector<std::string> cur_;                    
    std::string row_key_;                             
    std::ofstream file_;
    bool schema_changed_ = false;
    bool has_any_ = false;

    size_t ensure_column_(const std::string& name) 
    {
        auto it = cidx_.find(name);
        if (it != cidx_.end()) return it->second;

        const size_t new_idx = cols_.size();
        cols_.push_back(name);
        cidx_[name] = new_idx;

        for (auto& r : rows_)
        {
            r.push_back("");
        }
        if (cur_.size() < cols_.size())
        {
            cur_.resize(cols_.size());
        }

        schema_changed_ = true;
        return new_idx;
    }

    static std::string to_string_(const std::string& s) { return s; }
    static std::string to_string_(const char* s) { return s ? std::string(s) : std::string(); }
    template <typename T>
    static std::string to_string_(const T& v) { std::ostringstream oss; oss << v; return oss.str(); }

    void rewrite_all_() 
    {
        file_.close();
        file_.open(filename_, std::ios::out | std::ios::trunc);
        if (!file_) throw std::runtime_error("csv reopen fail");

        write_header_();
        for (const auto& r : rows_) write_row_(r);
        file_.flush();
    }

    void write_header_() 
    {
        for (size_t i = 0; i < cols_.size(); ++i) 
        {
            if (i) file_ << ',';
            file_ << cols_[i];
        }
        file_ << '\n';
    }

    void write_row_(const std::vector<std::string>& r) 
    {
        for (size_t i = 0; i < r.size(); ++i) 
        {
            if (i) file_ << ',';
            file_ << r[i];                 
        }
        file_ << '\n';
    }
};
