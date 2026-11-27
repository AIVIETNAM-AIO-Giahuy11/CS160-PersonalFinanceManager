#include "include\data\data.h"
#include <fstream>
#include <stdexcept>

namespace {
    const std::string FILE_PATH = "data.dat";
}

namespace Data {
    void serializeData() {
        std::ofstream out(FILE_PATH, std::ios::binary);
        if (!out.is_open())
            throw std::runtime_error("Cannot open save file for writing");
            
        wallet.write(out);
        incomeSource.write(out);
        expenseCategory.write(out);
        income.write(out);
        expense.write(out);
        recurringIncome.write(out);
        recurringExpense.write(out);
    }

    void deserializeData() {
        std::ifstream in(FILE_PATH, std::ios::binary);
        if (!in.is_open())
            throw std::runtime_error("Cannot open save file for reading");

        wallet.read(in);
        incomeSource.read(in);
        expenseCategory.read(in);
        income.read(in);
        expense.read(in);
        recurringIncome.read(in);
        recurringExpense.read(in);
    }
}
