#pragma once
#include <memory>
#include "idatabase.h"
#include "csv_database.h"

namespace Database {
    inline std::unique_ptr<IDatabase> createDatabase() {
        return std::make_unique<CSVDatabase>();
    }
}
