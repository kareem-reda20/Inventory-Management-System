# Inventory Management System

A professional **Console-Based Inventory Management System** developed in **C++** as part of the **Mansoura Robotics Training Team** training program.

The project provides a complete inventory management solution with a modern console interface, robust input validation, automatic file saving, colorful UI, and several bonus features.

---

##  Developers

- Kareem Reda
- Ahmed Bakhit
- Yousef Abomsalam

**Organization**

Mansoura Robotics Training Team

---

#  Features

## Product Management

- Add Product
- Display Products
- Search Product
- Update Product
- Delete Product
- Undo Delete

---

##  Inventory Operations

- Sell Products
- Restock Products
- Automatic Stock Status

Status Levels

-  Available
-  Low Stock
-  Out Of Stock

---

##  Statistics

- Total Products
- Total Quantity
- Inventory Value
- Average Product Price
- Most Expensive Product
- Cheapest Product
- Total Sales Revenue

---

##  Search

Search products by

- Product ID
- Product Name

---

##  Sorting

Sort products by

- Name (Ascending / Descending)
- Price (Ascending / Descending)
- Quantity (Ascending / Descending)

---

##  Filtering

Filter products by

- Available Products
- Low Stock
- Out Of Stock
- Products Above Specific Price

---

## Data Persistence

The inventory is automatically saved to a file.

When the program starts again:

- Products are loaded automatically.
- Total sales are restored.
- Inventory remains unchanged.

No data is lost after closing the program.

---

## Login System

The program starts with a simple login screen before accessing the inventory.

---

## User Experience

The project includes several UI improvements:

- Colorful Console Interface
- Dashboard Screen
- Welcome Screen
- Exit Screen
- Loading Animation
- Breadcrumb Navigation
- Recent Activity Log
- Success / Error Messages
- Confirmation Messages
- Sound Effects
- Professional Tables

---

##  Input Validation

The system validates every user input.

Examples:

- Invalid menu choices
- Duplicate Product IDs
- Duplicate Product Names
- Empty product names
- Negative prices
- Negative quantities
- Selling more than available stock
- Invalid numeric input
- Invalid Yes/No input

The user is never forced back to the main menu because of invalid input.

---

##  Project Structure

```
main.cpp
products.txt
README.md
```

---

#  Programming Concepts Used

- Functions
- Structs
- Arrays
- File Handling
- Bubble Sort
- Linear Search
- Input Validation
- Modular Programming
- ANSI Colors
- Console UI Design

---

#  Future Improvements

Possible future enhancements include:

- Admin/User roles
- Password encryption
- CSV/Excel export
- Sales reports
- Monthly analytics
- Product categories
- Barcode support
- Database integration (MySQL)
- GUI version using Qt
- Online inventory synchronization

---

# Built With

- C++
- Visual Studio 2022
- Standard Library

---

#  How to Run

1. Open the project in Visual Studio.
2. Build the solution.
3. Run the program.
4. Login.
5. Start managing your inventory.

---

# Screenshots

You can add screenshots here later.

Example:

```
screenshots/
    dashboard.png
    add_product.png
    statistics.png
```

---

# License

This project was developed for educational purposes as part of the Mansoura Robotics Training Team training program.

---

## Project Highlights

✔ Professional Console UI

✔ Automatic File Save & Load

✔ Undo Delete

✔ Dashboard

✔ Statistics

✔ Sorting & Filtering

✔ Login System

✔ Strong Input Validation

✔ Clean Modular Code

✔ Beginner-Friendly Architecture
