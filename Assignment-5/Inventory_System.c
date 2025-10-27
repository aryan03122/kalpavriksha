#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN 50

typedef struct {
    int id;
    char name[NAME_LEN];
    float price;
    int quantity;
} Product;

void inputProducts(Product *products, int n);
void displayProduct(Product p);
void viewAllProducts(Product *products, int count);
void addNewProduct(Product **products, int *count);
void updateQuantity(Product *products, int count);
void searchByID(Product *products, int count);
void searchByName(Product *products, int count);
void searchByPriceRange(Product *products, int count);
void deleteProduct(Product **products, int *count);
void showMenu();

void inputProducts(Product *products, int n) {
    for (int i = 0; i < n; i++) {
        printf("\nEnter details for product %d:\n", i + 1);
        printf("Product ID: ");
        scanf("%d", &products[i].id);
        printf("Product Name: ");
        scanf("%s", products[i].name);
        printf("Product Price: ");
        scanf("%f", &products[i].price);
        printf("Product Quantity: ");
        scanf("%d", &products[i].quantity);
    }
}

void displayProduct(Product p) {
    printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
           p.id, p.name, p.price, p.quantity);
}

void viewAllProducts(Product *products, int count) {
    if (count == 0) {
        printf("No products available.\n");
        return;
    }

    printf("\n========= PRODUCT LIST =========\n");
    for (int i = 0; i < count; i++) {
        displayProduct(products[i]);
    }
}

void addNewProduct(Product **products, int *count) {
    *products = realloc(*products, (*count + 1) * sizeof(Product));
    if (*products == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    printf("\nEnter new product details:\n");
    printf("Product ID: ");
    scanf("%d", &(*products)[*count].id);
    printf("Product Name: ");
    scanf("%s", (*products)[*count].name);
    printf("Product Price: ");
    scanf("%f", &(*products)[*count].price);
    printf("Product Quantity: ");
    scanf("%d", &(*products)[*count].quantity);
    (*count)++;
    printf("Product added!\n");
}

void updateQuantity(Product *products, int count) {
    int id, newQty, found = 0;
    printf("Enter Product ID to update quantity: ");
    scanf("%d", &id);

    for (int i = 0; i < count; i++) {
        if (products[i].id == id) {
            printf("Enter new Quantity: ");
            scanf("%d", &newQty);
            products[i].quantity = newQty;
            printf("Quantity updated successfully!\n");
            found = 1;
            break;
        }
    }

    if (!found)
        printf("Product not found.\n");
}

void searchByID(Product *products, int count) {
    int id, found = 0;
    printf("Enter Product ID to search: ");
    scanf("%d", &id);

    for (int i = 0; i < count; i++) {
        if (products[i].id == id) {
            printf("Product Found:\n");
            displayProduct(products[i]);
            found = 1;
            break;
        }
    }

    if (!found)
        printf("Product not found.\n");
}

void searchByName(Product *products, int count) {
    char name[NAME_LEN];
    int found = 0;
    printf("Enter name to search: ");
    scanf("%s", name);

    printf("Products Found:\n");
    for (int i = 0; i < count; i++) {
        if (strstr(products[i].name, name)) {
            displayProduct(products[i]);
            found = 1;
        }
    }

    if (!found)
        printf("No products match that name.\n");
}

void searchByPriceRange(Product *products, int count) {
    float min, max;
    int found = 0;

    printf("Enter minimum price: ");
    scanf("%f", &min);
    printf("Enter maximum price: ");
    scanf("%f", &max);

    printf("Products in price range:\n");
    for (int i = 0; i < count; i++) {
        if (products[i].price >= min && products[i].price <= max) {
            displayProduct(products[i]);
            found = 1;
        }
    }

    if (!found)
        printf("No products in the given range.\n");
}

void deleteProduct(Product **products, int *count) {
    int id, found = 0;
    printf("Enter Product ID to delete: ");
    scanf("%d", &id);

    for (int i = 0; i < *count; i++) {
        if ((*products)[i].id == id) {
            for (int j = i; j < *count - 1; j++) {
                (*products)[j] = (*products)[j + 1];
            }
            (*count)--;
            *products = realloc(*products, (*count) * sizeof(Product));
            printf("Product deleted successfully!\n");
            found = 1;
            break;
        }
    }

    if (!found)
        printf("Product not found.\n");
}

void showMenu() {
    printf("\n========= INVENTORY MENU =========\n");
    printf("1. Add New Product\n");
    printf("2. View All Products\n");
    printf("3. Update Quantity\n");
    printf("4. Search Product by ID\n");
    printf("5. Search Product by Name\n");
    printf("6. Search Product by Price Range\n");
    printf("7. Delete Product\n");
    printf("8. Exit\n");
}

int main() {
    int n, choice;
    Product *products = NULL;

    printf("Enter initial number of products: ");
    scanf("%d", &n);

    products = (Product *)calloc(n, sizeof(Product));
    if (products == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    inputProducts(products, n);

    while (1) {
        showMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addNewProduct(&products, &n);
                break;
            case 2:
                viewAllProducts(products, n);
                break;
            case 3:
                updateQuantity(products, n);
                break;
            case 4:
                searchByID(products, n);
                break;
            case 5:
                searchByName(products, n);
                break;
            case 6:
                searchByPriceRange(products, n);
                break;
            case 7:
                deleteProduct(&products, &n);
                break;
            case 8:
                free(products);
                printf("Exiting program");
                return 0;
            default:
                printf("Invalid choice!\n");
        }
    }
}
