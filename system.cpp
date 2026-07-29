//[invntory management system] 

// Visual Studio marks ctime() (used for the dashboard's date/time) as
// "unsafe" and suggests ctime_s() instead. ctime_s() is Windows-only and
// not standard C++, so instead we simply tell MSVC not to enforce that
// warning-as-error for this file. Must be defined before any include.
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <ctime>
#include <limits>
#include <cctype>
#include <climits>

// windows.h gives us Sleep() and Beep(). On non-Windows machines (used only
// for testing/grading on this machine) we use the matching Linux function
// so the program still compiles and the animations still work.
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

// CONSTANTS & GLOBALS 

const int MAX_PRODUCTS = 100;

struct Product
{
    int id;
    string name;
    double price;
    int quantity;
};

Product products[MAX_PRODUCTS];
int productCount = 0;
double totalSalesValue = 0.0; // tracks revenue from sellProduct(), saved to file

// For Undo Delete
Product lastDeletedProduct;
bool canUndoDelete = false;
int deletedIndex = -1;

// Recent activity log (latest 5)
const int RECENT_MAX = 5;
string recentActivities[RECENT_MAX];
int recentIndex = 0; // next position to write (circular)
int recentCount = 0; // how many stored (<= RECENT_MAX)

// ANSI color codes used for a colorful, professional console UI.
const string COLOR_RESET = "\033[0m";
const string COLOR_BLUE = "\033[34m";
const string COLOR_GREEN = "\033[32m";
const string COLOR_YELLOW = "\033[33m";
const string COLOR_RED = "\033[31m";
const string COLOR_CYAN = "\033[36m";
const string COLOR_MAGENTA = "\033[35m";
const string COLOR_WHITE = "\033[37m";

// HELPER FUNCTIONS 

// Trim helpers
string ltrim(const string& s)
{
    size_t i = 0;
    while (i < s.size() && isspace((unsigned char)s[i])) ++i;
    return s.substr(i);
}
string rtrim(const string& s)
{
    if (s.empty()) return s;
    int i = (int)s.size() - 1;
    while (i >= 0 && isspace((unsigned char)s[i])) --i;
    return s.substr(0, i + 1);
}
string trim(const string& s)
{
    return rtrim(ltrim(s));
}

