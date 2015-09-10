/**
 * Typeface organiser.
 *
 * @file listfonts.cpp
 * @author Eamon Bohan
 * @version 1.2 09/09/15
 */

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <vector>
#include <fstream>
using namespace std;

/* Object that holds folder information before processing*/
class folder {
    private:
        string name; // What is this folder called?
        vector<string> filename; //What files are contained?
    public:
        string getName() {
            return name;
        }
        unsigned int getNumberOfFiles() {
            return filename.size();
        }
        string getFilename(int index) {
            return filename[index];
        }
        void setName(string setName) {
            name = setName;
        }
        void printName() {
            cout << name;
        }
        void insertFile(string newFile) {
            filename.push_back(newFile);
        }
};

/* Make int into string */
string intIntoString(int number) {
    stringstream ss;
    ss << number;
    string stringified = ss.str();
    return stringified;
}

/* Make string array with a delimiter */
vector<string> tokenise(const string str, char delimiter = ' ') {
    vector<string> result;
    unsigned int index = 0;
    string temp;
    while (index < str.size()) {
        if (str[index] != delimiter) {
            temp = temp + str[index];
        } else {
            result.push_back(temp);
            temp = "";
        }
        index++;
    }
    if ( (index = str.size()) ) { //get last word
        result.push_back(temp);
    }
    return result;
}

/* Normalise strings from struct dirent * variables */
string normaliseString(char* entry) {
    string contagious = "";
    string outputstring = contagious + entry;
    return outputstring;
}

/* Return true if the last letter of a string matches */
bool isLastChar(string str, char c) {
    if ( (str[str.length()-1]) == c) {
        return true;
    } else {
        return false;
    }
}

/* Return the content of a Fonts folder */
vector<string> listContents (const char *path) {
    vector<string> directorycontent; //save all list here

    DIR *pdir = NULL; //good practice setting pointers to null
    pdir = opendir (path); //"." is the current dir
    struct dirent * entry = NULL;

    if (pdir == NULL) { //if pdir did not initialise correctly
        printf ("Could not initialise pdir\n");
        exit(3);
    }

    while ( (entry = readdir(pdir)) ) { //while ther's still something in the dir
        if (entry == NULL) {       //if pent did not initialise correctly
            printf("Could not initialise ENTRY VAR\n");
            exit(3);
        }

        if (entry->d_type != DT_DIR) { //if it works, save !FOLDERS to an array of strings
            directorycontent.push_back(normaliseString(entry->d_name));
        }
    }
    closedir(pdir); //close directory
    return directorycontent;
}

/* Return the tree of folders and files */
vector<folder> listTree (const char *path) {

    DIR *pdir = NULL; //good practice setting pointers to null
    pdir = opendir (path); //"." refers to the current dir
    struct dirent * entry = NULL;
    if (pdir == NULL) { //if pdir did not initialise correctly
        printf ("Could not initialise pdir\n");
        exit(3); //exit program, status 1 (failed exec)
    }

    vector<folder> dataTree; //save directory structure here

    while ( (entry = readdir(pdir)) ) { //while ther's still something in the dir
        if (entry == NULL) {       //if pent did not initialise correctly
            printf("Could not initialise ENTRY\n");
            exit(3);
        }

        folder currentfolder; //above worked: save to an array of strings

        if ( (entry->d_type == DT_DIR) ) { //if this is a folder
            string newDirectory = path;
            newDirectory = newDirectory + "\\" + normaliseString(entry->d_name); // make path for entering folder

            if (!isLastChar(newDirectory, '.')) { //if a folder, but not root directories
                currentfolder.setName(normaliseString(entry->d_name)); //save folder name
                vector<string> filelist = listContents( newDirectory.c_str() ); // convert string to a char * before entering!

                for (unsigned int x = 0; x < filelist.size(); x++) {
                    currentfolder.insertFile(filelist[x]);
                }
                dataTree.push_back(currentfolder); // save this folder with its file list
            }
        }
    }
    closedir(pdir); //close directory
    return dataTree;
}

/* Typeface object */
class typeface {
    private:
        string name; //typeface name
        string sort; // is it an ORIG or DSIG?
        vector<string> fonts; //fonts present in this group
    public:
        void setName(string newName) {
            name = newName;
        }
        void setSort(string newSort) {
            sort = newSort;
        }
        void insertNewFont(string newFont) {
            fonts.push_back(newFont);
        }
        string getName() {
            return name;
        }
        string getSort() {
            return sort;
        }
        unsigned int getNumberOfFonts() {
            return fonts.size();
        }
        string getAllFonts() {
            string tabs = "\t\t";
            string allfonts = "";
            for (unsigned int x = 0; x < fonts.size(); x++) {
                allfonts = allfonts + tabs + fonts[x] + "\r\n";
            }
            return allfonts;
        }
};

