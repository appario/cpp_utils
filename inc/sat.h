#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_set>

class cnf {
    std::vector<std::vector<int>> cnf_;
    std::unordered_set<unsigned> lits_;

    bool unit_prop(std::unordered_set<int>& assignments) {
        std::unordered_set<int> units;
        for (auto it = cnf_.begin(); it != cnf_.end(); it++)
            if (it->size() == 1)
                units.insert(it->front());
        for (auto& unit : units)
            if (!assign(unit, assignments))
                return false;
        return true;
    }

public:
    cnf(std::string file) {
        std::string line;
        std::ifstream infile(file);
        while (std::getline(infile, line)) {
            std::istringstream iss(line);
            if (line.size() > 0 && line[0] == 'p') {
                int nc, nv;
                std::string tmp;
                iss >> tmp >> tmp >> nv >> nc;
                for (int i = 0; i < nc; i++) {
                    std::getline(infile, line);
                    iss = std::istringstream(line);
                    std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
                                                    std::istream_iterator<std::string>{}};
                    std::vector<int> clause;
                    for (auto it = tokens.begin(); it != tokens.end() - 1; it++) {
                        int var = std::atoi(it->c_str());
                        clause.push_back(var);
                        lits_.insert(std::abs(var));
                    }
                    cnf_.push_back(clause);
                }
            }
        }
    }

    bool assign(int assignment, std::unordered_set<int>& assignments) {
        if (assignments.find(-assignment) != assignments.end())
            return false;
        if (assignments.find(assignment) != assignments.end())
            return true;
        lits_.erase(std::abs(assignment));
        assignments.insert(assignment);
        for (auto it = cnf_.begin(); it != cnf_.end(); it++) {
            for (auto it2 = it->begin(); it2 != it->end();) {
                if (*it2 == assignment) {
                    it = cnf_.erase(it) - 1;
                    break;
                } else if (*it2 == -assignment) {
                    it2 = it->erase(it2);
                    if (it->size() == 0)
                        return false;
                    break;
                } else
                    it2++;
            }
        }
        return unit_prop(assignments);
    }

    bool solved() {
        return cnf_.size() == 0;
    }

    const std::unordered_set<unsigned>& lits() {
        return lits_;
    }
};

bool dpll(cnf c, std::unordered_set<int>& assignments) {
    if (c.solved())
        return true;
    cnf n = c;

    // Try assigning variable v
    int assn = *(c.lits().begin());
    std::unordered_set<int> t_assn(assignments);
    if (n.assign(assn, t_assn)) {
        if (dpll(n, t_assn)) {
            assignments = std::move(t_assn);
            return true;
        }
    }

    // Try assigning variable -v
    n = std::move(c);
    t_assn = assignments;
    if (!n.assign(-assn, t_assn))
        return false;
    if (dpll(n, t_assn)) {
        assignments = std::move(t_assn);
        return true;
    }
    return false;
}