// Clears the console screen
void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Pauses execution for a short number of milliseconds (used by loadingAnimation)
void sleepMs(int milliseconds)
{
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

// Waits for the user to press Enter before continuing
void pauseScreen()
{
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Prints a horizontal separator line
void printLine()
{
    cout << "----------------------------------------------------\n";
}

// Prints a breadcrumb-like header for navigation consistency
void showBreadcrumb(const string& page)
{
    cout << COLOR_MAGENTA << "Home";
    if (!page.empty()) cout << " > " << page;
    cout << COLOR_RESET << "\n";
    printLine();
}

// Prints a message in Blue -- used for section titles
void showTitle(string text)
{
    cout << COLOR_BLUE << text << COLOR_RESET << "\n";
}

// Prints a message in Green and plays a short success sound
void showSuccess(string text)
{
    cout << COLOR_GREEN << text << COLOR_RESET << "\n";
    cout << '\a';
}

// Prints a message in Red and plays a short error sound
void showError(string text)
{
    cout << COLOR_RED << text << COLOR_RESET << "\n";
    cout << '\a';
}

// Prints a message in Yellow -- used for warnings
void showWarning(string text)
{
    cout << COLOR_YELLOW << text << COLOR_RESET << "\n";
}

// Prints a message in Cyan -- used for general information
void showInfo(string text)
{
    cout << COLOR_CYAN << text << COLOR_RESET << "\n";
}

// Shows a short animated progress bar with a label.
void loadingAnimation(string label)
{
    cout << COLOR_CYAN << label << COLOR_RESET << "\n";

    for (int i = 0; i < 20; i++)
    {
        cout << "#";
        cout.flush();
        sleepMs(25);
    }

    cout << "\n" << COLOR_GREEN << "Done." << COLOR_RESET << "\n\n";
}

// Centralized Validation Helpers 

// Read a full line from stdin and return it trimmed.
// Keeps reading until a line is returned (never mix cin >> and getline).
string getLineTrimmed()
{
    string line;
    getline(cin, line);
    return trim(line);
}

// Validate integer input from text. Repeats until valid integer in range.
int getValidatedInt(const string& message, int minValue, int maxValue)
{
    while (true)
    {
        cout << message;
        string line = getLineTrimmed();
        if (line.empty())
        {
            showWarning("Input cannot be empty. Please try again.");
            continue;
        }

        // Check for invalid characters: allow optional leading + or -, then digits
        int start = 0;
        if (line[0] == '+' || line[0] == '-') start = 1;
        bool ok = true;
        if (start >= (int)line.size()) ok = false;
        for (int i = start; i < (int)line.size(); ++i)
        {
            if (!isdigit((unsigned char)line[i])) { ok = false; break; }
        }
        if (!ok)
        {
            showWarning("Invalid input. Please enter a whole number.");
            continue;
        }

        // Parse safely
        long long value = 0;
        try
        {
            value = stoll(line);
        }
        catch (...)
        {
            showWarning("Number out of range. Please try again.");
            continue;
        }

        if (value < minValue || value > maxValue)
        {
            showWarning("Invalid choice. Please try again.");
            continue;
        }

        return (int)value;
    }
}

// Validate double input (floating point). Repeats until valid number >= minValue.
double getValidatedDouble(const string& message, double minValue)
{
    while (true)
    {
        cout << message;
        string line = getLineTrimmed();
        if (line.empty())
        {
            showWarning("Input cannot be empty. Please try again.");
            continue;
        }

        // Validate characters: allow digits, one dot, optional leading +/-
        int dotCount = 0;
        int start = 0;
        if (line[0] == '+' || line[0] == '-') start = 1;
        bool ok = true;
        if (start >= (int)line.size()) ok = false;
        for (int i = start; i < (int)line.size(); ++i)
        {
            if (line[i] == '.')
            {
                dotCount++;
                if (dotCount > 1) { ok = false; break; }
            }
            else if (!isdigit((unsigned char)line[i]))
            {
                ok = false; break;
            }
        }
        if (!ok)
        {
            showWarning("Invalid input. Please enter a number.");
            continue;
        }

        double value = 0.0;
        try
        {
            value = stod(line);
        }
        catch (...)
        {
            showWarning("Invalid number. Please try again.");
            continue;
        }
        if (value < minValue)
        {
            showWarning("Value is below the minimum allowed. Please try again.");
            continue;
        }
        return value;
    }
}

// Validate yes/no input: returns 'y' or 'n'
char getValidatedYesNo(const string& message)
{
    while (true)
    {
        cout << message << " (y/n): ";
        string line = getLineTrimmed();
        if (line.size() != 1)
        {
            showWarning("Invalid choice. Please enter 'y' or 'n'.");
            continue;
        }
        char c = line[0];
        if (c == 'y' || c == 'Y') return 'y';
        if (c == 'n' || c == 'N') return 'n';
        showWarning("Invalid choice. Please enter 'y' or 'n'.");
    }
}

// General menu choice between minChoice and maxChoice inclusive.
int getMenuChoice(const string& title, int minChoice, int maxChoice)
{
    string prompt = title;
    if (!title.empty() && title.back() != ' ') prompt += " ";
    prompt += "(" + to_string(minChoice) + "-" + to_string(maxChoice) + "): ";
    return getValidatedInt(prompt, minChoice, maxChoice);
}

// Read non-empty line for names and text input
string getNonEmptyLine(const string& prompt)
{
    while (true)
    {
        cout << prompt;
        string line = getLineTrimmed();
        if (line.empty())
        {
            showWarning("Input cannot be empty. Please try again.");
            continue;
        }
        return line;
    }
}

// End Validation Helpers 

// Logs a short activity message with timestamp into the recentActivities circular buffer
void logActivity(const string& message)
{
    time_t now = time(0);
    char* dt = ctime(&now);
    string timestr(dt ? dt : "");
    if (!timestr.empty() && timestr.back() == '\n') timestr.pop_back();

    string entry = timestr + " - " + message;
    recentActivities[recentIndex] = entry;
    recentIndex = (recentIndex + 1) % RECENT_MAX;
    if (recentCount < RECENT_MAX) recentCount++;
}

// Shows last up to RECENT_MAX recent activities on the dashboard (most recent first)
void showRecentActivities()
{
    if (recentCount == 0)
    {
        cout << COLOR_CYAN << "Recent Activity   : " << COLOR_RESET << "No recent activity.\n";
        return;
    }

    cout << COLOR_CYAN << "Recent Activity   : " << COLOR_RESET << "\n";
    int shown = 0;
    for (int i = 0; i < recentCount && shown < RECENT_MAX; i++)
    {
        int idx = (recentIndex - 1 - i);
        while (idx < 0) idx += RECENT_MAX;
        cout << " - " << recentActivities[idx] << "\n";
        shown++;
    }
}

// Returns the stock status text based on quantity
string getStatus(int quantity)
{
    if (quantity == 0)
        return "Out Of Stock";
    else if (quantity < 5)
        return "Low Stock";
    else
        return "Available";
}

// Returns the color that matches a product's stock status
string getStatusColor(int quantity)
{
    if (quantity == 0)
        return COLOR_RED;
    else if (quantity < 5)
        return COLOR_YELLOW;
    else
        return COLOR_GREEN;
}

// Prints one product row in table format (used by display, search & filter)
void printProductRow(const Product& p)
{
    cout << left << setw(8) << p.id
        << setw(20) << p.name
        << setw(12) << fixed << setprecision(2) << p.price
        << setw(10) << p.quantity
        << getStatusColor(p.quantity) << setw(15) << getStatus(p.quantity) << COLOR_RESET << "\n";
}

// Prints the table header (used by display, search & filter)
void printTableHeader()
{
    cout << COLOR_WHITE
        << left << setw(8) << "ID"
        << setw(20) << "Name"
        << setw(12) << "Price"
        << setw(10) << "Quantity"
        << setw(15) << "Status" << COLOR_RESET << "\n";
    printLine();
}

// Prints all products without pausing (helper used before operations)
void printAllProductsNoPause()
{
    if (productCount == 0)
    {
        cout << COLOR_CYAN << "No products available.\n" << COLOR_RESET;
        return;
    }
    printTableHeader();
    for (int i = 0; i < productCount; i++)
    {
        printProductRow(products[i]);
    }
}

// SEARCH FUNCTIONS (NO PRINTING) 

// Returns index of product with given ID, or -1 if not found
int searchByID(int id)
{
    for (int i = 0; i < productCount; i++)
    {
        if (products[i].id == id)
            return i;
    }
    return -1;
}

// Returns index of product with exact, case-sensitive name match, or -1
int searchByName(string name)
{
    for (int i = 0; i < productCount; i++)
    {
        if (products[i].name == name)
            return i;
    }
    return -1;
}

// FILE HANDLING (AUTO SAVE/LOAD) 

// Save format:
// Line1: totalSalesValue
// Line2: productCount
// Then for each product: id\nname\nprice\nquantity\n
bool writeDataToFile(const string& filename = "products.txt", bool verbose = false)
{
    ofstream out(filename.c_str());
    if (!out)
    {
        if (verbose) showError("Error: Could not open file for saving.");
        return false;
    }
    out << fixed << setprecision(2) << totalSalesValue << "\n";
    out << productCount << "\n";
    for (int i = 0; i < productCount; i++)
    {
        out << products[i].id << "\n";
        out << products[i].name << "\n";
        out << fixed << setprecision(2) << products[i].price << "\n";
        out << products[i].quantity << "\n";
    }
    out.close();
    if (verbose) showSuccess("Products saved successfully to products.txt!");
    logActivity("Inventory Saved");
    return true;
}

bool readDataFromFile(const string& filename = "products.txt", bool verbose = false)
{
    ifstream in(filename.c_str());
    if (!in)
    {
        if (verbose) showInfo("No saved file found. Starting with empty inventory.");
        return false;
    }

    string line;

    // totalSalesValue
    if (!getline(in, line)) { in.close(); if (verbose) showWarning("Saved file empty or corrupted."); return false; }
    try { totalSalesValue = stod(trim(line)); }
    catch (...) { totalSalesValue = 0.0; }

    // productCount
    if (!getline(in, line)) { productCount = 0; in.close(); return true; }
    try { productCount = stoi(trim(line)); }
    catch (...) { productCount = 0; }

    if (productCount < 0 || productCount > MAX_PRODUCTS) productCount = 0;

    for (int i = 0; i < productCount; i++)
    {
        // id
        if (!getline(in, line)) { productCount = i; break; }
        try { products[i].id = stoi(trim(line)); }
        catch (...) { products[i].id = 0; }

        // name (may contain spaces)
        if (!getline(in, products[i].name)) { products[i].name = ""; }

        // price
        if (!getline(in, line)) { products[i].price = 0.0; }
        else { try { products[i].price = stod(trim(line)); } catch (...) { products[i].price = 0.0; } }

        // quantity
        if (!getline(in, line)) { products[i].quantity = 0; }
        else { try { products[i].quantity = stoi(trim(line)); } catch (...) { products[i].quantity = 0; } }
    }

    in.close();
    if (verbose) showSuccess("Products loaded successfully!");
    logActivity("Inventory Loaded");
    return true;
}

void saveProducts()
{
    clearScreen();
    loadingAnimation("Saving Products...");
    writeDataToFile("products.txt", true);
}

void loadProducts()
{
    clearScreen();
    loadingAnimation("Loading Products...");
    readDataFromFile("products.txt", true);
}

void autoSave()
{
    writeDataToFile("products.txt", false);
}

// HELP & ABOUT 

void displayHelpCenter()
{
    clearScreen();
    showBreadcrumb("Help Center");
    showTitle("===== HELP CENTER =====");

    cout << COLOR_WHITE << "\nAdd Product\n" << COLOR_RESET;
    cout << "Add a new product into the inventory.\n\n";
    cout << COLOR_WHITE << "----------------------------------------\n" << COLOR_RESET;

    cout << COLOR_WHITE << "Display Products\n" << COLOR_RESET;
    cout << "Show all stored products.\n\n";
    cout << COLOR_WHITE << "----------------------------------------\n" << COLOR_RESET;

    cout << COLOR_WHITE << "Search Product\n" << COLOR_RESET;
    cout << "Search by ID or Name.\n\n";
    cout << COLOR_WHITE << "----------------------------------------\n" << COLOR_RESET;

    cout << COLOR_WHITE << "Update Product\n" << COLOR_RESET;
    cout << "Modify product information. You can update Name, Price, Quantity, ID, or all fields.\n\n";
    cout << COLOR_WHITE << "----------------------------------------\n" << COLOR_RESET;

    cout << COLOR_WHITE << "Delete Product\n" << COLOR_RESET;
    cout << "Remove a product from inventory. You can Undo the most recent delete.\n\n";
    cout << COLOR_WHITE << "----------------------------------------\n" << COLOR_RESET;

    cout << COLOR_WHITE << "Sell Product\n" << COLOR_RESET;
    cout << "Decrease product quantity after a sale. Total Sales updates automatically.\n\n";
    cout << COLOR_WHITE << "----------------------------------------\n" << COLOR_RESET;

    cout << COLOR_WHITE << "Restock Product\n" << COLOR_RESET;
    cout << "Increase product quantity.\n\n";
    cout << COLOR_WHITE << "----------------------------------------\n" << COLOR_RESET;

    cout << COLOR_WHITE << "Statistics\n" << COLOR_RESET;
    cout << "Display inventory statistics.\n\n";
    cout << COLOR_WHITE << "----------------------------------------\n" << COLOR_RESET;

    cout << COLOR_WHITE << "Sort\n" << COLOR_RESET;
    cout << "Sort products by Name, Price, or Quantity (ascending/descending).\n\n";
    cout << COLOR_WHITE << "----------------------------------------\n" << COLOR_RESET;

    cout << COLOR_WHITE << "Filter\n" << COLOR_RESET;
    cout << "Display Available, Low Stock, Out Of Stock, or products above a price.\n\n";
    cout << COLOR_WHITE << "----------------------------------------\n" << COLOR_RESET;

    cout << COLOR_WHITE << "Save / Load\n" << COLOR_RESET;
    cout << "Save inventory to file and load it back. Multi-word product names are supported.\n\n";
    cout << COLOR_WHITE << "----------------------------------------\n" << COLOR_RESET;

    cout << COLOR_WHITE << "Undo Delete\n" << COLOR_RESET;
    cout << "Restore the most recently deleted product (if available).\n\n";
    cout << COLOR_WHITE << "----------------------------------------\n" << COLOR_RESET;

    pauseScreen();
}

void displayAboutPage()
{
    clearScreen();
    showBreadcrumb("About");
    showTitle("===== ABOUT =====");

    cout << COLOR_CYAN << "========================================================\n" << COLOR_RESET;
    cout << COLOR_BLUE << "Inventory Management System\n\n" << COLOR_RESET;
    cout << "Version\n\n1.0\n\n";
    cout << "Language\n\nC++\n\n";
    cout << "Project Type\n\nConsole Application\n\n";
    cout << "Developed By\n\nKareem Reda\nYousef Abomosalam\nAhmed Bikhet\n\n";
    cout << "Organization\n\nMansoura Robotics Training Team\n\n";
    cout << COLOR_GREEN << "Thank you for using our Inventory Management System.\n" << COLOR_RESET;
    cout << COLOR_CYAN << "========================================================\n" << COLOR_RESET;

    pauseScreen();
}

// CORE MENU FUNCTIONS 

// Displays the dashboard, then the main menu
void displayMenu()
{
    clearScreen();

    // ---- Dashboard: compute live statistics from the products array ----
    int lowStockCount = 0;
    int outOfStockCount = 0;
    int availableCount = 0;
    double inventoryValue = 0.0;

    for (int i = 0; i < productCount; i++)
    {
        inventoryValue += products[i].price * products[i].quantity;

        if (products[i].quantity == 0)
            outOfStockCount++;
        else if (products[i].quantity < 5)
            lowStockCount++;
        if (products[i].quantity >= 5)
            availableCount++;
    }

    time_t now = time(0); // current date & time

    cout << COLOR_CYAN << "=========================================\n";
    cout << "            INVENTORY DASHBOARD\n";
    cout << "=========================================\n" << COLOR_RESET;
    cout << "Date & Time     : " << ctime(&now);
    cout << "Total Products  : " << productCount << "\n";
    cout << "Available       : " << availableCount << "\n";
    cout << COLOR_YELLOW << "Low Stock Items : " << lowStockCount << COLOR_RESET << "\n";
    cout << COLOR_RED << "Out Of Stock    : " << outOfStockCount << COLOR_RESET << "\n";
    cout << fixed << setprecision(2);
    cout << "Inventory Value : " << inventoryValue << "\n";
    cout << "Total Sales     : " << totalSalesValue << "\n";
    showRecentActivities();
    cout << COLOR_CYAN << "=========================================\n" << COLOR_RESET;

    // ---- Main Menu ----
    cout << COLOR_BLUE << "\n============ INVENTORY MANAGEMENT SYSTEM ============\n" << COLOR_RESET;
    printLine();
    cout << "1.  Add Product\n";
    cout << "2.  Display Products\n";
    cout << "3.  Search Product\n";
    cout << "4.  Update Product\n";
    cout << "5.  Delete Product\n";
    cout << "6.  Sell Product\n";
    cout << "7.  Restock Product\n";
    cout << "8.  Store Statistics\n";
    cout << "9.  Sort Products\n";
    cout << "10. Filter Products\n";
    cout << "11. Save Products\n";
    cout << "12. Load Products\n";
    cout << "13. Exit\n";
    printLine();
    cout << COLOR_CYAN << "(Bonus reports: 14 Total Sales | 15 Top 5 Expensive | 16 Top 5 Lowest Stock)\n" << COLOR_RESET;
    cout << COLOR_YELLOW << "\n17. Undo Delete (restore most recently deleted product)\n" << COLOR_RESET;
    cout << COLOR_CYAN << "\n(Additional screens: 31 Help Center | 32 About Page)\n" << COLOR_RESET;
}

// Adds a new product after validating all fields
void addProduct()
{
    bool repeat = true;
    while (repeat)
    {
        clearScreen();
        showBreadcrumb("Add Product");
        showTitle("===== ADD PRODUCT =====");

        if (productCount >= MAX_PRODUCTS)
        {
            showWarning("Inventory is full. Cannot add more products.");
            pauseScreen();
            return;
        }

        int id;
        while (true)
        {
            id = getValidatedInt("Enter Product ID (0 = Cancel): ", 0, INT_MAX);
            if (id == 0) return;
            if (id <= 0) { showWarning("ID must be positive."); continue; }
            if (searchByID(id) != -1) { showWarning("ID already exists. Enter a unique ID."); continue; }
            break;
        }

        // Name input with uniqueness validation and cancel support
        string name;
        while (true)
        {
            cout << "Enter Product Name (0 = Cancel): ";
            string entered = getLineTrimmed();
            if (entered == "0")
            {
                // Cancel adding product; return to Add Product menu (do not go to main menu)
                return;
            }
            if (entered.empty())
            {
                showWarning("Input cannot be empty. Please try again.");
                continue;
            }
            int existingIndex = searchByName(entered);
            if (existingIndex != -1)
            {
                showError("Product name already exists.");
                showInfo("Please enter a different name.");
                continue;
            }
            // unique name
            name = entered;
            break;
        }

        double price;
        while (true)
        {
            price = getValidatedDouble("Enter Product Price: ", 0.01);
            if (price <= 0) { showWarning("Price must be greater than zero."); continue; }
            break;
        }

        int quantity;
        while (true)
        {
            quantity = getValidatedInt("Enter Product Quantity: ", 0, INT_MAX);
            if (quantity < 0) { showWarning("Quantity cannot be negative."); continue; }
            break;
        }

        products[productCount].id = id;
        products[productCount].name = name;
        products[productCount].price = price;
        products[productCount].quantity = quantity;
        productCount++;

        logActivity("Added Product: " + name);
        autoSave();

        showSuccess("\nProduct added successfully!");

        cout << "\n1. Add Another Product\n2. Return to Main Menu\n";
        int ch = getMenuChoice("Choose", 1, 2);
        if (ch == 1) repeat = true;
        else repeat = false;
    }
}

// Displays all products in a professional formatted table
void displayProducts()
{
    clearScreen();
    showBreadcrumb("Display Products");
    showTitle("===== PRODUCT LIST =====");

    if (productCount == 0)
    {
        showInfo("No products available.");
        pauseScreen();
        return;
    }

    printAllProductsNoPause();
    pauseScreen();
}

// Lets the user search by ID or Name and displays the result (highlighted)
void searchProduct()
{
    bool repeatOuter = true;
    while (repeatOuter)
    {
        clearScreen();
        showBreadcrumb("Search Product");
        showTitle("===== SEARCH PRODUCT =====");
        cout << "1. Search By ID\n";
        cout << "2. Search By Name\n";
        cout << "3. Back\n";
        int choice = getMenuChoice("Choose an option", 1, 3);
        if (choice == 3) return;

        if (choice == 1)
        {
            // Perform a single ID search, then handle navigation.
            int id = getValidatedInt("Enter Product ID (0 = Cancel): ", 0, INT_MAX);
            if (id == 0) { /* go back to Search Menu */ continue; }

            int index = searchByID(id);
            if (index == -1)
            {
                showError("Product not found.");
                cout << "\n1. Search Again\n2. Return\n";
                int c = getMenuChoice("Choose", 1, 2);
                if (c == 1)
                {
                    // Return to the top Search Menu (user chooses ID or Name again)
                    continue;
                }
                else
                {
                    return; // return to main menu
                }
            }
            else
            {
                cout << "\n" << COLOR_MAGENTA << ">> Match Found <<" << COLOR_RESET << "\n";
                printTableHeader();
                printProductRow(products[index]);
                logActivity("Searched Product (ID): " + to_string(id));

                cout << "\n1. Search Again\n2. Return\n";
                int c = getMenuChoice("Choose", 1, 2);
                if (c == 1)
                {
                    // Return to the top Search Menu
                    continue;
                }
                else
                {
                    return;
                }
            }
        }
        else if (choice == 2)
        {
            // Perform a single Name search, then handle navigation.
            string name = getNonEmptyLine("Enter Product Name (exact, case-sensitive) (0 = Cancel): ");
            if (name == "0") { /* go back to Search Menu */ continue; }

            int index = searchByName(name);
            if (index == -1)
            {
                showError("Product not found.");
                cout << "\n1. Search Again\n2. Return\n";
                int c = getMenuChoice("Choose", 1, 2);
                if (c == 1)
                {
                    // Return to the top Search Menu (user chooses ID or Name again)
                    continue;
                }
                else
                {
                    return;
                }
            }
            else
            {
                cout << "\n" << COLOR_MAGENTA << ">> Match Found <<" << COLOR_RESET << "\n";
                printTableHeader();
                printProductRow(products[index]);
                logActivity("Searched Product (Name): " + name);

                cout << "\n1. Search Again\n2. Return\n";
                int c = getMenuChoice("Choose", 1, 2);
                if (c == 1)
                {
                    // Return to the top Search Menu
                    continue;
                }
                else
                {
                    return;
                }
            }
        }
    }
}
// Updates a product's Name, Price, Quantity or ID. 
void updateProduct()
{
    bool outerRepeat = true;
    while (outerRepeat)
    {
        clearScreen();
        showBreadcrumb("Update Product");
        showTitle("===== UPDATE PRODUCT =====");

        // Display products first
        printAllProductsNoPause();

        int id;
        int index;
        while (true)
        {
            id = getValidatedInt("Enter Product ID (0 = Cancel): ", 0, INT_MAX);
            if (id == 0) return;
            index = searchByID(id);
            if (index == -1)
            {
                showError("Product not found.");
                cout << "1. Try Again\n2. Return\n";
                int c = getMenuChoice("Choose", 1, 2);
                if (c == 1) continue;
                else return;
            }
            else break;
        }

        // Display selected product
        cout << "\nSelected Product:\n";
        printTableHeader();
        printProductRow(products[index]);

        cout << "\nWhat do you want to update?\n";
        cout << "1. Name\n2. Price\n3. Quantity\n4. Product ID\n5. Update All Information\n6. Cancel\n";
        int choice = getMenuChoice("Choose", 1, 6);

        if (choice == 6) return;

        if (choice == 1)
        {
            // Update Name with uniqueness check (ignore current product)
            while (true)
            {
                cout << "Enter New Name (0 = Cancel): ";
                string newName = getLineTrimmed();
                if (newName == "0") break; // cancel name update, return to update menu
                if (newName.empty()) { showWarning("Input cannot be empty. Please try again."); continue; }
                int found = searchByName(newName);
                if (found != -1 && found != index)
                {
                    showError("Product name already exists.");
                    showInfo("Please enter a different name.");
                    continue;
                }
                products[index].name = newName;
                logActivity("Updated Product Name (ID: " + to_string(products[index].id) + ")");
                break;
            }
        }
        else if (choice == 2)
        {
            double newPrice = getValidatedDouble("Enter New Price: ", 0.01);
            products[index].price = newPrice;
            logActivity("Updated Product Price (ID: " + to_string(products[index].id) + ")");
        }
        else if (choice == 3)
        {
            int newQty = getValidatedInt("Enter New Quantity: ", 0, INT_MAX);
            products[index].quantity = newQty;
            logActivity("Updated Product Quantity (ID: " + to_string(products[index].id) + ")");
        }
        else if (choice == 4)
        {
            while (true)
            {
                int newID = getValidatedInt("Enter New Product ID (0 = Cancel): ", 0, INT_MAX);
                if (newID == 0) break; // cancel ID update
                if (newID <= 0) { showWarning("ID must be positive."); continue; }
                if (newID == products[index].id) { showWarning("New ID is same as current ID."); continue; }
                if (searchByID(newID) != -1) { showWarning("ID already exists. Enter a unique ID."); continue; }
                products[index].id = newID;
                logActivity("Updated Product ID (to: " + to_string(newID) + ")");
                break;
            }
        }
        else if (choice == 5)
        {
            // Update All: name (unique), price, quantity, ID with validation. Allow cancelling name entry.
            // Name
            string newName;
            while (true)
            {
                cout << "Enter New Name (0 = Cancel Update All): ";
                string entered = getLineTrimmed();
                if (entered == "0")
                {
                    // Cancel the Update All operation (return to update menu)
                    newName = ""; // signal cancel
                    break;
                }
                if (entered.empty()) { showWarning("Input cannot be empty. Please try again."); continue; }
                int found = searchByName(entered);
                if (found != -1 && found != index)
                {
                    showError("Product name already exists.");
                    showInfo("Please enter a different name.");
                    continue;
                }
                newName = entered;
                break;
            }
            if (newName.empty())
            {
                // User cancelled Update All via name step
                continue;
            }

            double newPrice = getValidatedDouble("Enter New Price: ", 0.01);
            int newQty = getValidatedInt("Enter New Quantity: ", 0, INT_MAX);

            int newID = products[index].id;
            while (true)
            {
                newID = getValidatedInt("Enter New Product ID (current ID will remain if you enter 0): ", 0, INT_MAX);
                if (newID == 0) { newID = products[index].id; break; }
                if (newID <= 0) { showWarning("ID must be positive."); continue; }
                if (newID != products[index].id && searchByID(newID) != -1) { showWarning("ID already exists."); continue; }
                break;
            }

            products[index].name = newName;
            products[index].price = newPrice;
            products[index].quantity = newQty;
            products[index].id = newID;
            logActivity("Updated Product All Info (ID: " + to_string(products[index].id) + ")");
        }

        autoSave();
        showSuccess("\nProduct Updated Successfully");

        cout << "\n1. Update Another Product\n2. Return to Main Menu\n";
        int post = getMenuChoice("Choose", 1, 2);
        if (post == 1) outerRepeat = true;
        else outerRepeat = false;
    }
}

// Deletes a product after confirmation, shifting the array left
void deleteProduct()
{
    bool outerRepeat = true;
    while (outerRepeat)
    {
        clearScreen();
        showBreadcrumb("Delete Product");
        showTitle("===== DELETE PRODUCT =====");

        // Display products first
        printAllProductsNoPause();

        int id;
        int index;
        while (true)
        {
            id = getValidatedInt("Enter Product ID to delete (0 = Cancel): ", 0, INT_MAX);
            if (id == 0) return;
            index = searchByID(id);
            if (index == -1)
            {
                showError("Product not found.");
                cout << "1. Try Again\n2. Return\n";
                int c = getMenuChoice("Choose", 1, 2);
                if (c == 1) continue;
                else return;
            }
            else break;
        }

        cout << "\n";
        printTableHeader();
        printProductRow(products[index]);

        cout << "\nDelete this product?\n1. Yes\n2. No\n";
        int confirm = getMenuChoice("Choose", 1, 2);
        if (confirm == 2) return;

        // store deleted product for undo
        lastDeletedProduct = products[index];
        canUndoDelete = true;
        deletedIndex = index;

        for (int i = index; i < productCount - 1; i++)
        {
            products[i] = products[i + 1];
        }
        productCount--;

        logActivity("Deleted Product: " + lastDeletedProduct.name);
        autoSave();

        showSuccess("\nProduct Deleted Successfully");

        cout << "\n1. Undo Delete\n2. Delete Another Product\n3. Return to Main Menu\n";
        int post = getMenuChoice("Choose", 1, 3);
        if (post == 1)
        {
            // Undo immediately
            if (!canUndoDelete)
            {
                showInfo("No product available to undo.");
            }
            else
            {
                if (productCount >= MAX_PRODUCTS)
                {
                    showWarning("Cannot restore: inventory is full.");
                }
                else
                {
                    if (deletedIndex < 0 || deletedIndex > productCount)
                    {
                        products[productCount] = lastDeletedProduct;
                    }
                    else
                    {
                        for (int i = productCount; i > deletedIndex; i--)
                            products[i] = products[i - 1];
                        products[deletedIndex] = lastDeletedProduct;
                    }
                    productCount++;
                    canUndoDelete = false;
                    deletedIndex = -1;
                    logActivity("Undo Delete: " + lastDeletedProduct.name);
                    autoSave();
                    showSuccess("Product restored successfully!");
                }
            }
            pauseScreen();
            outerRepeat = false;
        }
        else if (post == 2) outerRepeat = true;
        else outerRepeat = false;
    }
}

// Undo the most recent delete (restores product to original position)
void undoDelete()
{
    clearScreen();
    showBreadcrumb("Undo Delete");
    showTitle("===== UNDO DELETE =====");

    if (!canUndoDelete)
    {
        showInfo("No deleted product to restore.");
        pauseScreen();
        return;
    }

    if (productCount >= MAX_PRODUCTS)
    {
        showWarning("Cannot restore product: inventory is full.");
        pauseScreen();
        return;
    }

    if (deletedIndex < 0 || deletedIndex > productCount)
    {
        products[productCount] = lastDeletedProduct;
    }
    else
    {
        for (int i = productCount; i > deletedIndex; i--)
            products[i] = products[i - 1];
        products[deletedIndex] = lastDeletedProduct;
    }

    productCount++;
    canUndoDelete = false;
    deletedIndex = -1;
    logActivity("Undo Delete: " + lastDeletedProduct.name);
    autoSave();
    showSuccess("Product restored successfully!");
    pauseScreen();
}

// Reduces stock when a product is sold, tracks revenue
void sellProduct()
{
    bool repeatOuter = true;
    while (repeatOuter)
    {
        clearScreen();
        showBreadcrumb("Sell Product");
        showTitle("===== SELL PRODUCT =====");

        // Display products first
        printAllProductsNoPause();

        int id;
        int index;
        while (true)
        {
            id = getValidatedInt("Enter Product ID (0 = Cancel): ", 0, INT_MAX);
            if (id == 0) return;
            index = searchByID(id);
            if (index == -1)
            {
                showError("Product not found.");
                cout << "1. Try Again\n2. Return\n";
                int c = getMenuChoice("Choose", 1, 2);
                if (c == 1) continue;
                else return;
            }
            else break;
        }

        if (products[index].quantity == 0)
        {
            showWarning("Out Of Stock. Selling is not possible.");
            cout << "\n1. Sell Another\n2. Main Menu\n";
            int ch = getMenuChoice("Choose", 1, 2);
            if (ch == 1) { repeatOuter = true; continue; }
            else return;
        }

        int amount;
        while (true)
        {
            amount = getValidatedInt("Enter quantity to sell: ", 1, products[index].quantity);
            if (amount <= 0) showWarning("Amount must be greater than zero.");
            else if (amount > products[index].quantity) showWarning("Not enough stock available.");
            else break;
        }

        products[index].quantity -= amount;
        totalSalesValue += amount * products[index].price;
        logActivity("Sold " + to_string(amount) + " x " + products[index].name);
        autoSave();

        showSuccess("\nSale completed successfully!");

        cout << "\n1. Sell Another Product\n2. Return to Main Menu\n";
        int post = getMenuChoice("Choose", 1, 2);
        if (post == 1) repeatOuter = true;
        else repeatOuter = false;
    }
}

// Increases stock for an existing product
void restockProduct()
{
    bool repeatOuter = true;
    while (repeatOuter)
    {
        clearScreen();
        showBreadcrumb("Restock Product");
        showTitle("===== RESTOCK PRODUCT =====");

        // Display products first
        printAllProductsNoPause();

        int id;
        int index;
        while (true)
        {
            id = getValidatedInt("Enter Product ID (0 = Cancel): ", 0, INT_MAX);
            if (id == 0) return;
            index = searchByID(id);
            if (index == -1)
            {
                showError("Product not found.");
                cout << "1. Try Again\n2. Return\n";
                int c = getMenuChoice("Choose", 1, 2);
                if (c == 1) continue;
                else return;
            }
            else break;
        }

        int amount;
        while (true)
        {
            amount = getValidatedInt("Enter quantity to restock: ", 1, INT_MAX);
            if (amount <= 0) showWarning("Amount must be greater than zero.");
            else break;
        }

        products[index].quantity += amount;
        logActivity("Restocked " + to_string(amount) + " x " + products[index].name);
        autoSave();

        showSuccess("\nProduct restocked successfully!");

        cout << "\n1. Restock Another Product\n2. Return to Main Menu\n";
        int post = getMenuChoice("Choose", 1, 2);
        if (post == 1) repeatOuter = true;
        else repeatOuter = false;
    }
}

// Displays overall inventory statistics (numbered for clarity)
void displayStatistics()
{
    clearScreen();
    showBreadcrumb("Store Statistics");
    showTitle("===== STORE STATISTICS =====");

    if (productCount == 0)
    {
        showInfo("No products available.");
        pauseScreen();
        return;
    }

    int totalQuantity = 0;
    double totalValue = 0.0;
    double sumOfPrices = 0.0;
    int maxIndex = 0;
    int minIndex = 0;

    for (int i = 0; i < productCount; i++)
    {
        totalQuantity += products[i].quantity;
        totalValue += products[i].price * products[i].quantity;
        sumOfPrices += products[i].price;

        if (products[i].price > products[maxIndex].price)
            maxIndex = i;
        if (products[i].price < products[minIndex].price)
            minIndex = i;
    }

    double averagePrice = sumOfPrices / productCount;

    cout << fixed << setprecision(2);
    cout << "1. Total Products         : " << productCount << "\n";
    cout << "2. Total Quantity         : " << totalQuantity << "\n";
    cout << "3. Inventory Value        : " << totalValue << "\n";
    cout << "4. Average Price          : " << averagePrice << "\n";
    cout << "5. Most Expensive Product : " << products[maxIndex].name
        << " (" << products[maxIndex].price << ")\n";
    cout << "6. Cheapest Product       : " << products[minIndex].name
        << " (" << products[minIndex].price << ")\n";
    cout << "7. Total Sales Revenue    : " << totalSalesValue << "\n";

    pauseScreen();
}

// Sorts products with expanded options (ascending/descending)
void sortProducts()
{
    clearScreen();
    showBreadcrumb("Sort Products");
    showTitle("===== SORT PRODUCTS =====");

    if (productCount == 0)
    {
        showInfo("No products available.");
        pauseScreen();
        return;
    }

    cout << "1. Name Ascending\n";
    cout << "2. Name Descending\n";
    cout << "3. Price Ascending\n";
    cout << "4. Price Descending\n";
    cout << "5. Quantity Ascending\n";
    cout << "6. Quantity Descending\n";
    cout << "7. Back\n";
    int choice = getMenuChoice("Choose sort option", 1, 7);
    if (choice == 7) return;

    for (int i = 0; i < productCount - 1; i++)
    {
        for (int j = 0; j < productCount - 1 - i; j++)
        {
            bool shouldSwap = false;
            if (choice == 1 && products[j].name > products[j + 1].name) shouldSwap = true;
            else if (choice == 2 && products[j].name < products[j + 1].name) shouldSwap = true;
            else if (choice == 3 && products[j].price > products[j + 1].price) shouldSwap = true;
            else if (choice == 4 && products[j].price < products[j + 1].price) shouldSwap = true;
            else if (choice == 5 && products[j].quantity > products[j + 1].quantity) shouldSwap = true;
            else if (choice == 6 && products[j].quantity < products[j + 1].quantity) shouldSwap = true;

            if (shouldSwap)
            {
                Product temp = products[j];
                products[j] = products[j + 1];
                products[j + 1] = temp;
            }
        }
    }

    logActivity("Sorted Products (option " + to_string(choice) + ")");
    autoSave();

    showSuccess("\nProducts sorted successfully!");
    cout << "\n1. Sort Again\n2. Return to Main Menu\n";
    int post = getMenuChoice("Choose", 1, 2);
    if (post == 1) sortProducts();
}

// Filters and displays products matching a chosen condition
void filterProducts()
{
    bool repeatOuter = true;
    while (repeatOuter)
    {
        clearScreen();
        showBreadcrumb("Filter Products");
        showTitle("===== FILTER PRODUCTS =====");

        if (productCount == 0)
        {
            showInfo("No products available.");
            pauseScreen();
            return;
        }

        cout << "1. Available\n";
        cout << "2. Low Stock\n";
        cout << "3. Out Of Stock\n";
        cout << "4. Above Price\n";
        cout << "5. Back\n";
        int choice = getMenuChoice("Choose filter option", 1, 5);
        if (choice == 5) return;

        double priceLimit = 0;
        if (choice == 4)
            priceLimit = getValidatedDouble("Enter minimum price: ", 0.0);

        cout << "\n";
        printTableHeader();

        bool found = false;

        for (int i = 0; i < productCount; i++)
        {
            bool matches = false;
            string status = getStatus(products[i].quantity);

            if (choice == 1 && status == "Available") matches = true;
            else if (choice == 2 && status == "Low Stock") matches = true;
            else if (choice == 3 && status == "Out Of Stock") matches = true;
            else if (choice == 4 && products[i].price > priceLimit) matches = true;

            if (matches)
            {
                found = true;
                printProductRow(products[i]);
            }
        }

        if (!found) showInfo("No products match this filter.");
        else logActivity("Filtered Products (option " + to_string(choice) + ")");

        cout << "\n1. Filter Again\n2. Return to Main Menu\n";
        int post = getMenuChoice("Choose", 1, 2);
        if (post == 1) repeatOuter = true;
        else repeatOuter = false;
    }
}

// ================= BONUS REPORTS =================

void displayTotalSales()
{
    clearScreen();
    showBreadcrumb("Total Sales");
    showTitle("===== TOTAL SALES =====");
    cout << fixed << setprecision(2);
    cout << "Total Revenue From Sales: " << totalSalesValue << "\n";
    pauseScreen();
}

void displayTopExpensive()
{
    clearScreen();
    showBreadcrumb("Top 5 Most Expensive");
    showTitle("===== TOP 5 MOST EXPENSIVE PRODUCTS =====");

    if (productCount == 0)
    {
        showInfo("No products available.");
        pauseScreen();
        return;
    }

    bool used[MAX_PRODUCTS] = { false };
    int limit = productCount < 5 ? productCount : 5;

    for (int k = 0; k < limit; k++)
    {
        int bestIndex = -1;
        for (int i = 0; i < productCount; i++)
        {
            if (!used[i] && (bestIndex == -1 || products[i].price > products[bestIndex].price))
                bestIndex = i;
        }
        used[bestIndex] = true;
        cout << (k + 1) << ". " << products[bestIndex].name
            << " - " << fixed << setprecision(2) << products[bestIndex].price << "\n";
    }

    pauseScreen();
}

void displayLowestStock()
{
    clearScreen();
    showBreadcrumb("Top 5 Lowest Stock");
    showTitle("===== TOP 5 LOWEST STOCK PRODUCTS =====");

    if (productCount == 0)
    {
        showInfo("No products available.");
        pauseScreen();
        return;
    }

    bool used[MAX_PRODUCTS] = { false };
    int limit = productCount < 5 ? productCount : 5;

    for (int k = 0; k < limit; k++)
    {
        int bestIndex = -1;
        for (int i = 0; i < productCount; i++)
        {
            if (!used[i] && (bestIndex == -1 || products[i].quantity < products[bestIndex].quantity))
                bestIndex = i;
        }
        used[bestIndex] = true;
        cout << (k + 1) << ". " << products[bestIndex].name
            << " - Quantity: " << products[bestIndex].quantity << "\n";
    }

    pauseScreen();
}

// LOGIN SYSTEM 

bool login()
{
    string username, password;
    int attempts = 0;

    clearScreen();
    showTitle("===== LOGIN =====");
    cout << "Welcome to the Inventory Management System\n\n";

    while (attempts < 3)
    {
        cout << "Username: ";
        username = getNonEmptyLine("");
        cout << "Password: ";
        password = getNonEmptyLine("");

        if (username == "semicolon" && password == "1234")
        {
            showSuccess("\nLogin successful!");
            loadingAnimation("Preparing your dashboard");
            return true;
        }

        attempts++;
        showError("Invalid credentials. Attempts left: " + to_string(3 - attempts));
    }

    showError("\nToo many failed attempts. Exiting program.");
    return false;
}

// WELCOME & EXIT SCREENS 

void showWelcomeScreen()
{
    clearScreen();
    cout << COLOR_BLUE;
    cout << "=========================================================\n";
    cout << " ###   #   #  ###  \n";
    cout << "  #    ## ##  #    \n";
    cout << "  #    # # #  ###  \n";
    cout << "  #    #   #     # \n";
    cout << " ###   #   #  ###  \n";
    cout << "=========================================================\n";
    cout << COLOR_RESET;
    cout << COLOR_CYAN << "\n          Inventory Management System\n" << COLOR_RESET;
    cout << "          Mansoura Robotics Training Team\n";
    cout << "          Developers: Kareem Reda, Ahmed Bakhit, Yousef Abomsalam\n\n";

    time_t now = time(0);
    cout << "Version: 1.0\n";
    cout << "Date & Time: " << ctime(&now) << "\n";

    loadingAnimation("Starting Program");
}

void showExitScreen()
{
    clearScreen();
    loadingAnimation("Closing the program");

    cout << COLOR_MAGENTA;
    cout << "=========================================================\n";
    cout << "                       THANK YOU!\n";
    cout << "=========================================================\n";
    cout << COLOR_RESET;
    cout << "Developed By:\n";
    cout << "Kareem Reda\nAhmed Bakhit\nYousef Abomsalam\n";
    cout << "Team: Mansoura Robotics Training Team\n\n";
    cout << COLOR_GREEN << "Good Luck & Good Bye!\n" << COLOR_RESET;
    cout << '\a';
}

 // MAIN FUNCTION 

int main()
{
    showWelcomeScreen();

    if (!login())
        return 0;

    // Auto-load at program start
    readDataFromFile("products.txt", false);
    logActivity("Program Started and Data Auto-Loaded");

    pauseScreen();

    int choice;
    do
    {
        displayMenu();
        // Accept the menu number; validate and reject numbers outside expected set
        choice = getValidatedInt("Enter your choice: ", 1, 32);

        // Only accept valid options, otherwise prompt again
        bool valid = false;
        int allowedOptions[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,31,32 };
        for (int i = 0; i < (int)(sizeof(allowedOptions) / sizeof(allowedOptions[0])); ++i)
        {
            if (choice == allowedOptions[i]) { valid = true; break; }
        }
        if (!valid)
        {
            showWarning("Invalid choice. Try again.");
            pauseScreen();
            continue;
        }

        switch (choice)
        {
        case 1:  addProduct(); break;
        case 2:  displayProducts(); break;
        case 3:  searchProduct(); break;
        case 4:  updateProduct(); break;
        case 5:  deleteProduct(); break;
        case 6:  sellProduct(); break;
        case 7:  restockProduct(); break;
        case 8:  displayStatistics(); break;
        case 9:  sortProducts(); break;
        case 10: filterProducts(); break;
        case 11: saveProducts(); pauseScreen(); break;
        case 12: loadProducts(); pauseScreen(); break;
        case 14: displayTotalSales(); break;
        case 15: displayTopExpensive(); break;
        case 16: displayLowestStock(); break;
        case 17: undoDelete(); break;
        case 31: displayHelpCenter(); break;
        case 32: displayAboutPage(); break;
        case 13:
        {
            char yn = getValidatedYesNo("Are you sure you want to exit?");
            if (yn == 'y' || yn == 'Y' || yn == 'Y')
            {
                autoSave();
                showExitScreen();
                choice = 13; // will exit loop
            }
            else
            {
                // do nothing, re-display menu
            }
        }
        break;
        default:
            showWarning("Invalid choice. Try again.");
            pauseScreen();
        }

    } while (choice != 13);

    return 0;
}