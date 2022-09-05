#include <iostream>
#include <vector>
#include <fstream>
#include "api.hpp"

int flagCheck = 0;

class Automat {
    short nameCounter = 0;
    Alphabet *alphabet = nullptr;
    short currentIndex;
    char currentElement{};
    std::string regularExpression;

    void newElem() {
        if (currentIndex != regularExpression.length()) {
            currentElement = regularExpression[currentIndex];
            currentIndex += 1;
        }
        else {
            currentElement = '#';
        }
    }

public:

    explicit Automat(const std::string& regularExpr) {
        regularExpression = regularExpr;
        alphabet = new Alphabet(regularExpr);
        autom = new DFA(regularExpr);
        currentIndex = 0;
    }

    std::vector<short> initialState;
    std::vector<short> endFlag;
    DFA* autom = nullptr;


    struct Tree {
        bool nullable{};
        std::vector<short> firstpos;
        std::vector<short> lastpos;
    };

    struct followPos {
        std::vector<short> followpos;
        char character{};
    };

    std::vector<followPos> followpos;

    bool boolFinalState(std::vector<short> state) {

        for (int i = 0; i < state.size(); i++) {
            if (state[i] == endFlag[0]) {
                return true;
            }
        }
        return false;

    }
    std::vector<std::vector<short>> states;
    std::vector<std::string> stateNames;
    int statesCount = 1;

    void construct1(std::vector<short> nextState) {
        std::string oldName;
        std::string alphabet1 = autom->get_alphabet().to_string();
        if (states.empty()) {
            states.push_back(initialState);
            oldName = std::to_string(nameCounter++);
            stateNames.push_back(oldName);
            autom->create_state(oldName, boolFinalState(initialState));
            autom->set_initial(oldName);
            nextState = initialState;
        }
        else {
            for (int i = 0; i < states.size(); i++) {
                if (states[i] == nextState) {
                    oldName = stateNames[i];
                    break;
                }
            }
        }

        bool flag = false;
        std::string tempName;

        for (int i = 0; i < alphabet1.size(); i++) {
            std::vector<short> newState;
            for (int j = 0; j < nextState.size(); j++) {
                followPos folposTemp = followpos[nextState[j]];
                if (folposTemp.character == alphabet1[i])
                {
                    std::vector<short> vectords = {folposTemp.followpos.begin(), folposTemp.followpos.end()};
                    for (int k = 0; k < vectords.size(); k++) {
                        newState.push_back(vectords[k]);
                    }
                }
            }
            sort (newState.begin(), newState.end());
            auto pos2 = std::unique(newState.begin(), newState.end());
            newState.erase(pos2, newState.end());
            if (!newState.empty()) {
                for (int l = 0; l < states.size(); l++) {
                    if (newState == states[l]) {
                        flag = true;
                        tempName = stateNames[l];
                    }
                }
                if (!flag) {
                    bool isFinal = boolFinalState(newState);
                    states.push_back(newState);
                    std::string newName = std::to_string(nameCounter++);
                    stateNames.push_back(newName);
                    autom->create_state(newName, isFinal);
                    autom->set_trans(oldName, alphabet1[i], newName);
                }
                else {
                    autom->set_trans(oldName, alphabet1[i], tempName);
                }
                flag = false;
            }
        }
        if (statesCount < states.size()) {
            construct1(states[statesCount++]);
        }


    }

    void getAut() {
        fullTree();
        construct1({});
    }

