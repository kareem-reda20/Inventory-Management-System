/******************************************************************
Project : Inventory Management System
Team    : Mansoura Robotics

---------------
Follow PROJECT_SPEC.md exactly.
Do NOT:
- Rename functions
- Rename variables
- Modify Product struct
- Modify global variables
- Change menu numbering
Implement ONLY the assigned function.
******************************************************************/

#include <iostream>
#include <string>

using namespace std;

//==================================================
// Constants
//==================================================
const int MAX_PRODUCTS = 100;

//==================================================
// Structures
//==================================================
struct Product
{
    int id;
    string name;
    double price;
    int quantity;
};

//==================================================
// Global Variables
//==================================================
Product products[MAX_PRODUCTS];
int productCount = 0;

//==================================================
// Function Prototypes
//==================================================
void displayMenu();
void searchProduct();

void addProduct();
void displayProducts();

int searchByID(int id);
int searchByName(string name);

void updateProduct();
void deleteProduct();
void sellProduct();
void restockProduct();

void displayStatistics();
void sortProducts();
void filterProducts();

//==================================================
// Main
//==================================================
int main()
{
    int userChoice;

    do
    {
        displayMenu();

        cout << "\nEnter your choice: ";
        cin >> userChoice;

        switch (userChoice)
        {
        case 1: addProduct(); break;
        case 2: displayProducts(); break;
        case 3: searchProduct(); break;
        case 4: updateProduct(); break;
        case 5: deleteProduct(); break;
        case 6: sellProduct(); break;
        case 7: restockProduct(); break;
        case 8: displayStatistics(); break;
        case 9: sortProducts(); break;
        case 10: filterProducts(); break;
        case 11:
            cout << "\nThank you for using the Inventory Management System.\n";
            break;
        default:
            cout << "\nInvalid choice.\n";
        }

    } while (userChoice != 11);

    return 0;
}

//==================================================
// Shared Functions
//==================================================

// Owner: Kareem
// Displays the main menu.
void displayMenu()
{
    cout << "\n========== Inventory Management System ==========\n";
    cout << "1. Add Product\n";
    cout << "2. Display Products\n";
    cout << "3. Search Product\n";
    cout << "4. Update Product\n";
    cout << "5. Delete Product\n";
    cout << "6. Sell Product\n";
    cout << "7. Restock Product\n";
    cout << "8. Store Statistics\n";
    cout << "9. Sort Products\n";
    cout << "10. Filter Products\n";
    cout << "11. Exit\n";
    cout << "===============================================\n";
}

// Owner: Ahmed Bikhet
// TODO:
// Display search submenu.
// 1- Search by ID
// 2- Search by Name
// Call searchByID() or searchByName() only.
void searchProduct()
{

}

//==================================================
// Kareems Functions
//==================================================

// TODO:
// Validate inventory capacity.
// Validate unique ID.
// Validate name.
// Validate price.
// Validate quantity.
// Add product.
// Increase productCount.
void addProduct()
{

}

// TODO:
// Find product.
// Confirm deletion.
// Shift array.
// Decrease productCount.
void deleteProduct()
{

}

// TODO:
// Show sort menu.
// 1- Name (A-Z)
// 2- Price (Low-High)
// 3- Quantity (Low-High)
// Use Bubble Sort.
// Swap complete Product objects.
void sortProducts()
{

}

//==================================================
// Ahmeds Functions
//==================================================

// TODO:
// Display all products in table format.
// Show product status.
void displayProducts()
{

}

// Returns index if found, otherwise -1.
int searchByID(int id)
{
    return -1;
}

// Returns index if found, otherwise -1.
int searchByName(string name)
{
    return -1;
}

// TODO:
// Filter:
// Available
// Low Stock
// Out of Stock
// Above Price
void filterProducts()
{

}

//==================================================
// Yousefs Functions
//==================================================

// TODO:
// Search product.
// Update name, price and quantity.
// Product ID cannot change.
void updateProduct()
{

}

// TODO:
// Search product.
// Validate stock.
// Sell quantity.
void sellProduct()
{

}

// TODO:
// Search product.
// Increase quantity.
void restockProduct()
{

}

// TODO:
// Display:
// Total Products
// Total Quantity
// Inventory Value
// Average Price
// Highest Price
// Lowest Price
void displayStatistics()
{

}

//==================================================
// Bonus Functions (Optional)
//==================================================
// SaveToFile()
// LoadFromFile()
// LoginSystem()
// TotalSales()
// Top5Expensive()
// Top5LowestStock()
