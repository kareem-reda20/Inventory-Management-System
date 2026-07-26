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

# 15. Function Contracts

The following function signatures are the official interfaces used by the entire team.

Every team member must use these signatures exactly.

```cpp
void displayMenu();

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
```

## Return Rules

### searchByID()

Returns:

- Product index if found.
- -1 if product does not exist.

---

### searchByName()

Returns:

- Product index if found.
- -1 if product does not exist.

---

All other functions return `void`.

---

# 16. Search Rules

## Search by ID

- Search using Linear Search.
- IDs must be unique.
- Return the product index.
- If not found, return -1.

---

## Search by Name

- Exact Match only.
- Case Sensitive.
- Return the product index.
- If not found, return -1.

Example

Searching for:

Laptop

Matches:

Laptop

Does NOT match:

laptop

LAPTOP

Laptop Pro

---

# 17. Sort Rules

When the user chooses "Sort Products", display the following submenu:

1. Sort by Name (A → Z)

2. Sort by Price (Low → High)

3. Sort by Quantity (Low → High)

Use Bubble Sort.

The entire Product object must be swapped, not individual fields.

---

# 18. Filter Rules

When the user chooses "Filter Products", display:

1. Available Products

2. Low Stock Products

3. Out of Stock Products

4. Products Above Specific Price

Definitions

Available

Quantity >= 5

---

Low Stock

Quantity > 0 and Quantity < 5

---

Out of Stock

Quantity == 0

---

Above Price

Display all products with price greater than the user input.

---

# 19. Statistics Rules

Display the following information:

- Total Number of Products

- Total Quantity in Inventory

- Total Inventory Value

- Average Product Price

- Most Expensive Product

- Cheapest Product

Inventory Value

= Sum(price × quantity)

Average Price

= Sum(all prices) / productCount

---

# 20. Update Rules

Update Product allows editing all product information.

The user enters the Product ID.

If the product exists:

- Enter New Name

- Enter New Price

- Enter New Quantity

- All three fields are mandatory and must be re-entered,
even if the value does not change.

The Product ID cannot be changed.

---

# 21. Delete Rules

Delete Product asks for Product ID.

If found:

Display product information.

Ask for confirmation.

Delete? (Y/N)

If Y

Delete the product.

Shift remaining products one position left.

Decrease productCount.

If N

Cancel the operation.

---

# 22. Bonus Rules (Optional)

Bonus features are implemented only after all required features work correctly.

Recommended order:

1. Save Products to File

2. Load Products from File

3. Total Sales

4. Top 5 Most Expensive Products

5. Top 5 Lowest Stock Products

6. Login System

The file format for Save/Load will be decided only when the team starts implementing the bonus.

□ Ready for demonstration

