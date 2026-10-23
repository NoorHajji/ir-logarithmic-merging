// Logarithmic merging: each occupied level acts like a bit in a binary counter.
#include "ir/dynamic.hpp"

using namespace std;

namespace ir {

LogarithmicIndex::~LogarithmicIndex() {
    for (size_t i = 0; i < gen_.size(); i++) {
        if (!gen_[i].empty()) {
            readers_.forget(gen_[i]);
            remove_index(gen_[i]);
        }
    }
}
void LogarithmicIndex::flush() {
    if (z0_.empty()) return;

    string z = new_temp_name();
    z0_.write_to(z);
    z0_.clear();

    for (size_t i = 0; ; ++i) {
        if (i == gen_.size()) gen_.push_back("");

        if (gen_[i].empty()) {
            gen_[i] = z;
            return;
        }

        // This level is full, so merge and carry to the next level.
        string merged = new_temp_name();
        merge_indexes({gen_[i], z}, merged);
        stats_.merges++;

        readers_.forget(gen_[i]);
        readers_.forget(z);
        remove_index(gen_[i]);
        remove_index(z);
        gen_[i].clear();
        z = merged;
    }
}

PostingsList LogarithmicIndex::lookup(const string& term) const {
    PostingsList result = z0_.lookup(term);

    for (size_t i = 0; i < gen_.size(); i++) {
        if (!gen_[i].empty()) {
            PostingsList temp = disk_lookup(readers_, gen_[i], term);
            result = union_postings(result, temp);
        }
    }

    return result;
}

void LogarithmicIndex::finalize(const string& out_base) {
    flush();

    vector<string> inputs;
    // Higher levels contain older documents, so read them first.
    for (size_t i = gen_.size(); i > 0; --i) {
        if (!gen_[i - 1].empty()) inputs.push_back(gen_[i - 1]);
    }

    if (inputs.empty()) {
        write_empty_index(out_base);
    } else if (inputs.size() == 1) {
        readers_.forget(inputs[0]);
        rename_index(inputs[0], out_base);
    } else {
        merge_indexes(inputs, out_base);
        stats_.merges++;
        for (const string& base : inputs) {
            readers_.forget(base);
            remove_index(base);
        }
    }
    gen_.clear();
}

size_t LogarithmicIndex::disk_indexes() const {
    size_t count = 0;

    for (size_t i = 0; i < gen_.size(); i++) {
        if (gen_[i] != "") {
            count++;
        }
    }

    return count;
}

uint64_t LogarithmicIndex::generation_mask() const {
    uint64_t mask = 0;
    uint64_t value = 1;

    for (size_t i = 0; i < gen_.size() && i < 64; i++) {
        if (gen_[i] != "") {
            mask = mask + value;
        }
        value = value * 2;
    }

    return mask;
}

}  // namespace ir
