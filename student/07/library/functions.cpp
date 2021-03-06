/*
####################################################################
# TIE-02207 Programming 2: Basics, S2019                           #
#                                                                  #
# Project: Library                                                 #
# Program description: Program for managing library reservations   #
#                                                                  #
# File: functions.cpp                                              #
# Description: All major functions of the program.                 #
#                                                                  #
# Author: Valtteri Kirsilä, 255342, valtteri.kirsila@tuni.fi       #
####################################################################
*/
#include <iostream>
#include <vector>
#include <cstdlib>
#include <map>
#include <string>
#include <fstream>
#include <algorithm>
#include "functions.hh"
#include "library.hh"

using namespace std;

void command_error(string command) {
    // Prints the chosen error message
    // Parameters: string command
    cout << "Error: error in command " << command << endl;
}
// Split-function given in week 6.
vector<string> split(const string& s, const char delimiter, bool ignore_empty = true){
    vector<string> result;
    string tmp = s;
    while(tmp.find(delimiter) != string::npos)
    {
        string new_part = tmp.substr(0, tmp.find(delimiter));
        tmp = tmp.substr(tmp.find(delimiter)+1, tmp.size());
        if(not (ignore_empty and new_part.empty()))
        {
            result.push_back(new_part);
        }
    }
    if(not (ignore_empty and tmp.empty()))
    {
        result.push_back(tmp);
    }
    return result;
}


// combine vector strings to string
string combine(vector<string> words) {
    string title;
    for (size_t i=0; i<words.size();i++) {
        string word = words[i];

        // Remove " marks from start and begin
        if (i == 0 && word.front() == '"')
            word.erase(word.begin(), word.begin()+1);

        if (i == words.size()-1 && word.back() == '"')
            word.erase(word.end()-1, word.end());

        // Join words by space
        if (i == 0)
            title += word;
        else
            title += " " + word;
    }
    return title;
}

// If book is on the shelf, its reservation count is 0. This function turns that string into an integer.
int numReservations(string str) {
    if(str == "on-the-shelf")
        return 0;
    else
        return atoi(str.c_str());
}

int getMinReservations(map<string, Library> libraries, string title) {
    int minReservations = 9999;

    for (auto library : libraries) {
        for (auto book : library.second.books) {
            if(book.reservations < minReservations && book.title == title)
                minReservations = book.reservations;
        }
    }
    return minReservations;
}

// Read row and return possible exception as a boolean
bool read_row(string row, map<string, Library>& libraries) {
    vector<string> result = split(row, ';');
    if(result.size() != 4) {
        cout << "Error: empty field" << endl;
        return false;
    }
    string libraryName = result.at(0);
    string author = result.at(1);
    string title = result.at(2);
    int reservations = numReservations(result.at(3));
    Book book = {title, author, reservations};

    if(libraries.find(libraryName) != libraries.end()) {
        Library& library = libraries.at(libraryName);
        library.addBook(book);
    } else {
        // Library was not found from libraries
        // Create new library and add book into it
        Library library(libraryName);
        library.addBook(book);
        libraries.insert(make_pair(libraryName, library));
    }
    return true;
}

bool read_file(map<string, Library>& libraries) {
    string inputName, row;
    std::cout << "Input file: ";
    getline(std::cin, inputName);
    ifstream input(inputName);

    if (!input) {
        std::cout << "Error: the input file cannot be opened" << std::endl;
        return false;
    }

    while ( getline(input, row) ) {
        bool rowSuccess = read_row(row, libraries);
        if(not rowSuccess)
            return false;
    }
    input.close();
    return true;
}

vector<Book> getBooks(map<string, Library> libraries) {
    vector<Book> books;
    for (auto& library : libraries) {
        for (auto& book : library.second.books) {
            books.push_back(book);
        }
    }
    sort(books.begin(), books.end());
    return books;
}



void printLibraries(map<string, Library> libraries) {
    // print all libraries
    for (auto& library : libraries) {
        cout << library.first << endl;
    }
}

void printReservable(map<string, Library> libraries, string title) {
    int minReservations = getMinReservations(libraries, title);

    // no books by title in lib (minReservations has initial value)
    if (minReservations == 9999) {
        cout << "Book is not a library book." << endl;
        return;
    } else if (minReservations >= 100) {
        cout << "The book is not reservable from any library." << endl;
        return;
    }

    if (minReservations == 0)
        cout << "on the shelf" << endl;
    else
        cout << minReservations << " reservations" << endl;

    for (auto& library : libraries) {
        for (auto& book : library.second.books) {
            if(book.reservations == minReservations && book.title == title)
                cout << "--- " << library.first << endl;
        }
    }
}

void Loanable(vector<Book> books) {
    for (auto book : books) {
        if(book.reservations == 0)
            cout << book.author << ": " << book.title << endl;
    }
}

bool startUI(map<string, Library> libraries) {
    // UI asks inputVector until 'quit' is given
    // Checks that given command is available and has the proper amount of parameters
    // returns EXIT_SUCCESS boolean when the user wants to close program
    // Parameters: map<string, Library> books

    bool quit = false;
    string input;
    vector<string> inputVector;
    while ( !quit ) {
        // for the input UI
        cout << "> ";
        getline( cin, input );
        inputVector = split(input,' ');

        // If empty line -> continue
        if ( inputVector.size() == 0) {
            continue;

        // Close on quit
        } else if (inputVector.at(0) == "quit"  ) {
            quit = true;

        } else if (inputVector.at(0) == "libraries"  ) {
            if ( inputVector.size() != 1 ) {
                command_error("libraries");
            } else {
                printLibraries(libraries);
            }
        } else if ( inputVector.at(0) == "books") {

            if ( inputVector.size() != 3 ) {
                command_error("books");
            }else{

            string libraryName = inputVector.at(1);
            string author = inputVector.at(2);

            try {
                Library library = libraries.at(libraryName);
                library.printBooks(author);
            } catch(const out_of_range& oor) {
                cout << "Error: unknown library name" << endl;
            }
            }
        } else if ( inputVector.at(0) == "material") {

            string libraryName = inputVector.at(1);

             try {
                 Library library = libraries.at(libraryName);
                 library.printMaterial();
             } catch(const out_of_range& oor) {
                 cout << "Error: unknown library name" << endl;

             }
        } else if ( inputVector.at(0) == "reservable" ) {

            // reservable has 2 components
            // checks for the shortest reservation line for requested book

            inputVector.erase(inputVector.begin(), inputVector.begin()+1);
            string title = combine(inputVector);
            printReservable(libraries, title);


        } else if ( inputVector.at(0) == "loanable" ) {

            // loanable has 1 component
            if ( inputVector.size() != 1 ) {
                command_error( "loanable" );

            } else {
                vector<Book> books = getBooks(libraries);
                Loanable(books);
            }

        } else {
            // If none predetermined commands match print an error
            cout << "Error: Unknown command: " << inputVector.at(0) << endl;
        }
    }
    return EXIT_SUCCESS;
}
