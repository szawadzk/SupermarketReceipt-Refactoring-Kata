#include "ShoppingCart.h"

void addItemQuantity(const Product& product, double quantity);

std::vector<ProductQuantity> ShoppingCart::getItems() const {
    return items;
}

std::map<Product, double> ShoppingCart::getProductQuantities() const {
    return productQuantities;
}

void ShoppingCart::addItem(const Product& product) {
    addItemQuantity(product, 1.0);
}

void ShoppingCart::addItemQuantity(const Product& product, double quantity) {
    items.emplace_back(product, quantity);
    if (productQuantities.find(product) != productQuantities.end()) {
        productQuantities[product] += quantity;
    } else {
        productQuantities[product] = quantity;
    }
}

int ShoppingCart::ComputeMinQuantity(Offer &offer, \
                double quantity, \
                Product &product, \
                SupermarketCatalog* catalog)
{
    int quantityAsInt = (int) quantity;
    double unitPrice = catalog->getUnitPrice(product);
    int minQuantity = 1;

    if (offer.getOfferType() == SpecialOfferType::ThreeForTwo) 
    {
        minQuantity = 3;
    } 
    else if (offer.getOfferType() == SpecialOfferType::TwoForAmount) 
    {
        minQuantity = 2;
    } 
    else if (offer.getOfferType() == SpecialOfferType::FiveForAmount) 
    {
        minQuantity = 5;
    }

    return minQuantity;
}

Discount *ShoppingCart::createDiscount(Offer &offer, \
                double quantity, \
                Product &product, \
                int minQuantity, \
                SupermarketCatalog* catalog)
{
    int quantityAsInt = (int) quantity;
    int numberOfDiscounts = quantityAsInt / minQuantity;
    double unitPrice = catalog->getUnitPrice(product);

    Discount* discount = nullptr;
    if (offer.getOfferType() == SpecialOfferType::TwoForAmount && quantityAsInt >= 2) 
    {
        double total = offer.getArgument() * (quantityAsInt / minQuantity) + quantityAsInt % 2 * unitPrice;
        double discountN = unitPrice * quantity - total;
        discount = new Discount("2 for " + std::to_string(offer.getArgument()), -discountN, product);
    }
    else if (offer.getOfferType() == SpecialOfferType::ThreeForTwo && quantityAsInt > 2) 
    {
        double discountAmount = quantity * unitPrice - ((numberOfDiscounts * 2 * unitPrice) + quantityAsInt % 3 * unitPrice);
        discount = new Discount("3 for 2", -discountAmount, product);
    }
    else if (offer.getOfferType() == SpecialOfferType::TenPercentDiscount) 
    {
        discount = new Discount(std::to_string(offer.getArgument()) + "% off", -quantity * unitPrice * offer.getArgument() / 100.0, product);
    }
    else if (offer.getOfferType() == SpecialOfferType::FiveForAmount && quantityAsInt >= 5) 
    {
        double discountTotal = unitPrice * quantity - (offer.getArgument() * numberOfDiscounts + quantityAsInt % 5 * unitPrice);
        discount = new Discount(std::to_string(minQuantity) + " for " + std::to_string(offer.getArgument()), -discountTotal, product);
    }

    return discount;    
}
/*
    Code smells:
    - long method
        - extract method: ComputeMinQuantity, createDiscount
*/
void ShoppingCart::handleOffers(Receipt& receipt, std::map<Product, Offer> offers, SupermarketCatalog* catalog) {
    for (const auto& productQuantity : productQuantities) {
        Product product = productQuantity.first;
        double quantity = productQuantity.second;
        if (offers.find(product) != offers.end()) {
            auto offer = offers[product];
            double unitPrice = catalog->getUnitPrice(product);
            int quantityAsInt = (int) quantity;
            Discount* discount = nullptr;

            int minQuantity = ComputeMinQuantity(offer, quantity, product, catalog);

            discount = createDiscount(offer, quantity, product, minQuantity, catalog);
            if (discount != nullptr)
                receipt.addDiscount(*discount);
        }
    }
}