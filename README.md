# Inventory Management System
## Project Specification v1.0

**Team:** Mansoura Robotics Training Team

**Project Type:** Console Application (C++)

**Language:** C++

**Team Leader:** Kareem

---

# 1. Project Objective

Develop a console-based Inventory Management System that allows the user to manage store products.

The project must satisfy all required features in the provided assignment while maintaining:

- Clean Code
- Readability
- Input Validation
- Modular Functions
- Consistent Naming

---

# 2. Development Rules

Before writing any code, every team member must follow these rules.

## DO

✔ Use the same variable names.

✔ Use the same function names.

✔ Keep functions independent.

✔ Test every function before sending it.

✔ Inform the team leader before changing any shared code.

---

## DON'T

❌ Rename variables.

❌ Rename functions.

❌ Modify the Product struct.

❌ Create additional global arrays.

❌ Change MAX_PRODUCTS.

❌ Change the menu numbering.

---

# 3. Project Architecture

```
                     main()

                       │

                  displayMenu()

                       │

               switch(userChoice)

                       │

 ┌─────────────────────────────────────────────┐
 │                                             │
 │ Add Product                                 │
 │ Display Products                            │
 │ Search Product                              │
 │ Update Product                              │
 │ Delete Product                              │
 │ Sell Product                                │
 │ Restock Product                             │
 │ Statistics                                  │
 │ Sort                                        │
 │ Filter                                      │
 │ Exit                                        │
 │                                             │
 └─────────────────────────────────────────────┘

                       │

                products[MAX_PRODUCTS]

```

All functions work on the same shared products array.

No function should create another inventory.

---

# 4. Constants

```cpp
const int MAX_PRODUCTS = 100;
```

No one is allowed to modify this constant.

---

# 5. Product Structure

```cpp
struct Product
{
    int id;
    string name;
    double price;
    int quantity;
};
```

Every function must use this structure.

No member should modify it.

---

# 6. Global Variables

```cpp
Product products[MAX_PRODUCTS];

int productCount = 0;
```

Bonus Variables (Only if bonus is implemented)

```cpp
double totalSales = 0;
```

No additional global inventory variables are allowed.

---

# 7. Naming Convention

Use **camelCase** for everything.

## Variables

```cpp
productCount

userChoice

searchID

newPrice

addedQuantity

soldQuantity
```

---

## Functions

```cpp
displayMenu()

addProduct()

displayProducts()

searchByID()

searchByName()

updateProduct()

deleteProduct()

sellProduct()

restockProduct()

displayStatistics()

sortProducts()

filterProducts()
```

---

## Struct

Always

```cpp
Product
```

Never

```cpp
product

PRODUCT

Items
```

---

# 8. Menu Layout

```
========== Inventory Management System ==========

1. Add Product

2. Display Products

3. Search Product

4. Update Product

5. Delete Product

6. Sell Product

7. Restock Product

8. Store Statistics

9. Sort Products

10. Filter Products

11. Exit

===============================================

Enter your choice:
```

The menu numbering must never change.

---

# 9. Product Status Rules

Quantity == 0

→ Out of Stock

Quantity < 5

→ Low Stock

Otherwise

→ Available

---

# 10. Input Rules

Product ID

✔ Positive Integer

✔ Must be unique

---

Product Name

✔ Cannot be empty

---

Price

✔ Greater than zero

---

Quantity

✔ Greater than or equal to zero

---

Sell Quantity

✔ Greater than zero

✔ Cannot exceed current stock

---

Restock Quantity

✔ Greater than zero

---

# 11. Output Style

Use clear messages.

Examples

```
Product added successfully.

Product deleted successfully.

Product updated successfully.

Product not found.

Invalid input.

Inventory is full.
```

Avoid random message styles.

---

# 12. Shared Coding Rules

Each function should do ONE job only.

Avoid duplicated code.

Keep code readable.

Add comments only when needed.

Example

```cpp
// Search product by ID
```

---

# 13. Team Responsibilities

Leader

• Main Menu

• Add Product

• Delete Product

• Sort Products

• Integration

• Final Testing

---

Member 1

• Display Products

• Search

• Filter

---

Member 2

• Update

• Sell

• Restock

• Statistics

---

Bonus

Implemented only after all required features work correctly.

Priority

1. Save to File

2. Load from File

3. Total Sales

4. Top 5 Most Expensive

5. Top 5 Lowest Stock

6. Login System

---

# 14. Completion Checklist

□ All required features completed

□ All validations completed

□ No compilation errors

□ No runtime errors

□ All functions tested

□ Code reviewed

□ Bonus implemented (optional)

□ Ready for demonstration