/* Foundry that owns several typefaces */
class foundry {
    private:
        string name; //What is the name of the foundry?
        vector<typeface> productline; // list of typefaces foundry owns
    public:
        string getName() {
            return name;
        }
        typeface getProduct(unsigned int index) {
            return productline[index];
        }
        unsigned int getNumberOfProducts() {
            return productline.size();
        }
        void setName(string newName) {
            name = newName;
        }
        void insertNewProduct(typeface newTypeface) {
            productline.push_back(newTypeface);
        }
        string listFonts(unsigned int x) {
            string fontlist = productline[x].getAllFonts();
            return fontlist;
        }
        string listAllProducts() {
            string list = "";
            for (unsigned int x = 0; x < productline.size(); x++) {
                list = list + "\t" + productline[x].getName()
                    + "\r\n\t(type: " + productline[x].getSort() + ")\r\n";
                list = list + listFonts(x);
            }
            return list;
        }
};

/* Finds the foundry name in the folder array */
string findFoundryName(vector<string> tokenisedfolder) {
    string foundryname;
    for (int x = (tokenisedfolder.size()-1); x >= 0; x--) { //goes in reverse
        if (tokenisedfolder[x] != "by") { // stop when "by" is found
            foundryname = tokenisedfolder[x] + " " + foundryname;
        } else {
            x = 0;
        }
    }
    if (!isLastChar(foundryname, ')')) { //remove last character
        foundryname = foundryname.substr(0, (foundryname.size()-2));
    }
    return foundryname;
}

/* Finds the typeface name in the folder name */
string findTypefaceName(vector<string> tokenisedfolder) {
    string typefacename;
    for (unsigned int x = 0; x < (tokenisedfolder.size()-1); x++) {
        if (tokenisedfolder[x][0] != '(') {
            typefacename = typefacename + tokenisedfolder[x];
        } else {
            x = tokenisedfolder.size();
        }
    }
    return typefacename;
}

/* Finds the type of typeface as described in the folder */
string findTypefaceType(vector<string> tokenisedfolder) {
    string typefacetype = "none";
    for (unsigned int x = 0; x < tokenisedfolder.size(); x++) {
        if (tokenisedfolder[x][0] == '(') {
            typefacetype = tokenisedfolder[x].substr(1, (tokenisedfolder[x].size() ) );
            return typefacetype;
        }
    }
    return typefacetype;
}

/* Make a database of foundries, man! */
vector<foundry> createFoundry(vector<folder> datatree) {
    vector<foundry> allfoundries; // where the mother lode goes

    for (unsigned int x = 0; x < datatree.size(); x++) { //for every folder in the DataTree
        string currentfoundry = findFoundryName( tokenise( datatree[x].getName() ) ); //split foldername into array, find name
        string currenttypeface = findTypefaceName ( tokenise( datatree[x].getName() ) ); //split foldername, find typeface
        string currenttypefacetype = findTypefaceType ( tokenise ( datatree[x].getName() ) ); //find class of typeface

        int foundryindex = -1; // -1 for false, 0 and onwards for index in array
        for (unsigned int y = 0; y < allfoundries.size(); y++) { //search for preexisting foundry of same name as one in current folder
            if (currentfoundry == allfoundries[y].getName() ) {// see if foundry name matches anywhere in the set
                foundryindex = y;           //get index position
                y = allfoundries.size(); //break from loop
            }
        }

        typeface newtypeface; // make new typeface object with all its fonts
        newtypeface.setName(currenttypeface);
        newtypeface.setSort(currenttypefacetype);
        for (unsigned int z = 0; z < datatree[x].getNumberOfFiles(); z++) {
            newtypeface.insertNewFont(datatree[x].getFilename(z));
        }

        if ( (foundryindex == -1) ) { // if the foundry is not present already
            foundry newfoundry;
            newfoundry.setName(currentfoundry);
            newfoundry.insertNewProduct(newtypeface);
            allfoundries.push_back(newfoundry); //create foundry in new index
        } else { // if the foundry was found
            allfoundries[foundryindex].insertNewProduct(newtypeface);
        }
    }
    return allfoundries;
}

/**
 * The main method for the application.
 */
int main() {

    // Where our results will be stored
    vector<foundry> entirety;

    // Make a structure of folders and files
    vector<folder> datatree = listTree(".\\Fonts");

    // Make a database of foundries, man!
    entirety = createFoundry(datatree);

    //create stream
    ofstream myFile;

    myFile.open ("font_list.txt");

    myFile << "List all the contents found:\r\n";

    // loop through database
    for (unsigned int x = 0; x < entirety.size(); x++) {
        myFile << "Foundry name:\n\t" << entirety[x].getName() << "\r\n";
        myFile << "Typeface list:\r\n" << entirety[x].listAllProducts() << "\r\n";
    }

    myFile.close();
    return 0;
}