    Tree fullTree() {
        if (flagCheck == 0) {
            newElem();
            flagCheck+=1;
        }
        Tree lTree = multiplication();
        while (currentElement == '|') {
            newElem();

            lTree = checkOr(lTree, multiplication());
        }
        if (currentElement == ')') {
            newElem();
          }
        else if (currentElement == '#') {
            Tree treeEnd = Tree();
            treeEnd.nullable = false;
            treeEnd.firstpos.push_back(followpos.size());
            treeEnd.lastpos.push_back(followpos.size());
            sort (treeEnd.firstpos.begin(), treeEnd.firstpos.end());
            sort (treeEnd.lastpos.begin(), treeEnd.lastpos.end());
            auto last2 = std::unique(treeEnd.lastpos.begin(), treeEnd.lastpos.end());
            auto pred = std::unique(treeEnd.firstpos.begin(), treeEnd.firstpos.end());
            treeEnd.lastpos.erase(last2, treeEnd.lastpos.end());
            treeEnd.firstpos.erase(pred, treeEnd.firstpos.end());
            followpos.push_back({std::vector<short>(), currentElement});
            lTree = checkAnd(lTree, treeEnd);
            initialState = lTree.firstpos;
            endFlag = lTree.lastpos;
        }
        return lTree;
    }

private:
    static Tree checkOr(Tree lTree, Tree rTree) {
        Tree newTree = Tree();
        std::vector<int> tempVect = {lTree.firstpos.begin(), lTree.firstpos.end()};
        std::vector<int> tempVect1 = {rTree.firstpos.begin(), rTree.firstpos.end()};
        std::vector<int> tempVect2 = {lTree.lastpos.begin(), lTree.lastpos.end()};
        std::vector<int> tempVect3 = {rTree.lastpos.begin(), rTree.lastpos.end()};
        newTree.nullable = lTree.nullable or rTree.nullable;
        for (int i = 0; i < tempVect.size(); i++) {
            newTree.firstpos.push_back(tempVect[i]);
        }
        for (int i = 0; i < tempVect1.size(); i++) {
            newTree.firstpos.push_back(tempVect1[i]);
        }
        for (int i = 0; i < tempVect2.size(); i++) {
            newTree.lastpos.push_back(tempVect2[i]);
        }
        for (int i = 0; i < tempVect3.size(); i++) {
            newTree.lastpos.push_back(tempVect3[i]);
        }
        sort (newTree.firstpos.begin(), newTree.firstpos.end());
        sort (newTree.lastpos.begin(), newTree.lastpos.end());
        auto last = std::unique(newTree.firstpos.begin(), newTree.firstpos.end());
        newTree.firstpos.erase(last, newTree.firstpos.end());
        auto pred = std::unique(newTree.lastpos.begin(), newTree.lastpos.end());
        newTree.lastpos.erase(pred, newTree.lastpos.end());

        return newTree;
    }
    Tree checkAnd(Tree lTree, Tree rTree) {
        Tree newTree = Tree();
        std::vector<int> tempVect = {lTree .firstpos.begin(), lTree .firstpos.end()};
        std::vector<int> tempVect1 = {rTree.firstpos.begin(), rTree.firstpos.end()};
        std::vector<int> tempVect2 = {lTree .lastpos.begin(), lTree .lastpos.end()};
        std::vector<int> tempVect3 = {rTree.lastpos.begin(), rTree.lastpos.end()};
        newTree.nullable = rTree.nullable and lTree.nullable;
        if (!lTree.nullable) {
            for (int i = 0; i < tempVect.size(); i++) {
                newTree.firstpos.push_back(tempVect[i]);
            }
        }
        else {
            for (int i = 0; i < tempVect.size(); i++) {
                newTree.firstpos.push_back(tempVect[i]);
            }
            for (int i = 0; i < tempVect1.size(); i++) {
                newTree.firstpos.push_back(tempVect1[i]);
            }
        }

        if (!rTree.nullable) {
            for (int i = 0; i < tempVect3.size(); i++) {
                newTree.lastpos.push_back(tempVect3[i]);
            }
        }
        else {
            for (int i = 0; i < tempVect2.size(); i++) {
                newTree.lastpos.push_back(tempVect2[i]);
            }
            for (int i = 0; i < tempVect3.size(); i++) {
                newTree.lastpos.push_back(tempVect3[i]);
            }
        }
        sort (newTree.firstpos.begin(), newTree.firstpos.end());
        sort (newTree.lastpos.begin(), newTree.lastpos.end());
        auto last = std::unique(newTree.firstpos.begin(), newTree.firstpos.end());
        auto pred = std::unique(newTree.lastpos.begin(), newTree.lastpos.end());
        newTree.firstpos.erase(last, newTree.firstpos.end());
        newTree.lastpos.erase(pred, newTree.lastpos.end());
        for (int j = 0; j < lTree.lastpos.size(); j++) {
            short tempJ = lTree.lastpos[j];
            std::vector<int> tempVect5 = {rTree.firstpos.begin(), rTree.firstpos.end()};
            for (int z = 0; z < tempVect5.size(); z++) {
                followpos[tempJ].followpos.push_back(tempVect5[z]);
            }
            sort (followpos[tempJ].followpos.begin(), followpos[tempJ].followpos.end());
            auto last = std::unique(followpos[tempJ].followpos.begin(), followpos[tempJ].followpos.end());
            followpos[tempJ].followpos.erase(last, followpos[tempJ].followpos.end());
        }

        return newTree;
    }
    Tree checkIterat(Tree tree) {

        for (int j = 0; j < tree.lastpos.size(); j++) {
            short i = tree.lastpos[j];
            std::vector<int> tempVect4 = {tree.firstpos.begin(), tree.firstpos.end()};
            for (int z = 0; z < tempVect4.size(); z++) {
                followpos[i].followpos.push_back(tempVect4[z]);
            }
            sort (followpos[i].followpos.begin(), followpos[i].followpos.end());
            auto last = std::unique(followpos[i].followpos.begin(), followpos[i].followpos.end());
            followpos[i].followpos.erase(last, followpos[i].followpos.end());
        }

        Tree treeN = Tree();
        treeN.firstpos = tree.firstpos;
        treeN.lastpos = tree.lastpos;
        treeN.nullable = true;

        return treeN;
    }
    Tree BrackCharacter()
    {
        if (alphabet->has_char(currentElement)) {
            Tree tree = Tree();
            tree.firstpos.push_back(followpos.size());
            sort (tree.firstpos.begin(), tree.firstpos.end());
            tree.lastpos.push_back(followpos.size());
            sort (tree.lastpos.begin(), tree.lastpos.end());
            auto last = std::unique(tree.lastpos.begin(), tree.lastpos.end());
            auto pred = std::unique(tree.firstpos.begin(), tree.firstpos.end());
            tree.lastpos.erase(last, tree.lastpos.end());
            tree.firstpos.erase(pred, tree.firstpos.end());
            tree.nullable = false;
            followpos.push_back({std::vector<short>(), currentElement});
            newElem();

            return tree;
        }
        else if (currentElement == '(') {
            newElem();

            return fullTree();
        }
        else {
            Tree tree = Tree();
            tree.firstpos.clear();
            tree.lastpos.clear();
            tree.nullable = true;

            return tree;
        }
    }
    Tree boolIter() {
        Tree tree = BrackCharacter();

        if (currentElement == '*') {
            newElem();
            tree = checkIterat(tree);
        }

        return tree;
    }

    Tree multiplication() {
        Tree lTree = boolIter();

        while (currentElement == '(' or alphabet->has_char(currentElement)) {

            lTree = checkAnd(lTree, boolIter());
        }
        return lTree;
    }
};

DFA re2dfa(const std::string &s) {
    Automat parser = Automat(s);
    parser.getAut();
    return *parser.autom;
}

int main() {
    std::ifstream infile("re2dfa.in");
    std::ofstream outfile("re2dfa.out");


    std::string line;
    std::getline(infile, line);
    outfile << re2dfa(line).to_string();
    return 0;
}
