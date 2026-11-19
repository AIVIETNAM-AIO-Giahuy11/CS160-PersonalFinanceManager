// include/service/RecurringService.h
#pragma once
#include "core/Date.h"
#include "data/data.h"

namespace RecurringService {

Date scheduledDateForMonth(const Date& startDate, int year, int month);

bool addRecurringIncome(const RecurringIncome& rule);
bool addRecurringExpense(const RecurringExpense& rule);
bool deleteRecurringIncome(int id);
bool deleteRecurringExpense(int id);

// Trả về số giao dịch vừa được tự động sinh cho tháng của `today`, hoặc -1 nếu vượt giới hạn
int syncForCurrentMonth(const Date& today);

} // namespace RecurringService