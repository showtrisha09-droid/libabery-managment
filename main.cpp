#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <limits>

using namespace std;

// Base class
class LibraryItem {
public:
    virtual void display() const = 0;
    virtual ~LibraryItem() {}
};

// Book class
class Book : public LibraryItem {
private:
    int bookID;
    string title;
    string author;
    bool available;

public:
    Book() {
        bookID = 0;
        available = true;
    }

    Book(int id, string t, string a, bool status = true) {
        bookID = id;
        title = t;
        author = a;
        available = status;
    }

    int getID() const {
        return bookID;
    }

    string getTitle() const {
        return title;
    }

    string getAuthor() const {
        return author;
    }

    bool isAvailable() const {
        return available;
    }

    void issueBook() {
        if (!available)
            throw runtime_error("Book is already issued.");
        available = false;
    }

    void returnBook() {
        if (available)
            throw runtime_error("Book is already available.");
        available = true;
    }

    void display() const override {
        cout << bookID << " | " << title
             << " | " << author << " | "
             << (available ? "Available" : "Issued") << endl;
    }
};

// File handling class
class FileManager {
public:
    static void saveBooks(const vector<Book>& books) {
        ofstream file("books.csv");

        if (!file)
            throw runtime_error("Unable to save book records.");

        for (const Book& b : books) {
            file << b.getID() << ","
                 << b.getTitle() << ","
                 << b.getAuthor() << ","
                 << b.isAvailable() << endl;
        }

        file.close();
    }

    static void loadBooks(vector<Book>& books) {
        ifstream file("books.csv");

        if (!file)
            return;

        string line;

        while (getline(file, line)) {
            stringstream ss(line);
            string id, title, author, status;

            if (getline(ss, id, ',') &&
                getline(ss, title, ',') &&
                getline(ss, author, ',') &&
                getline(ss, status, ',')) {

                try {
                    int bookID = stoi(id);
                    bool available = (status == "1");

                    books.push_back(
                        Book(bookID, title, author, available)
                    );
                }
                catch (...) {
                    cout << "Invalid record skipped." << endl;
                }
            }
        }

        file.close();
    }
};

// Library management class
class Library {
private:
    vector<Book> books;

    Book* findBook(int id) {
        for (Book& b : books) {
            if (b.getID() == id)
                return &b;
        }
        return NULL;
    }

public:
    Library() {
        FileManager::loadBooks(books);
    }

    void addBook() {
        int id;
        string title, author;

        cout << "Enter Book ID: ";
        cin >> id;

        if (id <= 0)
            throw invalid_argument("Book ID must be positive.");

        if (findBook(id))
            throw invalid_argument("Book ID already exists.");

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter Book Title: ";
        getline(cin, title);

        cout << "Enter Author Name: ";
        getline(cin, author);

        if (title.empty() || author.empty())
            throw invalid_argument("Title and author cannot be empty.");

        if (title.find(',') != string::npos ||
            author.find(',') != string::npos)
            throw invalid_argument("Commas are not allowed in text.");

        books.push_back(Book(id, title, author));

        FileManager::saveBooks(books);

        cout << "Book added successfully!" << endl;
    }

    void searchBook() {
        int id;
        cout << "Enter Book ID: ";
        cin >> id;

        Book* b = findBook(id);

        if (b)
            b->display();
        else
            cout << "Book not found." << endl;
    }

    void displayBooks() const {
        if (books.empty()) {
            cout << "No books available." << endl;
            return;
        }

        cout << "\nID | Title | Author | Status\n";
        cout << "----------------------------------------\n";

        for (const Book& b : books)
            b.display();
    }

    void issueBook() {
        int id;
        cout << "Enter Book ID to issue: ";
        cin >> id;

        Book* b = findBook(id);

        if (!b)
            throw runtime_error("Book ID not found.");

        b->issueBook();
        FileManager::saveBooks(books);

        cout << "Book issued successfully!" << endl;
    }

    void returnBook() {
        int id;
        cout << "Enter Book ID to return: ";
        cin >> id;

        Book* b = findBook(id);

        if (!b)
            throw runtime_error("Book ID not found.");

        b->returnBook();
        FileManager::saveBooks(books);

        cout << "Book returned successfully!" << endl;
    }
};

// Main function
int main() {
    Library library;
    int choice;

    cout << "====================================\n";
    cout << "       LIBRARY MANAGEMENT SYSTEM\n";
    cout << "====================================\n";

    do {
        cout << "\n1. Add Book";
        cout << "\n2. Search Book";
        cout << "\n3. Display All Books";
        cout << "\n4. Issue Book";
        cout << "\n5. Return Book";
        cout << "\n0. Exit";
        cout << "\nEnter your choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Enter a number.\n";
            continue;
        }

        try {
            switch (choice) {
                case 1:
                    library.addBook();
                    break;

                case 2:
                    library.searchBook();
                    break;

                case 3:
                    library.displayBooks();
                    break;

                case 4:
                    library.issueBook();
                    break;

                case 5:
                    library.returnBook();
                    break;

                case 0:
                    cout << "Thank you for using the system!\n";
                    break;

                default:
                    cout << "Invalid choice! Try again.\n";
            }
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }

    } while (choice != 0);

    return 0;
}
